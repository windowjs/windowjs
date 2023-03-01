#include "js_api_process.h"

#include <cstdlib>

#include <uv.h>

#include "args.h"
#include "fail.h"
#include "file.h"
#include "platform.h"
#include "subprocess.h"
#include "thread.h"

namespace {

void GetArgs(v8::Local<v8::String> property,
             const v8::PropertyCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());
  std::vector<v8::Local<v8::Value>> args;
  args.resize(Args().args.size());
  for (unsigned i = 0; i < Args().args.size(); i++) {
    args[i] = api->js()->MakeString(Args().args[i]);
  }
  v8::Local<v8::Array> array =
      v8::Array::New(info.GetIsolate(), args.data(), args.size());
  info.GetReturnValue().Set(array);
}

void GetCpus(v8::Local<v8::String> property,
             const v8::PropertyCallbackInfo<v8::Value>& info) {
  static int cpus = 0;
  if (cpus == 0) {
    uv_cpu_info_t* info = nullptr;
    uv_cpu_info(&info, &cpus);
    uv_free_cpu_info(info, cpus);
  }
  info.GetReturnValue().Set(cpus);
}

void Process(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall()) {
    info.GetIsolate()->ThrowError("Process is a constructor");
    return;
  }
  JsApi* api = JsApi::Get(info.GetIsolate());
  v8::Local<v8::Object> thiz = info.This();
  new ProcessApi(api, thiz);
}

}  // namespace

ProcessApi::ProcessApi(JsApi* api, v8::Local<v8::Object> thiz)
    : JsApiWrapper(api->isolate(), thiz) {}

ProcessApi::~ProcessApi() {}

bool ProcessApi::SendMessage(MessageType type, std::string message) {
  if (pipe_) {
    pipe_->SendMessage(type, std::move(message));
    return true;
  } else {
    return false;
  }
}

// static
v8::Local<v8::Function> ProcessApi::GetConstructor(JsApi* api,
                                                   const JsScope& scope) {
  v8::Local<v8::FunctionTemplate> process =
      v8::FunctionTemplate::New(scope.isolate, Process);
  process->SetClassName(scope.GetConstantString(StringId::Process));

  v8::Local<v8::ObjectTemplate> instance = process->InstanceTemplate();
  // Used in JsApiWrapper to track this.
  instance->SetInternalFieldCount(1);

  // Methods on instances of "Process". Called like so:
  //   const child = ...;
  //   child.postMessage("message to send to the child process");
  //   child.addEventListener('message', (event) => { ... message from child });
  v8::Local<v8::ObjectTemplate> prototype = process->PrototypeTemplate();
  scope.Set(prototype, StringId::postMessage, PostMessage);
  scope.Set(prototype, StringId::addEventListener, AddEventListener);
  scope.Set(prototype, StringId::removeEventListener, RemoveEventListener);
  scope.Set(prototype, StringId::close, Close);

  // Functions on the "Process" object itself. Called like so:
  //   console.log(Process.args);
  //   const child = Process.spawn('another-window.js');
  scope.Set(process, StringId::args, GetArgs);
  scope.Set(process, StringId::cpus, GetCpus);
  scope.Set(process, StringId::spawn, Spawn);
  scope.Set(process, StringId::exit, Exit);

  v8::Local<v8::Function> constructor =
      process->GetFunction(scope.context).ToLocalChecked();

  return constructor;
}

// static
ProcessApi* ProcessApi::MaybeAttachToParent(
    JsApi* api, const JsScope& scope, v8::Local<v8::Function> constructor) {
  if (!Args().is_child_process) {
    return nullptr;
  }

  v8::Local<v8::Object> object =
      constructor->NewInstance(api->isolate()->GetCurrentContext())
          .ToLocalChecked();
  ProcessApi* process = api->GetProcessApi(object);
  ASSERT(process);

  process->pipe_ = Pipe::AttachToParent(
      [api, process](uint32_t type, std::string message) {
        // Called on the Pipe's background thread.
        api->task_queue()->Post([process, type, message = std::move(message)] {
          process->HandleMessageFromParentProcess(type, std::move(message));
        });
      },
      [api, process](int64_t status, std::string error) {
        // Called on the Pipe's background thread.
        api->task_queue()->Post([process, status, error = std::move(error)] {
          process->HandleParentProcessExit(status, std::move(error));
        });
      });

  scope.Set(constructor, StringId::parent, object);

  return process;
}

