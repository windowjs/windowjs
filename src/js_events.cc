#include "js_events.h"

#include <GLFW/glfw3.h>

#include "fail.h"

JsEventType GetEventType(const std::string& type) {
  static const std::unordered_map<std::string, JsEventType> types{
      {"keydown", JsEventType::KEYDOWN},
      {"keyup", JsEventType::KEYUP},
      {"keypress", JsEventType::KEYPRESS},
      {"click", JsEventType::CLICK},
      {"mousedown", JsEventType::MOUSEDOWN},
      {"mouseup", JsEventType::MOUSEUP},
      {"mousemove", JsEventType::MOUSEMOVE},
      {"mouseenter", JsEventType::MOUSEENTER},
      {"mouseleave", JsEventType::MOUSELEAVE},
      {"drop", JsEventType::DROP},
      {"wheel", JsEventType::WHEEL},
      {"focus", JsEventType::FOCUS},
      {"blur", JsEventType::BLUR},
      {"resize", JsEventType::RESIZE},
      {"maximize", JsEventType::MAXIMIZE},
      {"minimize", JsEventType::MINIMIZE},
      {"restore", JsEventType::RESTORE},
      {"close", JsEventType::CLOSE},
      {"message", JsEventType::MESSAGE},
      {"log", JsEventType::CHILD_LOG},
      {"exception", JsEventType::CHILD_EXCEPTION},
      {"exit", JsEventType::CHILD_EXIT},
  };
  auto it = types.find(type);
  return it == types.end() ? JsEventType::NO_EVENT : it->second;
}

JsEvents::JsEvents() {}

JsEvents::~JsEvents() {}

void JsEvents::AddEventListener(std::string type, v8::Local<v8::Function> f,
                                v8::Isolate* isolate) {
  JsEventType t = GetEventType(type);
  if (t != JsEventType::NO_EVENT) {
    Listeners& list = listeners_[t];
    auto it = std::find(list.begin(), list.end(), f);
    if (it == list.end()) {
      list.emplace_back();
      list.back().Reset(isolate, f);
    }
  }
}

void JsEvents::RemoveEventListener(std::string type,
                                   v8::Local<v8::Function> f) {
  JsEventType t = GetEventType(type);
  if (t != JsEventType::NO_EVENT) {
    Listeners& list = listeners_[t];
    list.erase(std::remove(list.begin(), list.end(), f), list.end());
  }
}

void JsEvents::RemoveAll() {
  for (auto& [type, listeners] : listeners_) {
    listeners.clear();
  }
}

bool JsEvents::HasListeners(JsEventType type) const {
  ASSERT(type != JsEventType::NO_EVENT);
  auto it = listeners_.find(type);
  return it != listeners_.end() && !it->second.empty();
}

bool JsEvents::Dispatch(JsEventType type, v8::Local<v8::Value> event,
                        const JsScope& scope) {
  ASSERT(type != JsEventType::NO_EVENT);
  v8::Local<v8::Value> argv[] = {event};
  const Listeners& listeners = listeners_[type];
  bool any_handled = false;
  for (const auto& listener : listeners) {
    v8::Local<v8::Function> f = listener.Get(scope.isolate);
    v8::MaybeLocal<v8::Value> result =
        f->Call(scope.context, scope.context->Global(), 1, argv);
    v8::Local<v8::Value> value;
    if (result.ToLocal(&value) && value->BooleanValue(scope.isolate)) {
      any_handled = true;
    }
  }
  return any_handled;
}

