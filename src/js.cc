#include "js.h"

#include <sstream>

#include <GLFW/glfw3.h>
#include <v8/include/libplatform/libplatform.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <v8/src/debug/debug-interface.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include "args.h"
#include "file.h"
#include "js_scope.h"
#include "json.h"
#include "util.h"
#include "zip.h"

extern const std::string_view kEmbeddedConsoleSource;
extern const std::string_view kEmbeddedDefaultSource;

namespace {

v8::Platform* platform = nullptr;

void AppendModulePath(std::stringstream* ss,
                      const std::filesystem::path& base_path,
                      const std::vector<std::filesystem::path>& paths) {
  if (paths.empty()) {
    return;
  }
  int i = paths.size() - 1;
  *ss << "    loading "
      << std::filesystem::relative(paths[i], base_path).string() << "\n";
  for (i--; i >= 0; i--) {
    *ss << "       from "
        << std::filesystem::relative(paths[i], base_path).string() << "\n";
  }
}

bool IsValidImport(std::string_view name) {
  return StartsWith(name, "./") || StartsWith(name, "../");
}

std::string MakeInvalidImportError(std::string_view name) {
  std::stringstream ss;
  ss << "Invalid module name: '" << name
     << "'. Valid imports must begin with ./ or ../";
  return ss.str();
}

v8::Local<v8::Message> MakeErrorMessage(v8::Isolate* isolate,
                                        v8::Local<v8::Value> exception) {
  v8::Local<v8::Message> message =
      v8::Exception::CreateMessage(isolate, exception);
  if (!exception->IsNativeError() &&
      (message.IsEmpty() || message->GetStackTrace().IsEmpty())) {
    // Create a stack trace manually if it's missing.
    exception = v8::Exception::Error(
        v8::String::NewFromUtf8Literal(isolate, "Uncaught Exception."));
    message = v8::Exception::CreateMessage(isolate, exception);
  }
  return message;
}

}  // namespace

// static
void Js::Init(const char* program) {
#if !defined(WINDOWJS_RELEASE_BUILD)
  // Enables calling RequestGarbageCollectionForTesting to catch memory leaks
  // at shutdown.
  v8::V8::SetFlagsFromString("--expose_gc");
#endif
  platform = v8::platform::NewDefaultPlatform().release();
  ASSERT(v8::V8::InitializeICUDefaultLocation(program));
  v8::V8::InitializeExternalStartupData(program);
  v8::V8::InitializePlatform(platform);
  ASSERT(v8::V8::Initialize());
}

// static
void Js::Shutdown() {
  v8::V8::Dispose();
  v8::V8::DisposePlatform();
  delete platform;
}

// static
double Js::MonotonicallyIncreasingTime() {
  return platform->MonotonicallyIncreasingTime();
}

Js::Js(Delegate* delegate, std::filesystem::path base_path,
       TaskQueue* task_queue)
    : delegate_(delegate),
      base_path_(std::move(base_path)),
      task_queue_(task_queue),
      suppress_next_script_result_(false) {
  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] create JS context start: " << glfwGetTime();
  }

  allocator_ = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  console_delegate_ = MakeConsoleDelegate(this);

  v8::Isolate::CreateParams params;
  params.array_buffer_allocator = allocator_;

  isolate_ = v8::Isolate::New(params);
  // The number of data slots is a fixed constant in v8. Make sure
  // we have enough for our usage:
  // 0 is Js*.
  // 1 is JsApi*.
  ASSERT(isolate_->GetNumberOfDataSlots() == 4);
  isolate_->SetData(0, this);
  isolate_->SetCaptureStackTraceForUncaughtExceptions(true);
  isolate_->SetHostImportModuleDynamicallyCallback(ImportDynamic);
  isolate_->SetPromiseRejectCallback(HandlePromiseRejectCallback);

  v8::debug::SetConsoleDelegate(isolate_, console_delegate_.get());

  v8::Locker locker(isolate_);
  v8::Isolate::Scope isolate_scope(isolate_);
  v8::HandleScope handle_scope(isolate_);
  context_.Reset(isolate_, v8::Context::New(isolate_));

  strings_ = std::make_unique<JsStrings>(isolate_);

  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] create JS context end: " << glfwGetTime();
  }
}

