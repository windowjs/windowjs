#ifndef WINDOWJS_JS_API_PROCESS_H
#define WINDOWJS_JS_API_PROCESS_H

#include <memory>

#include <v8/include/v8.h>

#include "js_api.h"
#include "js_events.h"
#include "js_scope.h"

class Pipe;

class ProcessApi final : public JsApiWrapper {
 public:
  // Constants for the Pipe::SendMessage "type" parameter, used internally.
  enum MessageType {
    // Messages sent via process.postMessage() and received via
    // process.addEventListener('message', (message) => { ... }).
    MESSAGE,

    // Logs issued by a child process and received via
    // process.addEventListener('log', (message) => { ... }).
    LOG,

    // Uncaught exceptions in a child process and received via
    // process.addEventListener('exception', (message) => { ... }).
    EXCEPTION,
  };

  ProcessApi(JsApi* api, v8::Local<v8::Object> thiz);
  ~ProcessApi() override;

  bool SendMessage(MessageType type, std::string message);

  static v8::Local<v8::Function> GetConstructor(JsApi* api,
                                                const JsScope& scope);

  static ProcessApi* MaybeAttachToParent(JsApi* api, const JsScope& scope,
                                         v8::Local<v8::Function> constructor);

 private:
  static void Spawn(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void PostMessage(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Exit(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Close(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void AddEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RemoveEventListener(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  void HandleMessageFromChildProcess(uint32_t type, std::string message);
  void HandleChildProcessExit(int64_t status, std::string message);

  void HandleMessageFromParentProcess(uint32_t type, std::string message);
  void HandleParentProcessExit(int64_t status, std::string message);

  std::unique_ptr<Pipe> pipe_;
  JsEvents events_;
};

#endif  // WINDOWJS_JS_API_PROCESS_H