// static
void ProcessApi::Spawn(const v8::FunctionCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());
  v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

  if (info.Length() < 1 || !info[0]->IsString()) {
    api->js()->ThrowError("Process.spawn requires a string argument.");
    return;
  }
  std::string initial_module = api->js()->ToString(info[0]);

  std::vector<std::string> args_to_js;
  if (info.Length() >= 2 && info[1]->IsArray()) {
    v8::Local<v8::Array> array = info[1].As<v8::Array>();
    int length = array->Length();
    args_to_js.resize(length);
    for (int i = 0; i < length; i++) {
      v8::Local<v8::Value> value = array->Get(context, i).ToLocalChecked();
      if (!value->IsString()) {
        api->js()->ThrowError(
            "Process.spawn takes a list of strings as the second argument.");
        return;
      }
      args_to_js[i] = api->js()->ToString(value.As<v8::String>());
    }
  }

  bool headless = false;
  bool log = Args().log;
  if (info.Length() >= 3 && info[2]->IsObject()) {
    v8::Local<v8::Object> options = info[2].As<v8::Object>();
    headless = api->js()->GetBooleanOr(options, "headless", headless);
    if (log) {
      log = api->js()->GetBooleanOr(options, "log", log);
    }
  }

  std::string error;
  std::string exe = GetExePath(&error);
  ASSERT(error.empty());

  std::vector<std::string> args;
  args.reserve(6 + args_to_js.size());
  args.emplace_back(Basename(exe).string());
  args.emplace_back("--child");
  if (headless) {
    args.emplace_back("--headless");
  }
  if (!log) {
    args.emplace_back("--no-log");
  }
  args.emplace_back(std::move(initial_module));
  args.emplace_back("--");
  for (std::string& arg : args_to_js) {
    args.emplace_back(std::move(arg));
  }

  v8::Local<v8::Object> object =
      api->GetProcessConstructor()->NewInstance(context).ToLocalChecked();

  ProcessApi* process = api->GetProcessApi(object);
  ASSERT(process);

  // Hold a reference to this object until the child process quits,
  // to prevent the GC from destroying it and its Pipe.
  process->SetStrong();

  process->pipe_ = Pipe::Spawn(
      std::move(exe), std::move(args), log,
      [api, process](uint32_t type, std::string message) {
        // Called on the Pipe's background thread, which is owned by
        // the ProcessApi instance and synchronized in its destructor.
        api->task_queue()->Post([process, type, message = std::move(message)] {
          process->HandleMessageFromChildProcess(type, std::move(message));
        });
      },
      [api, process](int64_t status, std::string error) {
        api->task_queue()->Post([process, status, error = std::move(error)] {
          process->HandleChildProcessExit(status, std::move(error));
        });
      });

  info.GetReturnValue().Set(object);
}

// static
void ProcessApi::PostMessage(const v8::FunctionCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  ProcessApi* process = api->GetProcessApi(info.This());
  if (!process || info.Length() < 1) {
    return;
  }
  v8::MaybeLocal<v8::String> json =
      v8::JSON::Stringify(info.GetIsolate()->GetCurrentContext(), info[0]);
  if (json.IsEmpty()) {
    // Throws on failures.
    return;
  }
  if (!process->pipe_) {
    api->js()->ThrowError("Connection closed.");
    return;
  }
  std::string s = api->js()->ToString(json.ToLocalChecked());
  process->pipe_->SendMessage(MESSAGE, std::move(s));
}

// static
void ProcessApi::Exit(const v8::FunctionCallbackInfo<v8::Value>& info) {
  int64_t code = 0;
  if (info.Length() >= 1 && info[0]->IsInt32()) {
    code = info[0].As<v8::Int32>()->Value();
  }
  if (Args().headless || code != 0) {
    // TODO: this crashes on Github when running tests.
    // The crash reproes under high contention on debug builds with MSVC.
    // It's a crash in a destructor at shutdown; using std::quick_exit to
    // bypass atexit callbacks as a temporary workaround.
    // https://github.com/windowjs/windowjs/issues/86
#if defined(WINDOWJS_WIN)
    std::quick_exit(code);
#else
    std::exit(code);
#endif
  } else {
    JsApi* api = JsApi::Get(info.GetIsolate());
    glfwSetWindowShouldClose(api->glfw_window(), GLFW_TRUE);
    glfwPostEmptyEvent();
  }
}