Js::~Js() {
  isolate_->SetData(0, nullptr);
  strings_.reset();
  dynamic_imports_.clear();
  modules_.clear();
  context_.Reset();
#if !defined(WINDOWJS_RELEASE_BUILD)
  isolate_->RequestGarbageCollectionForTesting(
      v8::Isolate::kFullGarbageCollection);
#endif
  isolate_->Dispose();
  delete allocator_;
}

v8::Local<v8::String> Js::MakeString(std::string_view s) {
  return v8::String::NewFromUtf8(isolate_, s.data(), v8::NewStringType::kNormal,
                                 s.size())
      .ToLocalChecked();
}

std::string Js::ToString(v8::Local<v8::Value> value) {
  ASSERT(!value.IsEmpty());
  if (value->IsString()) {
    v8::Local<v8::String> v8s = value.As<v8::String>();
    std::string s;
    s.resize(v8s->Utf8Length(isolate_));
    v8s->WriteUtf8(isolate_, s.data());
    return s;
  } else if (value->IsModuleNamespaceObject()) {
    return "[Module]";
  } else if (value->IsSymbolObject()) {
    return "[SymbolObject]";
  } else if (value->IsSymbol()) {
    return ToString(value.As<v8::Symbol>()->Description(isolate_));
  } else {
    v8::String::Utf8Value v(isolate_, value);
    if (*v == nullptr) {
      return ToString(value->TypeOf(isolate_));
    } else {
      return std::string(*v, v.length());
    }
  }
}

std::string Js::ToStringOr(v8::Local<v8::Value> value,
                           std::string_view or_string) {
  if (value.IsEmpty()) {
    return {or_string.data(), or_string.size()};
  } else {
    return ToString(value);
  }
}

bool Js::GetBooleanOr(v8::Local<v8::Object> object, std::string_view key,
                      bool fallback) {
  v8::MaybeLocal<v8::Value> value = object->Get(context(), MakeString(key));
  if (!value.IsEmpty()) {
    v8::Local<v8::Value> v = value.ToLocalChecked();
    if (v->IsBoolean()) {
      return v.As<v8::Boolean>()->Value();
    }
  }
  return fallback;
}

void Js::ThrowError(std::string_view error) {
  isolate_->ThrowError(MakeString(error));
}

void Js::ThrowTypeError(std::string_view error) {
  isolate_->ThrowException(v8::Exception::TypeError(MakeString(error)));
}

void Js::ThrowIllegalConstructor() {
  ThrowTypeError("Illegal constructor");
}

void Js::ThrowIllegalInvocation() {
  ThrowTypeError("Illegal invocation");
}

void Js::ThrowInvalidArgument() {
  ThrowError("Invalid argument");
}

void Js::ReportException(v8::Local<v8::Message> message) {
  std::string str = ToString(message->Get());

  std::vector<std::string> trace;
  v8::Local<v8::StackTrace> stack_trace = message->GetStackTrace();
  if (!stack_trace.IsEmpty()) {
    int count = stack_trace->GetFrameCount();
    for (int i = 0; i < count; i++) {
      v8::Local<v8::StackFrame> frame = stack_trace->GetFrame(isolate_, i);
      std::string file = ToStringOr(frame->GetScriptName(), "<script>");
      std::string function = ToStringOr(frame->GetFunctionName(), "<top>");
      int line = frame->GetLineNumber();
      std::stringstream ss;
      ss << function << " (" << file << ":" << line << ")";
      trace.emplace_back(ss.str());
    }
  }

  delegate_->OnJavascriptException(std::move(str), std::move(trace));
}

void Js::SuppressNextScriptResult() {
  suppress_next_script_result_ = true;
}