namespace {

int GetLocation(int key) {
  switch (key) {
    case GLFW_KEY_LEFT_ALT:
    case GLFW_KEY_LEFT_CONTROL:
    case GLFW_KEY_LEFT_SHIFT:
    case GLFW_KEY_LEFT_SUPER:  //
      return 1;                // DOM_KEY_LOCATION_LEFT

    case GLFW_KEY_RIGHT_ALT:
    case GLFW_KEY_RIGHT_CONTROL:
    case GLFW_KEY_RIGHT_SHIFT:
    case GLFW_KEY_RIGHT_SUPER:  //
      return 2;                 // DOM_KEY_LOCATION_RIGHT
  }

  return 0;  // DOM_KEY_LOCATION_STANDARD
}

StringId GetKey(int key) {
  switch (key) {
    case GLFW_KEY_SPACE: return StringId::_SPACE_;
    case GLFW_KEY_APOSTROPHE: return StringId::_QUOTE_;
    case GLFW_KEY_COMMA: return StringId::_COMMA_;
    case GLFW_KEY_MINUS: return StringId::_MINUS_;
    case GLFW_KEY_PERIOD: return StringId::_DOT_;
    case GLFW_KEY_SLASH: return StringId::_FORWARD_SLASH_;
    case GLFW_KEY_0: return StringId::_0_;
    case GLFW_KEY_1: return StringId::_1_;
    case GLFW_KEY_2: return StringId::_2_;
    case GLFW_KEY_3: return StringId::_3_;
    case GLFW_KEY_4: return StringId::_4_;
    case GLFW_KEY_5: return StringId::_5_;
    case GLFW_KEY_6: return StringId::_6_;
    case GLFW_KEY_7: return StringId::_7_;
    case GLFW_KEY_8: return StringId::_8_;
    case GLFW_KEY_9: return StringId::_9_;
    case GLFW_KEY_SEMICOLON: return StringId::_SEMICOLON_;
    case GLFW_KEY_EQUAL: return StringId::_EQUAL_;
    case GLFW_KEY_A: return StringId::a;
    case GLFW_KEY_B: return StringId::b;
    case GLFW_KEY_C: return StringId::c;
    case GLFW_KEY_D: return StringId::d;
    case GLFW_KEY_E: return StringId::e;
    case GLFW_KEY_F: return StringId::f;
    case GLFW_KEY_G: return StringId::g;
    case GLFW_KEY_H: return StringId::h;
    case GLFW_KEY_I: return StringId::i;
    case GLFW_KEY_J: return StringId::j;
    case GLFW_KEY_K: return StringId::k;
    case GLFW_KEY_L: return StringId::l;
    case GLFW_KEY_M: return StringId::m;
    case GLFW_KEY_N: return StringId::n;
    case GLFW_KEY_O: return StringId::o;
    case GLFW_KEY_P: return StringId::p;
    case GLFW_KEY_Q: return StringId::q;
    case GLFW_KEY_R: return StringId::r;
    case GLFW_KEY_S: return StringId::s;
    case GLFW_KEY_T: return StringId::t;
    case GLFW_KEY_U: return StringId::u;
    case GLFW_KEY_V: return StringId::v;
    case GLFW_KEY_W: return StringId::w;
    case GLFW_KEY_X: return StringId::x;
    case GLFW_KEY_Y: return StringId::y;
    case GLFW_KEY_Z: return StringId::z;
    case GLFW_KEY_LEFT_BRACKET: return StringId::_LEFT_BRACKET_;
    case GLFW_KEY_BACKSLASH: return StringId::_BACK_SLASH_;
    case GLFW_KEY_RIGHT_BRACKET: return StringId::_RIGHT_BRACKET_;
    case GLFW_KEY_GRAVE_ACCENT: return StringId::_BACK_QUOTE_;
    case GLFW_KEY_ESCAPE: return StringId::Escape;
    case GLFW_KEY_ENTER: return StringId::Enter;
    case GLFW_KEY_TAB: return StringId::Tab;
    case GLFW_KEY_BACKSPACE: return StringId::Backspace;
    case GLFW_KEY_INSERT: return StringId::Insert;
    case GLFW_KEY_DELETE: return StringId::Delete;
    case GLFW_KEY_RIGHT: return StringId::ArrowRight;
    case GLFW_KEY_LEFT: return StringId::ArrowLeft;
    case GLFW_KEY_DOWN: return StringId::ArrowDown;
    case GLFW_KEY_UP: return StringId::ArrowUp;
    case GLFW_KEY_PAGE_UP: return StringId::PageUp;
    case GLFW_KEY_PAGE_DOWN: return StringId::PageDown;
    case GLFW_KEY_HOME: return StringId::Home;
    case GLFW_KEY_END: return StringId::End;
    case GLFW_KEY_CAPS_LOCK: return StringId::CapsLock;
    case GLFW_KEY_SCROLL_LOCK: return StringId::ScrollLock;
    case GLFW_KEY_NUM_LOCK: return StringId::NumLock;
    case GLFW_KEY_PRINT_SCREEN: return StringId::PrintScreen;
    case GLFW_KEY_PAUSE: return StringId::Pause;
    case GLFW_KEY_F1: return StringId::F1;
    case GLFW_KEY_F2: return StringId::F2;
    case GLFW_KEY_F3: return StringId::F3;
    case GLFW_KEY_F4: return StringId::F4;
    case GLFW_KEY_F5: return StringId::F5;
    case GLFW_KEY_F6: return StringId::F6;
    case GLFW_KEY_F7: return StringId::F7;
    case GLFW_KEY_F8: return StringId::F8;
    case GLFW_KEY_F9: return StringId::F9;
    case GLFW_KEY_F10: return StringId::F10;
    case GLFW_KEY_F11: return StringId::F11;
    case GLFW_KEY_F12: return StringId::F12;
    case GLFW_KEY_F13: return StringId::F13;
    case GLFW_KEY_F14: return StringId::F14;
    case GLFW_KEY_F15: return StringId::F15;
    case GLFW_KEY_F16: return StringId::F16;
    case GLFW_KEY_F17: return StringId::F17;
    case GLFW_KEY_F18: return StringId::F18;
    case GLFW_KEY_F19: return StringId::F19;
    case GLFW_KEY_F20: return StringId::F20;
    case GLFW_KEY_F21: return StringId::F21;
    case GLFW_KEY_F22: return StringId::F22;
    case GLFW_KEY_F23: return StringId::F23;
    case GLFW_KEY_F24: return StringId::F24;
    case GLFW_KEY_F25: return StringId::F25;
    case GLFW_KEY_KP_0: return StringId::_0_;
    case GLFW_KEY_KP_1: return StringId::_1_;
    case GLFW_KEY_KP_2: return StringId::_2_;
    case GLFW_KEY_KP_3: return StringId::_3_;
    case GLFW_KEY_KP_4: return StringId::_4_;
    case GLFW_KEY_KP_5: return StringId::_5_;
    case GLFW_KEY_KP_6: return StringId::_6_;
    case GLFW_KEY_KP_7: return StringId::_7_;
    case GLFW_KEY_KP_8: return StringId::_8_;
    case GLFW_KEY_KP_9: return StringId::_9_;
    case GLFW_KEY_KP_DECIMAL: return StringId::_DOT_;
    case GLFW_KEY_KP_DIVIDE: return StringId::_FORWARD_SLASH_;
    case GLFW_KEY_KP_MULTIPLY: return StringId::_STAR_;
    case GLFW_KEY_KP_SUBTRACT: return StringId::_MINUS_;
    case GLFW_KEY_KP_ADD: return StringId::_PLUS_;
    case GLFW_KEY_KP_ENTER: return StringId::Enter;
    case GLFW_KEY_LEFT_SHIFT: return StringId::Shift;
    case GLFW_KEY_LEFT_CONTROL: return StringId::Control;
    case GLFW_KEY_LEFT_ALT: return StringId::Alt;
    case GLFW_KEY_LEFT_SUPER: return StringId::Meta;
    case GLFW_KEY_RIGHT_SHIFT: return StringId::Shift;
    case GLFW_KEY_RIGHT_CONTROL: return StringId::Control;
    case GLFW_KEY_RIGHT_ALT: return StringId::Alt;
    case GLFW_KEY_RIGHT_SUPER: return StringId::Meta;
    case GLFW_KEY_MENU: return StringId::ContextMenu;

    case GLFW_KEY_WORLD_1: break;
    case GLFW_KEY_WORLD_2: break;
    case GLFW_KEY_KP_EQUAL: break;
  }

  return StringId::Unidentified;
}

StringId GetCode(int key) {
  switch (key) {
    case GLFW_KEY_SPACE: return StringId::Space;
    case GLFW_KEY_APOSTROPHE: return StringId::Quote;
    case GLFW_KEY_COMMA: return StringId::Comma;
    case GLFW_KEY_MINUS: return StringId::Minus;
    case GLFW_KEY_PERIOD: return StringId::Period;
    case GLFW_KEY_SLASH: return StringId::Slash;
    case GLFW_KEY_0: return StringId::Digit0;
    case GLFW_KEY_1: return StringId::Digit1;
    case GLFW_KEY_2: return StringId::Digit2;
    case GLFW_KEY_3: return StringId::Digit3;
    case GLFW_KEY_4: return StringId::Digit4;
    case GLFW_KEY_5: return StringId::Digit5;
    case GLFW_KEY_6: return StringId::Digit6;
    case GLFW_KEY_7: return StringId::Digit7;
    case GLFW_KEY_8: return StringId::Digit8;
    case GLFW_KEY_9: return StringId::Digit9;
    case GLFW_KEY_SEMICOLON: return StringId::Semicolon;
    case GLFW_KEY_EQUAL: return StringId::Equal;
    case GLFW_KEY_A: return StringId::KeyA;
    case GLFW_KEY_B: return StringId::KeyB;
    case GLFW_KEY_C: return StringId::KeyC;
    case GLFW_KEY_D: return StringId::KeyD;
    case GLFW_KEY_E: return StringId::KeyE;
    case GLFW_KEY_F: return StringId::KeyF;
    case GLFW_KEY_G: return StringId::KeyG;
    case GLFW_KEY_H: return StringId::KeyH;
    case GLFW_KEY_I: return StringId::KeyI;
    case GLFW_KEY_J: return StringId::KeyJ;
    case GLFW_KEY_K: return StringId::KeyK;
    case GLFW_KEY_L: return StringId::KeyL;
    case GLFW_KEY_M: return StringId::KeyM;
    case GLFW_KEY_N: return StringId::KeyN;
    case GLFW_KEY_O: return StringId::KeyO;
    case GLFW_KEY_P: return StringId::KeyP;
    case GLFW_KEY_Q: return StringId::KeyQ;
    case GLFW_KEY_R: return StringId::KeyR;
    case GLFW_KEY_S: return StringId::KeyS;
    case GLFW_KEY_T: return StringId::KeyT;
    case GLFW_KEY_U: return StringId::KeyU;
    case GLFW_KEY_V: return StringId::KeyV;
    case GLFW_KEY_W: return StringId::KeyW;
    case GLFW_KEY_X: return StringId::KeyX;
    case GLFW_KEY_Y: return StringId::KeyY;
    case GLFW_KEY_Z: return StringId::KeyZ;
    case GLFW_KEY_LEFT_BRACKET: return StringId::BracketLeft;
    case GLFW_KEY_BACKSLASH: return StringId::Backslash;
    case GLFW_KEY_RIGHT_BRACKET: return StringId::BracketRight;
    case GLFW_KEY_GRAVE_ACCENT: return StringId::Backquote;
    case GLFW_KEY_ESCAPE: return StringId::Escape;
    case GLFW_KEY_ENTER: return StringId::Enter;
    case GLFW_KEY_TAB: return StringId::Tab;
    case GLFW_KEY_BACKSPACE: return StringId::Backspace;
    case GLFW_KEY_INSERT: return StringId::Insert;
    case GLFW_KEY_DELETE: return StringId::Delete;
    case GLFW_KEY_RIGHT: return StringId::ArrowRight;
    case GLFW_KEY_LEFT: return StringId::ArrowLeft;
    case GLFW_KEY_DOWN: return StringId::ArrowDown;
    case GLFW_KEY_UP: return StringId::ArrowUp;
    case GLFW_KEY_PAGE_UP: return StringId::PageUp;
    case GLFW_KEY_PAGE_DOWN: return StringId::PageDown;
    case GLFW_KEY_HOME: return StringId::Home;
    case GLFW_KEY_END: return StringId::End;
    case GLFW_KEY_CAPS_LOCK: return StringId::CapsLock;
    case GLFW_KEY_SCROLL_LOCK: return StringId::ScrollLock;
    case GLFW_KEY_NUM_LOCK: return StringId::NumLock;
    case GLFW_KEY_PRINT_SCREEN: return StringId::PrintScreen;
    case GLFW_KEY_PAUSE: return StringId::Pause;
    case GLFW_KEY_F1: return StringId::F1;
    case GLFW_KEY_F2: return StringId::F2;
    case GLFW_KEY_F3: return StringId::F3;
    case GLFW_KEY_F4: return StringId::F4;
    case GLFW_KEY_F5: return StringId::F5;
    case GLFW_KEY_F6: return StringId::F6;
    case GLFW_KEY_F7: return StringId::F7;
    case GLFW_KEY_F8: return StringId::F8;
    case GLFW_KEY_F9: return StringId::F9;
    case GLFW_KEY_F10: return StringId::F10;
    case GLFW_KEY_F11: return StringId::F11;
    case GLFW_KEY_F12: return StringId::F12;
    case GLFW_KEY_F13: return StringId::F13;
    case GLFW_KEY_F14: return StringId::F14;
    case GLFW_KEY_F15: return StringId::F15;
    case GLFW_KEY_F16: return StringId::F16;
    case GLFW_KEY_F17: return StringId::F17;
    case GLFW_KEY_F18: return StringId::F18;
    case GLFW_KEY_F19: return StringId::F19;
    case GLFW_KEY_F20: return StringId::F20;
    case GLFW_KEY_F21: return StringId::F21;
    case GLFW_KEY_F22: return StringId::F22;
    case GLFW_KEY_F23: return StringId::F23;
    case GLFW_KEY_F24: return StringId::F24;
    case GLFW_KEY_F25: return StringId::F25;
    case GLFW_KEY_KP_0: return StringId::Numpad0;
    case GLFW_KEY_KP_1: return StringId::Numpad1;
    case GLFW_KEY_KP_2: return StringId::Numpad2;
    case GLFW_KEY_KP_3: return StringId::Numpad3;
    case GLFW_KEY_KP_4: return StringId::Numpad4;
    case GLFW_KEY_KP_5: return StringId::Numpad5;
    case GLFW_KEY_KP_6: return StringId::Numpad6;
    case GLFW_KEY_KP_7: return StringId::Numpad7;
    case GLFW_KEY_KP_8: return StringId::Numpad8;
    case GLFW_KEY_KP_9: return StringId::Numpad9;
    case GLFW_KEY_KP_DECIMAL: return StringId::NumpadDecimal;
    case GLFW_KEY_KP_DIVIDE: return StringId::NumpadDivide;
    case GLFW_KEY_KP_MULTIPLY: return StringId::NumpadMultiply;
    case GLFW_KEY_KP_SUBTRACT: return StringId::NumpadSubtract;
    case GLFW_KEY_KP_ADD: return StringId::NumpadAdd;
    case GLFW_KEY_KP_ENTER: return StringId::NumpadEnter;
    case GLFW_KEY_LEFT_SHIFT: return StringId::ShiftLeft;
    case GLFW_KEY_LEFT_CONTROL: return StringId::ControlLeft;
    case GLFW_KEY_LEFT_ALT: return StringId::AltLeft;
    case GLFW_KEY_LEFT_SUPER: return StringId::MetaLeft;
    case GLFW_KEY_RIGHT_SHIFT: return StringId::ShiftRight;
    case GLFW_KEY_RIGHT_CONTROL: return StringId::ControlRight;
    case GLFW_KEY_RIGHT_ALT: return StringId::AltRight;
    case GLFW_KEY_RIGHT_SUPER: return StringId::MetaRight;
    case GLFW_KEY_MENU: return StringId::ContextMenu;

    case GLFW_KEY_WORLD_1: break;
    case GLFW_KEY_WORLD_2: break;
    case GLFW_KEY_KP_EQUAL: break;
  }

  return StringId::Unidentified;
}

}  // namespace

