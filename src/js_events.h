#ifndef WINDOWJS_JS_EVENTS_H
#define WINDOWJS_JS_EVENTS_H

#include <string>
#include <unordered_map>
#include <vector>

#include <v8/include/v8.h>

#include "js_scope.h"

enum class JsEventType {
  KEYDOWN,
  KEYUP,
  KEYPRESS,
  CLICK,
  MOUSEDOWN,
  MOUSEUP,
  MOUSEMOVE,
  MOUSEENTER,
  MOUSELEAVE,
  DROP,
  WHEEL,
  FOCUS,
  BLUR,
  RESIZE,
  MAXIMIZE,
  MINIMIZE,
  RESTORE,
  CLOSE,
  MESSAGE,
  CHILD_LOG,
  CHILD_EXCEPTION,
  CHILD_EXIT,
  NO_EVENT,  // Must be the last entry; this is also the number of JsEventTypes.
};

JsEventType GetEventType(const std::string& type);

class JsEvents final {
 public:
  using Listeners = std::vector<v8::Global<v8::Function>>;

  JsEvents();
  ~JsEvents();

  void AddEventListener(std::string type, v8::Local<v8::Function> f,
                        v8::Isolate* isolate);

  void RemoveEventListener(std::string type, v8::Local<v8::Function> f);

  void RemoveAll();

  bool HasListeners(JsEventType type) const;

  // Returns "true" if any listener returned true.
  bool Dispatch(JsEventType type, v8::Local<v8::Value> event,
                const JsScope& scope);

 private:
  std::unordered_map<JsEventType, Listeners> listeners_;
};

v8::Local<v8::Object> MakeEvent(StringId type, const JsScope& scope);

v8::Local<v8::Value> MakeKeyEvent(int key, int scancode, int action, int mods,
                                  const JsScope& scope);

v8::Local<v8::Object> MakeKeyPressEvent(unsigned int codepoint,
                                        const JsScope& scope);

v8::Local<v8::Object> MakeMouseButtonEvent(JsEventType type, int button,
                                           double x, double y,
                                           const JsScope& scope);

v8::Local<v8::Object> MakeMouseMoveEvent(double x, double y,
                                         const JsScope& scope);

v8::Local<v8::Object> MakeMouseWheelEvent(double x, double y,
                                          const JsScope& scope);

v8::Local<v8::Object> MakeDropEvent(std::vector<std::string> paths,
                                    const JsScope& scope);

v8::Local<v8::Object> MakeLogEvent(std::string log, std::string level,
                                   const JsScope& scope);

v8::Local<v8::Object> MakeExceptionEvent(std::string error,
                                         const JsScope& scope);

v8::Local<v8::Object> MakeExitEvent(std::string error, int64_t status,
                                    const JsScope& scope);

#endif  // WINDOWJS_JS_EVENTS_H