// static
void ProcessApi::Close(const v8::FunctionCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());
  ProcessApi* process = api->GetProcessApi(info.This());
  if (!process || !process->pipe_) {
    // Already closed.
    return;
  }
  if (process->pipe_->is_child_process()) {
    api->js()->ThrowError(
        "Child processes can't close their parent processes.");
    return;
  }
  process->pipe_.reset();
}

// static
void ProcessApi::AddEventListener(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    api->js()->ThrowError(
        "addEventListener requires an event type and a callback function");
    return;
  }

  std::string type = api->js()->ToString(info[0]);
  v8::Local<v8::Function> f = info[1].As<v8::Function>();

  ProcessApi* process = api->GetProcessApi(info.This());
  if (process) {
    process->events_.AddEventListener(type, f, info.GetIsolate());
  }
}

// static
void ProcessApi::RemoveEventListener(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    api->js()->ThrowError(
        "removeEventListener requires an event type and a callback function");
    return;
  }

  std::string type = api->js()->ToString(info[0]);
  v8::Local<v8::Function> f = info[1].As<v8::Function>();

  ProcessApi* process = api->GetProcessApi(info.This());
  if (process) {
    process->events_.RemoveEventListener(type, f);
  }
}

void ProcessApi::HandleMessageFromChildProcess(uint32_t type,
                                               std::string message) {
  ASSERT(IsMainThread());
  JsScope scope(api()->js());
  v8::TryCatch try_catch(scope.isolate);

  v8::Local<v8::Value> event;
  JsEventType event_type;

  // All of these message types have a JSON payload in the message.
  if (type == MESSAGE || type == LOG || type == EXCEPTION) {
    v8::MaybeLocal<v8::Value> json =
        v8::JSON::Parse(scope.context, scope.MakeString(message));
    // Invalid JSON should have been filtered out in the sending process.
    ASSERT(!json.IsEmpty());
    event = json.ToLocalChecked();
    if (type == MESSAGE) {
      event_type = JsEventType::MESSAGE;
    } else if (type == LOG) {
      event_type = JsEventType::CHILD_LOG;
    } else {
      event_type = JsEventType::CHILD_EXCEPTION;
    }
  } else {
    // Unknown event type.
    ASSERT(false);
  }

  if (!try_catch.HasCaught() && !event.IsEmpty()) {
    events_.Dispatch(event_type, event, scope);
  }

  if (try_catch.HasCaught()) {
    api()->js()->ReportException(try_catch.Message());
  }
}

void ProcessApi::HandleChildProcessExit(int64_t status, std::string error) {
  ASSERT(IsMainThread());
  JsScope scope(api()->js());
  v8::TryCatch try_catch(scope.isolate);
  v8::Local<v8::Object> event = MakeExitEvent(std::move(error), status, scope);
  if (!try_catch.HasCaught() && !event.IsEmpty()) {
    events_.Dispatch(JsEventType::CHILD_EXIT, event, scope);
  }
  if (try_catch.HasCaught()) {
    api()->js()->ReportException(try_catch.Message());
  }
  pipe_.reset();
  // No more events will happen to this object, so it can be GCed now.
  SetWeak();
}

void ProcessApi::HandleMessageFromParentProcess(uint32_t type,
                                                std::string message) {
  ASSERT(IsMainThread());
  ASSERT(type == MESSAGE);
  JsScope scope(api()->js());
  v8::TryCatch try_catch(scope.isolate);
  v8::MaybeLocal<v8::Value> json =
      v8::JSON::Parse(scope.context, scope.MakeString(message));
  // Invalid JSON should have been filtered out in the sending process.
  ASSERT(!json.IsEmpty());
  v8::Local<v8::Value> event = json.ToLocalChecked();
  events_.Dispatch(JsEventType::MESSAGE, event, scope);
  if (try_catch.HasCaught()) {
    api()->js()->ReportException(try_catch.Message());
  }
}

void ProcessApi::HandleParentProcessExit(int64_t status, std::string error) {
  std::exit(0);
}