v8::Local<v8::Object> MakeEvent(StringId type, const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);
  scope.Set(event, StringId::type, type);
  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);
  return event;
}

v8::Local<v8::Value> MakeKeyEvent(int key, int scancode, int action, int mods,
                                  const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);

  StringId type = StringId::keydown;
  bool repeat = false;

  if (action == GLFW_PRESS) {
    type = StringId::keydown;
    repeat = false;
  } else if (action == GLFW_REPEAT) {
    type = StringId::keydown;
    repeat = true;
  } else if (action == GLFW_RELEASE) {
    type = StringId::keyup;
    repeat = false;
  } else {
    ASSERT(false);
  }

  scope.Set(event, StringId::type, type);
  scope.Set(event, StringId::repeat, repeat);
  scope.Set(event, StringId::altKey, (mods & GLFW_MOD_ALT) != 0);
  scope.Set(event, StringId::ctrlKey, (mods & GLFW_MOD_CONTROL) != 0);
  scope.Set(event, StringId::metaKey, (mods & GLFW_MOD_SUPER) != 0);
  scope.Set(event, StringId::shiftKey, (mods & GLFW_MOD_SHIFT) != 0);
  scope.Set(event, StringId::location, GetLocation(key));
  scope.Set(event, StringId::key, GetKey(key));
  scope.Set(event, StringId::code, GetCode(key));

  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);

  return event;
}