std::unique_ptr<std::string> Js::ExecuteScript(std::string_view source) {
  JsScope scope(this);
  v8::TryCatch try_catch(scope.isolate);

  // This is the ResourceName used in ImportDynamic below.
  auto resource_name = MakeString("<console>");
  constexpr int line_offset = 0;
  constexpr int column_offset = 0;
  constexpr bool is_shared_cross_origin = false;
  constexpr int script_id = -1;
  auto source_map_url = v8::Local<v8::Value>();
  constexpr bool is_opaque = false;
  constexpr bool is_warm = false;
  constexpr bool is_module = false;
  auto host_defined_options = v8::Local<v8::PrimitiveArray>();
  v8::ScriptOrigin origin(isolate_, resource_name, line_offset, column_offset,
                          is_shared_cross_origin, script_id, source_map_url,
                          is_opaque, is_warm, is_module, host_defined_options);

  v8::Local<v8::String> s = MakeString(source);
  v8::MaybeLocal<v8::Script> script =
      v8::Script::Compile(scope.context, s, &origin);

  if (try_catch.HasCaught()) {
    ReportException(try_catch.Message());
    return {};
  }

  v8::MaybeLocal<v8::Value> result =
      script.ToLocalChecked()->Run(scope.context);

  if (try_catch.HasCaught()) {
    suppress_next_script_result_ = false;
    ReportException(try_catch.Message());
    return {};
  }

  if (suppress_next_script_result_) {
    suppress_next_script_result_ = false;
    return {};
  }

  return std::make_unique<std::string>(ToString(result.ToLocalChecked()));
}

void Js::LoadMainModule(std::string_view name) {
  v8::Locker locker(isolate_);
  JsScope scope(this);
  v8::TryCatch try_catch(isolate_);

  std::filesystem::path path =
      name.substr(0, 2) == "--" ? name : (base_path_ / name).lexically_normal();

  LoadModuleByPath(std::move(path), {});

  if (try_catch.HasCaught()) {
    ReportException(try_catch.Message());
    delegate_->OnMainModuleLoaded();
  }
}

// LoadModuleByPath is used in two flows:
// 1. Loading the main module
// 2. Loading a dynamically imported module.
//
// The "resolver" is set for the second case.
//
// LoadModuleByPath returns false if the module couldn't be located,
// instantiated, etc. In those cases, an exception has been thrown.
//
// Otherwise, the module is either fully loaded and ready, or has a top-level
// await and is still pending. In those case, a resolver or reject handler
// will handle cases 1 and 2 later.
bool Js::LoadModuleByPath(std::filesystem::path path,
                          v8::Local<v8::Promise::Resolver> resolver) {
  std::vector<std::filesystem::path> paths;
  paths.push_back(path);

  v8::Local<v8::Context> context = this->context();

  v8::Local<v8::Module> module = LoadModuleTree(context, path, &paths);
  if (module.IsEmpty()) {
    return false;
  }

  // At this stage, the module is compiled but not instantiated yet.
  // Instantiate it now; this will recursively instantiate its dependencies
  // that haven't been instantiated yet.

  if (!module->InstantiateModule(context, ResolveModule).FromMaybe(false)) {
    return false;
  }

  v8::Local<v8::Value> result;
  if (!module->Evaluate(context).ToLocal(&result)) {
    return false;
  }

  v8::Local<v8::Promise> promise = result.As<v8::Promise>();

  if (resolver.IsEmpty()) {
    // Loading the main module.
    if (promise->State() == v8::Promise::kPending) {
      IGNORE_RESULT(promise->Then(
          context,
          v8::Function::New(context, OnMainModuleResolve).ToLocalChecked(),
          v8::Function::New(context, OnMainModuleFailure).ToLocalChecked()));
    } else {
      delegate_->OnMainModuleLoaded();
    }
  } else {
    // Dynamic import: pass the result to the resolver.
    v8::Local<v8::Value> ns = module->GetModuleNamespace();
    if (promise->State() == v8::Promise::kPending) {
      v8::Local<v8::Array> data = v8::Array::New(isolate_, 2);
      ASSERT(data->Set(context, 0, resolver).FromMaybe(false));
      ASSERT(data->Set(context, 1, ns).FromMaybe(false));
      IGNORE_RESULT(promise->Then(
          context,
          v8::Function::New(context, OnDynamicModuleResolve, data)
              .ToLocalChecked(),
          v8::Function::New(context, OnDynamicModuleFailure, resolver)
              .ToLocalChecked()));
    } else if (promise->State() == v8::Promise::kFulfilled) {
      ASSERT(resolver->Resolve(context, ns).FromMaybe(false));
    } else {
      RemovePendingFailedPromise(promise);
      ASSERT(resolver->Reject(context, promise->Result()).FromMaybe(false));
    }
  }

  return true;
}

