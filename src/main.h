#ifndef WINDOWJS_MAIN_H
#define WINDOWJS_MAIN_H

#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "console.h"
#include "js.h"
#include "js_api.h"
#include "js_events.h"
#include "js_scope.h"
#include "signal.h"
#include "subprocess.h"
#include "task_queue.h"
#include "window.h"

class Main final : public Js::Delegate, public Window::Delegate, LogHandler {
 public:
  Main();
  ~Main() override;

  void RunUntilClosed();

  void OnLog(std::string message, ConsoleLogLevel level) override;
  void OnClearLogs() override;

  void OnMainModuleLoaded() override;
  void OnJavascriptException(std::string message,
                             std::vector<std::string> stack_trace) override;

  void OnKey(int key, int scancode, int action, int mods) override;
  void OnCharacter(unsigned int codepoint) override;
  void OnMouseMove(double x, double y) override;
  void OnMouseButton(int button, bool pressed, double x, double y) override;
  void OnMouseWheel(double x, double y) override;
  void OnMouseEnter(bool entered) override;
  void OnFocus(bool focused) override;
  void OnResize(int width, int height) override;
  void OnClose() override;
  void OnDrop(std::vector<std::string> paths) override;
  void OnMaximize() override;
  void OnMinimize() override;
  void OnRestore() override;
  void OnTitleChanged() override;

 private:
  struct PendingEvent {
    JsEventType type;
    std::function<v8::Local<v8::Value>(const JsScope&)> f;
  };

  void Reload();
  void AttachToParentProcess();
  double GetTimeoutUntilNextFrame() const;
  void UpdateStats();
  void GcThread();
  void ShowConsole();
  void SaveScreenshot();
  void HandleMessageFromConsoleProcess(std::string message);
  void HandleConsoleProcessExit(std::string error);
  void PostMessageToConsole(std::string json);

  // This order is important. Background tasks may reference the TaskQueue
  // and post tasks to the foreground, so task_queue_ must be valid as long as
  // background_queue_ is still valid too. See PostToBackgroundAndResolve.
  TaskQueue task_queue_;
  ThreadPoolTaskQueue background_queue_;

  std::vector<PendingEvent> pending_events_;
  JsEvents events_;
  Window window_;
  std::unique_ptr<Js> js_;
  std::unique_ptr<JsApi> api_;

  std::thread gc_thread_;
  Signal gc_signal_;
  bool gc_quit_;

  bool main_module_loaded_;
  bool reload_requested_;
  bool first_load_;

  std::unique_ptr<Pipe> console_;
  std::deque<std::string> messages_to_console_;
};

#endif  // WINDOWJS_MAIN_H
