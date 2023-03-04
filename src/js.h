#ifndef WINDOWJS_JS_H
#define WINDOWJS_JS_H

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <v8/include/v8.h>

#include "console.h"
#include "fail.h"
#include "js_strings.h"
#include "task_queue.h"

// Wrapper around a v8 Isolate and Context.
//
// This class manages module loading.
//
// v8 is multi-threaded, and multiple threads may enter the Javascript context.
// Only one thread will be executing it at a time though.
//
// Sometimes we enter Javascript from the main thread, but sometimes it also
// enters from a background thread (e.g. to run background tasks while main
// is swapping buffers and waiting for vsync, or to run async tasks that have
// resolved).
class Js final {
 public:
  static void Init(const char* program);
  static void Shutdown();
  static double MonotonicallyIncreasingTime();

  class Delegate {
   public:
    virtual ~Delegate() = default;

    virtual void OnMainModuleLoaded() {}

    virtual void OnJavascriptException(std::string message,
                                       std::vector<std::string> stack_trace) {}
  };

  // All of these dependencies must outlive the Js object.
  // If the object is deleted, then TaskQueue must *not* run any pending tasks
  // anymore.
  Js(Delegate* delegate, std::filesystem::path base_path,
     TaskQueue* task_queue);
  ~Js();

  static Js* Get(v8::Isolate* isolate) {
    return static_cast<Js*>(isolate->GetData(0));
  }

  static Js* GetCurrent() { return Get(v8::Isolate::GetCurrent()); }

  v8::Isolate* isolate() { return isolate_; }
  v8::Local<v8::Context> context() { return context_.Get(isolate_); }
  v8::Local<v8::Object> global() { return context()->Global(); }
  JsStrings* strings() { return strings_.get(); }

  v8::Local<v8::String> MakeString(std::string_view s);
  v8::Local<v8::String> GetConstantString(StringId id) const {
    return strings_->GetConstantString(id, isolate_);
  }
  std::string ToString(v8::Local<v8::Value> value);
  std::string ToStringOr(v8::Local<v8::Value> value,
                         std::string_view or_string);

  bool GetBooleanOr(v8::Local<v8::Object> object, std::string_view key,
                    bool fallback);

  void ThrowError(std::string_view error);
  void ThrowTypeError(std::string_view error);
  void ThrowIllegalConstructor();
  void ThrowIllegalInvocation();
  void ThrowInvalidArgument();
  void ReportException(v8::Local<v8::Message> message);
  void HandleUncaughtExceptionsInPromises();

  void LoadMainModule(std::string_view name);

  std::unique_ptr<std::string> ExecuteScript(std::string_view source);
  void SuppressNextScriptResult();

 private:
  bool LoadModuleByPath(std::filesystem::path path,
                        v8::Local<v8::Promise::Resolver> resolver);

  v8::Local<v8::Module> LoadModuleTree(
      v8::Local<v8::Context> context, const std::filesystem::path& path,
      std::vector<std::filesystem::path>* paths);

  static v8::MaybeLocal<v8::Module> ResolveModule(
      v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
      v8::Local<v8::FixedArray> import_assertions,
      v8::Local<v8::Module> referrer);

  v8::Local<v8::String> LoadModuleSource(
      const std::filesystem::path& path,
      const std::vector<std::filesystem::path>& paths);

  v8::Local<v8::Module> CompileModule(
      v8::Local<v8::String> source, const std::filesystem::path& path,
      const std::vector<std::filesystem::path>& paths);

  static void OnMainModuleResolve(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void OnMainModuleFailure(
      const v8::FunctionCallbackInfo<v8::Value>& info);

  static void OnDynamicModuleResolve(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void OnDynamicModuleFailure(
      const v8::FunctionCallbackInfo<v8::Value>& info);

  static v8::MaybeLocal<v8::Promise> ImportDynamic(
      v8::Local<v8::Context> context,
      v8::Local<v8::Data> host_defined_options,
      v8::Local<v8::Value> resource_name,
      v8::Local<v8::String> specifier,
      v8::Local<v8::FixedArray> import_assertions);

  v8::MaybeLocal<v8::Promise> ImportDynamic(v8::Local<v8::Value> resource_name,
                                            v8::Local<v8::String> specifier);

  void ImportDynamic(std::string path_str);

  static void HandlePromiseRejectCallback(v8::PromiseRejectMessage message);
  void RemovePendingFailedPromise(v8::Local<v8::Promise> promise);

  Delegate* delegate_;
  std::filesystem::path base_path_;
  TaskQueue* task_queue_;

  v8::ArrayBuffer::Allocator* allocator_;
  std::unique_ptr<v8::debug::ConsoleDelegate> console_delegate_;

  v8::Isolate* isolate_;
  v8::Global<v8::Context> context_;
  std::unordered_map<std::string, v8::Global<v8::Module>> modules_;
  std::unordered_map<int, std::string> module_path_by_id_;
  std::unordered_map<std::string, v8::Global<v8::Promise::Resolver>>
      dynamic_imports_;
  std::vector<std::pair<v8::Global<v8::Promise>, v8::Global<v8::Message>>>
      failed_promises_;

  std::unique_ptr<JsStrings> strings_;

  bool suppress_next_script_result_;
};

#endif  // WINDOWJS_JS_H