v8::Local<v8::Module> Js::LoadModuleTree(
    v8::Local<v8::Context> context, const std::filesystem::path& path,
    std::vector<std::filesystem::path>* paths) {
  // Check for recursive loading of the same module.
  // Note that recursive imports are supported: we only load module names that
  // haven't been loaded yet (recursively) below.
  auto it = modules_.find(path.string());
  if (it != modules_.end()) {
    return it->second.Get(isolate_);
  }

  v8::Local<v8::String> source = LoadModuleSource(path, *paths);
  if (source.IsEmpty()) {
    return {};
  }

  v8::Local<v8::Module> module = CompileModule(source, path, *paths);
  if (module.IsEmpty()) {
    return {};
  }

  // At this stage, the module is compiled but not instantiated yet.
  // Look up its dependencies, so that they can be instantiated later too.

  ASSERT(module_path_by_id_.find(module->ScriptId()) ==
         module_path_by_id_.end());
  modules_[path.string()] = v8::Global<v8::Module>(isolate_, module);
  module_path_by_id_[module->ScriptId()] = path.string();

  std::filesystem::path dir = path;
  dir.remove_filename();

  v8::Local<v8::FixedArray> requests = module->GetModuleRequests();
  int len = requests->Length();
  for (int i = 0; i < len; i++) {
    v8::Local<v8::ModuleRequest> request =
        requests->Get(context, i).As<v8::ModuleRequest>();
    std::string spec = ToString(request->GetSpecifier());
    if (!IsValidImport(spec)) {
      ThrowError(MakeInvalidImportError(spec));
      return {};
    }
    std::filesystem::path subpath = (dir / spec).lexically_normal();
    if (modules_.find(subpath.string()) == modules_.end()) {
      paths->push_back(subpath);
      bool failed = LoadModuleTree(context, subpath, paths).IsEmpty();
      paths->pop_back();
      if (failed) {
        return {};
      }
    }
  }

  return module;
}

// static
v8::MaybeLocal<v8::Module> Js::ResolveModule(
    v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
    v8::Local<v8::FixedArray> import_assertions,
    v8::Local<v8::Module> referrer) {
  Js* js = Js::Get(context->GetIsolate());

  auto it = js->module_path_by_id_.find(referrer->ScriptId());
  ASSERT(it != js->module_path_by_id_.end());
  std::filesystem::path referrer_dir = it->second;
  referrer_dir.remove_filename();

  std::string spec = js->ToString(specifier);
  std::filesystem::path path = (referrer_dir / spec).lexically_normal();

  auto it2 = js->modules_.find(path.string());
  ASSERT(it2 != js->modules_.end());

  return it2->second.Get(js->isolate_);
}

v8::Local<v8::String> Js::LoadModuleSource(
    const std::filesystem::path& path,
    const std::vector<std::filesystem::path>& paths) {
  std::string content;
  if (path == "--console") {
    content = GzipUncompress(kEmbeddedConsoleSource);
  } else if (path == "--default") {
    content = GzipUncompress(kEmbeddedDefaultSource);
  } else if (path.string().substr(0, 2) == "--") {
    ThrowError("Invalid module name: " + path.string());
    return {};
  } else {
    std::string error;
    if (!ReadFile(path, &content, &error)) {
      std::stringstream ss;
      ss << error << "\n";
      AppendModulePath(&ss, base_path_, paths);
      ThrowError(ss.str());
      return {};
    }
  }

  return MakeString("const __filename = " + Json::EscapeString(path.string()) +
                    ";const __dirname = " +
                    Json::EscapeString(Dirname(path).string()) + ";" + content);
}