v8::Local<v8::Object> MakeKeyPressEvent(unsigned int codepoint,
                                        const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);

  scope.Set(event, StringId::type, StringId::keypress);
  scope.Set(event, StringId::code, (int) codepoint);

  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);

  return event;
}

v8::Local<v8::Object> MakeMouseButtonEvent(JsEventType type, int button,
                                           double x, double y,
                                           const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);

  StringId type_id = StringId::LAST_STRING_ID;
  if (type == JsEventType::MOUSEDOWN) {
    type_id = StringId::mousedown;
  } else if (type == JsEventType::MOUSEUP) {
    type_id = StringId::mouseup;
  } else if (type == JsEventType::CLICK) {
    type_id = StringId::click;
  } else {
    ASSERT(false);
  }

  scope.Set(event, StringId::type, type_id);
  scope.Set(event, StringId::x, x);
  scope.Set(event, StringId::y, y);
  scope.Set(event, StringId::clientX, x);
  scope.Set(event, StringId::clientY, y);

  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    button = 0;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    button = 2;
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    button = 1;
  }
  scope.Set(event, StringId::button, button);

  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);

  return event;
}

v8::Local<v8::Object> MakeMouseMoveEvent(double x, double y,
                                         const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);

  scope.Set(event, StringId::type, StringId::mousemove);
  scope.Set(event, StringId::x, x);
  scope.Set(event, StringId::y, y);
  scope.Set(event, StringId::clientX, x);
  scope.Set(event, StringId::clientY, y);
  scope.Set(event, StringId::offsetX, x);
  scope.Set(event, StringId::offsetY, y);

  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);

  return event;
}