v8::Local<v8::Module> Js::CompileModule(
    v8::Local<v8::String> source, const std::filesystem::path& path,
    const std::vector<std::filesystem::path>& paths) {
  // This is the ResourceName used in ImportDynamic below.
  auto resource_name = MakeString(path.string());
  constexpr int line_offset = 0;
  constexpr int column_offset = 0;
  constexpr bool is_shared_cross_origin = false;
  constexpr int script_id = -1;
  auto source_map_url = v8::Local<v8::Value>();
  constexpr bool is_opaque = false;
  constexpr bool is_warm = false;
  constexpr bool is_module = true;
  auto host_defined_options = v8::Local<v8::PrimitiveArray>();
  v8::ScriptOrigin origin(isolate_, resource_name, line_offset, column_offset,
                          is_shared_cross_origin, script_id, source_map_url,
                          is_opaque, is_warm, is_module, host_defined_options);

  v8::ScriptCompiler::Source src(source, origin);

  v8::TryCatch try_catch(isolate_);
  v8::Local<v8::Module> module;

  if (v8::ScriptCompiler::CompileModule(isolate_, &src).ToLocal(&module)) {
    return module;
  }

  ASSERT(try_catch.HasCaught());
  std::stringstream ss;
  ss << ToString(try_catch.Exception()) << "\n";
  AppendModulePath(&ss, base_path_, paths);
  try_catch.Reset();
  ThrowError(ss.str());
  try_catch.ReThrow();
  return {};
}

// static
void Js::OnMainModuleResolve(const v8::FunctionCallbackInfo<v8::Value>& info) {
  Js::Get(info.GetIsolate())->delegate_->OnMainModuleLoaded();
}

// static
void Js::OnMainModuleFailure(const v8::FunctionCallbackInfo<v8::Value>& info) {
  ASSERT(info.Length() >= 1);
  Js* js = Js::Get(info.GetIsolate());
  v8::Local<v8::Value> error = info[0];
  v8::Local<v8::Message> message = MakeErrorMessage(js->isolate_, error);
  js->ReportException(message);
  js->delegate_->OnMainModuleLoaded();
}

// static
void Js::OnDynamicModuleResolve(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  Js* js = Js::Get(info.GetIsolate());
  v8::Local<v8::Array> data = info.Data().As<v8::Array>();
  v8::Local<v8::Promise::Resolver> resolver =
      data->Get(js->context(), 0).ToLocalChecked().As<v8::Promise::Resolver>();
  v8::Local<v8::Value> ns =
      data->Get(js->context(), 1).ToLocalChecked().As<v8::Value>();
  ASSERT(resolver->Resolve(js->context(), ns).FromMaybe(false));
}

// static
void Js::OnDynamicModuleFailure(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  Js* js = Js::Get(info.GetIsolate());
  v8::Local<v8::Promise::Resolver> resolver =
      info.Data().As<v8::Promise::Resolver>();
  ASSERT(resolver->Reject(js->context(), info[0]).FromMaybe(false));
}

// static
v8::MaybeLocal<v8::Promise> Js::ImportDynamic(
    v8::Local<v8::Context> context, v8::Local<v8::Data> host_defined_options,
    v8::Local<v8::Value> resource_name, v8::Local<v8::String> specifier,
    v8::Local<v8::FixedArray> import_assertions) {
  Js* js = Js::Get(context->GetIsolate());
  return js->ImportDynamic(resource_name, specifier);
}