v8::Local<v8::Object> MakeMouseWheelEvent(double x, double y,
                                          const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);

  scope.Set(event, StringId::type, StringId::wheel);
  scope.Set(event, StringId::deltaX, x);
  scope.Set(event, StringId::deltaY, y);

  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);

  return event;
}

v8::Local<v8::Object> MakeDropEvent(std::vector<std::string> paths,
                                    const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);
  scope.Set(event, StringId::type, StringId::drop);
  v8::Local<v8::Array> files = v8::Array::New(scope.isolate, paths.size());
  for (uint32_t i = 0; i < paths.size(); i++) {
    IGNORE_RESULT(files->Set(scope.context, i, scope.MakeString(paths[i])));
  }
  scope.Set(event, StringId::files, files.As<v8::Value>());
  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);
  return event;
}

v8::Local<v8::Object> MakeLogEvent(std::string log, std::string level,
                                   const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);
  scope.Set(event, StringId::type, StringId::log);
  scope.SetValue(event, StringId::log, scope.MakeString(log));
  scope.SetValue(event, StringId::level, scope.MakeString(level));
  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);
  return event;
}

v8::Local<v8::Object> MakeExceptionEvent(std::string error,
                                         const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);
  scope.Set(event, StringId::type, StringId::exception);
  scope.SetValue(event, StringId::exception, scope.MakeString(error));
  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);
  return event;
}

v8::Local<v8::Object> MakeExitEvent(std::string error, int64_t status,
                                    const JsScope& scope) {
  v8::Local<v8::Object> event = v8::Object::New(scope.isolate);
  scope.Set(event, StringId::type, StringId::exit);
  if (!error.empty()) {
    scope.SetValue(event, StringId::error, scope.MakeString(error));
  }
  scope.Set(event, StringId::status, (double) status);
  event->SetIntegrityLevel(scope.context, v8::IntegrityLevel::kFrozen);
  return event;
}