v8::MaybeLocal<v8::Promise> Js::ImportDynamic(
    v8::Local<v8::Value> resource_name, v8::Local<v8::String> specifier) {
  v8::MaybeLocal<v8::Promise::Resolver> maybe_resolver =
      v8::Promise::Resolver::New(context());
  v8::Local<v8::Promise::Resolver> resolver;
  if (!maybe_resolver.ToLocal(&resolver)) {
    return {};
  }

  ASSERT(resource_name->IsString());
  std::string ref = ToString(resource_name);
  std::string spec = ToString(specifier);

  if (!IsValidImport(spec)) {
    ASSERT(resolver->Reject(context(), MakeString(MakeInvalidImportError(spec)))
               .FromMaybe(false));
    return resolver->GetPromise();
  }

  std::filesystem::path dir;

  if (ref == "<console>") {
    dir = base_path_;
  } else {
    ASSERT(modules_.find(ref) != modules_.end());
    dir = ref;
    dir.remove_filename();
  }

  std::filesystem::path path = (dir / spec).lexically_normal();
  std::string path_str = path.string();

  auto it = dynamic_imports_.find(path_str);
  if (it != dynamic_imports_.end()) {
    // Already importing; return the existing promise.
    return it->second.Get(isolate_)->GetPromise();
  }

  dynamic_imports_[path_str].Reset(isolate_, resolver);

  task_queue_->Post([=]() {
    ImportDynamic(path_str);
  });

  return resolver->GetPromise();
}

void Js::ImportDynamic(std::string path_str) {
  v8::Locker locker(isolate_);
  JsScope scope(this);
  v8::TryCatch try_catch(isolate_);

  auto it = dynamic_imports_.find(path_str);
  ASSERT(it != dynamic_imports_.end());

  v8::Local<v8::Promise::Resolver> resolver = it->second.Get(isolate_);
  dynamic_imports_.erase(it);

  std::filesystem::path path{path_str};

  if (LoadModuleByPath(path, resolver)) {
    // Everything has been handled inside LoadModuleByPath.
    try_catch.Reset();
  } else {
    // LoadModuleByPath failed to load the module.
    v8::Local<v8::Value> exception;
    if (try_catch.HasCaught()) {
      exception = try_catch.Exception();
      // Reset() is important to clear the pending exception state;
      // otherwise, v8 might crash. Repro: await on a dynamic import()
      // that has a syntax error in the imported module.
      try_catch.Reset();
    } else {
      exception = v8::Exception::Error(
          v8::String::NewFromUtf8Literal(isolate_, "Failed to import."));
    }
    ASSERT(!exception.IsEmpty());
    ASSERT(resolver->Reject(scope.context, exception).FromMaybe(false));
  }
}

void Js::RemovePendingFailedPromise(v8::Local<v8::Promise> promise) {
  auto it = failed_promises_.begin();
  while (it != failed_promises_.end()) {
    v8::Local<v8::Promise> failed_promise = it->first.Get(isolate_);
    if (failed_promise == promise) {
      it = failed_promises_.erase(it);
    } else {
      ++it;
    }
  }
}

// static
void Js::HandlePromiseRejectCallback(v8::PromiseRejectMessage data) {
  if (data.GetEvent() == v8::kPromiseRejectAfterResolved ||
      data.GetEvent() == v8::kPromiseResolveAfterResolved) {
    return;
  }

  v8::Local<v8::Promise> promise = data.GetPromise();
  v8::Isolate* isolate = promise->GetIsolate();
  Js* js = Js::Get(isolate);

  if (data.GetEvent() == v8::kPromiseRejectWithNoHandler) {
    // An promise has been rejected and there is no failure handler yet.
    // A handler might be added asynchronously later, so keep this failure
    // around until Javascript has returned to native;
    // see HandleUncaughtExceptionsInPromises below.
    v8::Local<v8::Value> exception = data.GetValue();
    v8::Local<v8::Message> message = MakeErrorMessage(isolate, exception);
    js->failed_promises_.emplace_back(
        v8::Global<v8::Promise>(isolate, promise),
        v8::Global<v8::Message>(isolate, message));
  } else if (data.GetEvent() == v8::kPromiseHandlerAddedAfterReject) {
    // A handler has been added after a promise has failed;
    // ignore its exception and don't report it.
    js->RemovePendingFailedPromise(promise);
  }
}

void Js::HandleUncaughtExceptionsInPromises() {
  if (failed_promises_.empty()) {
    return;
  }

  std::vector<std::pair<v8::Global<v8::Promise>, v8::Global<v8::Message>>> list;
  failed_promises_.swap(list);

  for (auto& p : list) {
    ReportException(p.second.Get(isolate_));
  }
}
