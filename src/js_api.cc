#include "js_api.h"

#include <stdlib.h>

#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkTypeface.h>

#include "args.h"
#include "console.h"
#include "css.h"
#include "file.h"
#include "js_api_canvas.h"
#include "js_api_codec.h"
#include "js_api_file.h"
#include "js_api_process.h"
#include "platform.h"
#include "version.h"

#if defined(WINDOWJS_WIN)
// Must come first:
#include <windows.h>
// Must come after windows.h:
#include <shellapi.h>
#endif

namespace {

constexpr uint16_t wrapper_class_id_for_destructors = 12345;

void Now(const v8::FunctionCallbackInfo<v8::Value>& args) {
  args.GetReturnValue().Set(glfwGetTime() * 1000);
}

void JsHeapSizeLimit(v8::Local<v8::Name> property,
                     const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::HeapStatistics stats;
  info.GetIsolate()->GetHeapStatistics(&stats);
  info.GetReturnValue().Set((double) stats.heap_size_limit());
}

void TotalJsHeapSize(v8::Local<v8::Name> property,
                     const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::HeapStatistics stats;
  info.GetIsolate()->GetHeapStatistics(&stats);
  info.GetReturnValue().Set((double) stats.total_heap_size());
}

void UsedJsHeapSize(v8::Local<v8::Name> property,
                    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::HeapStatistics stats;
  info.GetIsolate()->GetHeapStatistics(&stats);
  info.GetReturnValue().Set((double) stats.used_heap_size());
}

void Close(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  // window.close() doesn't trigger the 'close' event, and can be used
  // to force a window close at a later time.
  glfwSetWindowShouldClose(api->glfw_window(), GLFW_TRUE);
  glfwPostEmptyEvent();
}

void Focus(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  api->window()->Focus();
}

void RequestAttention(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  api->window()->RequestAttention();
}

void Minimize(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  api->window()->Minimize();
}

void Maximize(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  api->window()->Maximize();
}

void Restore(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  api->window()->Restore();
}

void GetTitle(v8::Local<v8::Name> property,
              const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->js()->MakeString(api->window()->title()));
}

void SetTitle(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
              const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsString()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetTitle(api->js()->ToString(value));
  }
}

void GetWidth(v8::Local<v8::Name> property,
              const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->width());
}

void SetWidth(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
              const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsNumber()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetWidth((int) value.As<v8::Number>()->Value());
  }
}

void GetHeight(v8::Local<v8::Name> property,
               const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->height());
}

void SetHeight(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
               const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsNumber()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetHeight((int) value.As<v8::Number>()->Value());
  }
}

void GetFrameLeft(v8::Local<v8::Name> property,
                  const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->frame_left());
}

void GetFrameRight(v8::Local<v8::Name> property,
                   const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->frame_right());
}

void GetFrameTop(v8::Local<v8::Name> property,
                 const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->frame_top());
}

void GetFrameBottom(v8::Local<v8::Name> property,
                    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->frame_bottom());
}

void GetX(v8::Local<v8::Name> property,
          const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->x());
}

void GetY(v8::Local<v8::Name> property,
          const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->y());
}

void SetX(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
          const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsNumber()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetX((int) value.As<v8::Number>()->Value());
  }
}

void SetY(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
          const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsNumber()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetY((int) value.As<v8::Number>()->Value());
  }
}

void GetVisible(v8::Local<v8::Name> property,
                const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->visible());
}

void SetVisible(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetVisible(value.As<v8::Boolean>()->Value());
  }
}

void GetDecorated(v8::Local<v8::Name> property,
                  const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->decorated());
}

void SetDecorated(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                  const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetDecorated(value.As<v8::Boolean>()->Value());
  }
}

void GetResizable(v8::Local<v8::Name> property,
                  const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->resizable());
}

void SetResizable(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                  const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetResizable(value.As<v8::Boolean>()->Value());
  }
}

void GetAlwaysOnTop(v8::Local<v8::Name> property,
                    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->always_on_top());
}

void SetAlwaysOnTop(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                    const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetAlwaysOnTop(value.As<v8::Boolean>()->Value());
  }
}

void GetKeepAspectRatio(v8::Local<v8::Name> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->keep_aspect_ratio());
}

void SetKeepAspectRatio(v8::Local<v8::Name> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetKeepAspectRatio(value.As<v8::Boolean>()->Value());
  }
}

void GetFocused(v8::Local<v8::Name> property,
                const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->focused());
}

void GetMaximized(v8::Local<v8::Name> property,
                  const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->maximized());
}

void GetMinimized(v8::Local<v8::Name> property,
                  const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->minimized());
}

void GetFullscreen(v8::Local<v8::Name> property,
                   const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->fullscreen());
}

void SetFullscreen(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                   const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetFullscreen(value.As<v8::Boolean>()->Value());
  }
}

void GetVsync(v8::Local<v8::Name> property,
              const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->vsync());
}

void SetVsync(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
              const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->SetVsync(value.As<v8::Boolean>()->Value());
  }
}

void AvailWidth(v8::Local<v8::Name> property,
                const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->avail_width());
}

void AvailHeight(v8::Local<v8::Name> property,
                 const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->avail_height());
}

void ScreenWidth(v8::Local<v8::Name> property,
                 const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->screen_width());
}

void ScreenHeight(v8::Local<v8::Name> property,
                  const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->screen_height());
}

void DevicePixelRatio(v8::Local<v8::Name> property,
                      const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->device_pixel_ratio());
}

void GetFonts(v8::Local<v8::Name> property,
              const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  JsScope scope(api->js());

  sk_sp<SkFontMgr> font_manager = SkFontMgr::RefDefault();
  int count = font_manager->countFamilies();

  v8::Local<v8::Array> fonts = v8::Array::New(scope.isolate, count);

  for (int i = 0; i < count; i++) {
    SkString name;
    font_manager->getFamilyName(i, &name);
    v8::MaybeLocal<v8::String> font = v8::String::NewFromUtf8(
        scope.isolate, name.c_str(), v8::NewStringType::kNormal, name.size());
    ASSERT(fonts->Set(scope.context, i, font.ToLocalChecked()).FromJust());
  }

  info.GetReturnValue().Set(fonts);
}

void GetJs(v8::Local<v8::Name> property,
           const v8::PropertyCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());
  api->js()->SuppressNextScriptResult();
  $(DEV) << "Hello developer!";
}

void GetClipboardText(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  std::string text;
  const char* ptr = glfwGetClipboardString(api->glfw_window());
  if (ptr) {
    text = ptr;
  }
  args.GetReturnValue().Set(api->js()->MakeString(text));
}

void SetClipboardText(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() >= 1 && args[0]->IsString()) {
    JsApi* api = JsApi::Get(args.GetIsolate());
    std::string text = api->js()->ToString(args[0]);
    glfwSetClipboardString(api->glfw_window(), text.c_str());
  }
}

void GetShowOverlayConsole(v8::Local<v8::Name> property,
                           const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->console_overlay()->is_enabled());
}

void SetShowOverlayConsole(v8::Local<v8::Name> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->console_overlay()->SetEnabled(
        value.As<v8::Boolean>()->Value());
  }
}

void GetShowOverlayConsoleOnErrors(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(
      api->window()->console_overlay()->is_enable_on_errors());
}

void SetShowOverlayConsoleOnErrors(v8::Local<v8::Name> property,
                                   v8::Local<v8::Value> value,
                                   const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->console_overlay()->SetEnableOnErrors(
        value.As<v8::Boolean>()->Value());
  }
}

void GetOverlayConsoleTextColor(
    v8::Local<v8::Name> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  std::string color =
      SkColorToCSSColor(api->window()->console_overlay()->text_color());
  info.GetReturnValue().Set(api->js()->MakeString(color));
}

void SetOverlayConsoleTextColor(v8::Local<v8::Name> property,
                                v8::Local<v8::Value> value,
                                const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsString()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    std::string s = api->js()->ToString(value);
    SkColor color;
    if (CSSColorToSkColor(s, &color)) {
      api->window()->console_overlay()->SetTextColor(color);
    }
  }
}

void GetShowOverlayStats(v8::Local<v8::Name> property,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->stats()->is_enabled());
}

void SetShowOverlayStats(v8::Local<v8::Name> property,
                         v8::Local<v8::Value> value,
                         const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->stats()->SetEnabled(value.As<v8::Boolean>()->Value());
  }
}

void GetProfileFrameTimes(v8::Local<v8::Name> property,
                          const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->stats()->is_print_frame_times());
}

void SetProfileFrameTimes(v8::Local<v8::Name> property,
                          v8::Local<v8::Value> value,
                          const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  if (value->IsBoolean()) {
    JsApi* api = JsApi::Get(info.GetIsolate());
    api->window()->stats()->SetPrintFrameTimes(
        value.As<v8::Boolean>()->Value());
  }
}

void Open(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() >= 1 && args[0]->IsString()) {
    JsApi* api = JsApi::Get(args.GetIsolate());
    std::string what = api->js()->ToString(args[0]);
#if defined(WINDOWJS_WIN)
    ShellExecuteA(0, 0, what.c_str(), 0, 0, SW_SHOW);
#elif defined(WINDOWJS_MAC)
    std::string command = "open \"" + what + "\"";
    system(command.c_str());
#else
    std::string command = "xdg-open \"" + what + "\"";
    IGNORE_RESULT(system(command.c_str()));
#endif
  }
}

void GetRetinaScale(v8::Local<v8::Name> property,
                    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window()->retina_scale());
}

void GetVersion(v8::Local<v8::Name> property,
                const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->js()->MakeString(GetVersionString()));
}

void GetPlatform(v8::Local<v8::Name> property,
                 const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
#if defined(WINDOWJS_WIN)
  info.GetReturnValue().Set(api->js()->MakeString("Windows"));
#elif defined(WINDOWJS_MAC)
  info.GetReturnValue().Set(api->js()->MakeString("macOS"));
#elif defined(WINDOWJS_LINUX)
  info.GetReturnValue().Set(api->js()->MakeString("Linux"));
#else
#error "Missing platform configuration for window.platform."
#endif
}

}  // namespace

JsApi::JsApi(Window* win, Js* js, JsEvents* events, TaskQueue* task_queue,
             ThreadPoolTaskQueue* background_queue)
    : weak_factory_(this),
      window_(win),
      js_(js),
      events_(events),
      task_queue_(task_queue),
      background_queue_(background_queue),
      next_timeout_id_(0),
      animation_frame_base_id_(0),
      animation_frame_next_id_(0),
      cursor_shape_(0),
      cursor_image_(nullptr),
      cursor_x_(0),
      cursor_y_(0),
      cursor_(nullptr),
      parent_process_(nullptr) {
  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] create JS APIs start: " << glfwGetTime();
  }

  v8::Locker locker(js->isolate());
  JsScope scope(js);

  js->isolate()->SetData(1, this);

  v8::Local<v8::Object> global = js->global();

  scope.Set(global, StringId::setTimeout, SetTimeout);
  scope.Set(global, StringId::clearTimeout, ClearTimeout);
  scope.Set(global, StringId::requestAnimationFrame, RequestAnimationFrame);
  scope.Set(global, StringId::cancelAnimationFrame, CancelAnimationFrame);
  scope.Set(global, StringId::devicePixelRatio, DevicePixelRatio);

  v8::Local<v8::Object> memory = v8::Object::New(scope.isolate);
  scope.Set(memory, StringId::jsHeapSizeLimit, JsHeapSizeLimit);
  scope.Set(memory, StringId::totalJSHeapSize, TotalJsHeapSize);
  scope.Set(memory, StringId::usedJSHeapSize, UsedJsHeapSize);

  v8::Local<v8::Object> performance = v8::Object::New(scope.isolate);
  scope.Set(performance, StringId::now, Now);
  scope.SetValue(performance, StringId::memory, memory);
  scope.SetValue(global, StringId::performance, performance);

  v8::Local<v8::Object> window = v8::Object::New(scope.isolate);
  scope.Set(window, StringId::close, Close);
  scope.Set(window, StringId::focus, Focus);
  scope.Set(window, StringId::requestAttention, RequestAttention);
  scope.Set(window, StringId::minimize, Minimize);
  scope.Set(window, StringId::maximize, Maximize);
  scope.Set(window, StringId::restore, Restore);
  scope.Set(window, StringId::title, GetTitle, SetTitle);
  scope.Set(window, StringId::width, GetWidth, SetWidth);
  scope.Set(window, StringId::height, GetHeight, SetHeight);
  scope.Set(window, StringId::frameLeft, GetFrameLeft);
  scope.Set(window, StringId::frameRight, GetFrameRight);
  scope.Set(window, StringId::frameTop, GetFrameTop);
  scope.Set(window, StringId::frameBottom, GetFrameBottom);
  scope.Set(window, StringId::x, GetX, SetX);
  scope.Set(window, StringId::y, GetY, SetY);
  scope.Set(window, StringId::visible, GetVisible, SetVisible);
  scope.Set(window, StringId::decorated, GetDecorated, SetDecorated);
  scope.Set(window, StringId::resizable, GetResizable, SetResizable);
  scope.Set(window, StringId::alwaysOnTop, GetAlwaysOnTop, SetAlwaysOnTop);
  scope.Set(window, StringId::keepAspectRatio, GetKeepAspectRatio,
            SetKeepAspectRatio);
  scope.Set(window, StringId::focused, GetFocused);
  scope.Set(window, StringId::maximized, GetMaximized);
  scope.Set(window, StringId::minimized, GetMinimized);
  scope.Set(window, StringId::fullscreen, GetFullscreen, SetFullscreen);
  scope.Set(window, StringId::vsync, GetVsync, SetVsync);
  scope.Set(window, StringId::addEventListener, AddEventListener);
  scope.Set(window, StringId::removeEventListener, RemoveEventListener);
  scope.Set(window, StringId::fonts, GetFonts);
  scope.Set(window, StringId::js, GetJs);
  scope.Set(window, StringId::icon, GetIcon, SetIcon);
  scope.Set(window, StringId::cursor, GetCursor, SetCursor);
  scope.Set(window, StringId::cursorOffsetX, GetCursorOffsetX,
            SetCursorOffsetX);
  scope.Set(window, StringId::cursorOffsetY, GetCursorOffsetY,
            SetCursorOffsetY);
  scope.Set(window, StringId::getClipboardText, GetClipboardText);
  scope.Set(window, StringId::setClipboardText, SetClipboardText);
  scope.Set(window, StringId::loadFont, LoadFont);
  scope.Set(window, StringId::open, Open);
  scope.Set(window, StringId::retinaScale, GetRetinaScale);
  scope.Set(window, StringId::version, GetVersion);
  scope.Set(window, StringId::platform, GetPlatform);
  scope.Set(global, StringId::window, window);

  v8::Local<v8::Object> debug = v8::Object::New(scope.isolate);
  scope.Set(debug, StringId::showOverlayConsole, GetShowOverlayConsole,
            SetShowOverlayConsole);
  scope.Set(debug, StringId::showOverlayConsoleOnErrors,
            GetShowOverlayConsoleOnErrors, SetShowOverlayConsoleOnErrors);
  scope.Set(debug, StringId::overlayConsoleTextColor,
            GetOverlayConsoleTextColor, SetOverlayConsoleTextColor);
  scope.Set(debug, StringId::showOverlayStats, GetShowOverlayStats,
            SetShowOverlayStats);
  scope.Set(debug, StringId::profileFrameTimes, GetProfileFrameTimes,
            SetProfileFrameTimes);
  scope.SetValue(window, StringId::debug, debug);

  v8::Local<v8::Object> screen = v8::Object::New(scope.isolate);
  scope.Set(screen, StringId::availWidth, AvailWidth);
  scope.Set(screen, StringId::availHeight, AvailHeight);
  scope.Set(screen, StringId::width, ScreenWidth);
  scope.Set(screen, StringId::height, ScreenHeight);
  scope.SetValue(window, StringId::screen, screen);

  v8::Local<v8::Function> process = ProcessApi::GetConstructor(this, scope);
  process_constructor_.Reset(scope.isolate, process);
  scope.Set(global, StringId::Process, process);
  parent_process_ = ProcessApi::MaybeAttachToParent(this, scope, process);

  v8::Local<v8::Function> image_data =
      ImageDataApi::GetConstructor(this, scope);
  image_data_constructor_.Reset(scope.isolate, image_data);
  scope.Set(global, StringId::ImageData, image_data);

  v8::Local<v8::Function> image_bitmap =
      ImageBitmapApi::GetConstructor(this, scope);
  image_bitmap_constructor_.Reset(scope.isolate, image_bitmap);
  scope.Set(global, StringId::ImageBitmap, image_bitmap);

  v8::Local<v8::Function> canvas_gradient =
      CanvasGradientApi::GetConstructor(this, scope);
  canvas_gradient_constructor_.Reset(scope.isolate, canvas_gradient);
  scope.Set(global, StringId::CanvasGradient, canvas_gradient);

  v8::Local<v8::Function> canvas_pattern =
      CanvasPatternApi::GetConstructor(this, scope);
  canvas_pattern_constructor_.Reset(scope.isolate, canvas_pattern);
  scope.Set(global, StringId::CanvasPattern, canvas_pattern);

  v8::Local<v8::Function> canvas_context =
      CanvasApi::GetConstructor(this, scope);
  canvas_rendering_context_2d_constructor_.Reset(scope.isolate, canvas_context);
  scope.Set(global, StringId::CanvasRenderingContext2D, canvas_context);

  v8::Local<v8::Value> args[2] = {
      v8::Number::New(scope.isolate, window_->width()),
      v8::Number::New(scope.isolate, window_->height()),
  };

  v8::Local<v8::Object> canvas =
      canvas_context->NewInstance(scope.context, 2, args).ToLocalChecked();
  scope.Set(window, StringId::canvas, canvas);
  CanvasApi* window_canvas = GetCanvasApi(canvas);
  ASSERT(window_canvas);
  window_->SetWindowCanvas(window_canvas->canvas());

  scope.Set(global, StringId::Codec, MakeCodecApi(this, scope));
  scope.Set(global, StringId::File, MakeFileApi(this, scope));

  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] create JS APIs end: " << glfwGetTime();
  }
}

JsApi::~JsApi() {
  if (cursor_) {
    glfwDestroyCursor(cursor_);
    cursor_ = nullptr;
  }
  // Delete all of the wrapped C++ objects that weren't cleaned up by GC so far.
  v8::Locker locker(isolate());
  v8::HandleScope scope(isolate());
  isolate()->VisitHandlesWithClassIds(this);
}

void JsApi::VisitPersistentHandle(v8::Persistent<v8::Value>* value,
                                  uint16_t class_id) {
  ASSERT(class_id == wrapper_class_id_for_destructors);
  v8::Local<v8::Value> v = value->Get(isolate());
  ASSERT(v->IsObject());
  v8::Local<v8::Object> o = v.As<v8::Object>();
  JsApiWrapper* w =
      static_cast<JsApiWrapper*>(o->GetAlignedPointerFromInternalField(0));
  delete w;
}

void* JsApi::GetWrappedInstanceOrThrow(v8::Local<v8::Value> thiz,
                                       v8::Local<v8::Function> constructor) {
  if (IsInstanceOf(thiz, constructor)) {
    // The internal pointer is set in JsApiWrapper::JsApiWrapper.
    return thiz.As<v8::Object>()->GetAlignedPointerFromInternalField(0);
  } else {
    js_->ThrowIllegalInvocation();
    return nullptr;
  }
}

// static
void JsApi::SetTimeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());

  if (args.Length() < 1 || !args[0]->IsFunction()) {
    api->js()->ThrowError("setTimeout requires a function argument.");
    return;
  }

  v8::Global<v8::Function> f(args.GetIsolate(), args[0].As<v8::Function>());

  double timeout = 0;
  if (args.Length() >= 2 && args[1]->IsNumber()) {
    timeout = args[1].As<v8::Number>()->Value();
    // From milliseconds to seconds.
    timeout /= 1000.0;
    if (timeout < 0) {
      timeout = 0;
    }
  }

  uint32_t id = api->next_timeout_id_++;
  args.GetReturnValue().Set(id);

  api->timeouts_[id] = std::move(f);
  api->task_queue()->Post(timeout, [=] {
    api->CallTimeout(id);
  });
}

// static
void JsApi::ClearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());

  if (args.Length() < 1 || !args[0]->IsUint32()) {
    api->js()->ThrowError("clearTimeout requires an int argument.");
    return;
  }

  uint32_t id = args[0].As<v8::Uint32>()->Value();
  api->timeouts_.erase(id);
}

void JsApi::CallTimeout(uint32_t id) {
  auto it = timeouts_.find(id);
  if (it == timeouts_.end()) {
    // Removed by ClearTimeout.
    return;
  }

  JsScope scope(js_);
  v8::TryCatch try_catch(scope.isolate);

  v8::Local<v8::Function> callback = it->second.Get(scope.isolate);
  timeouts_.erase(it);

  IGNORE_RESULT(callback->Call(scope.context, js_->global(), 0, {}));

  if (try_catch.HasCaught()) {
    js_->ReportException(try_catch.Message());
  }
}

// static
void JsApi::RequestAnimationFrame(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1 || !args[0]->IsFunction()) {
    args.GetReturnValue().Set(-1);
    return;
  }
  JsApi* api = JsApi::Get(args.GetIsolate());
  api->animation_frame_callbacks_.emplace_back(api->isolate(),
                                               args[0].As<v8::Function>());
  uint32_t id = api->animation_frame_next_id_++;
  args.GetReturnValue().Set(id);
}

// static
void JsApi::CancelAnimationFrame(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1 || !args[0]->IsUint32()) {
    return;
  }
  JsApi* api = JsApi::Get(args.GetIsolate());
  uint32_t n = args[0].As<v8::Uint32>()->Value();
  if (n < api->animation_frame_base_id_ || n >= api->animation_frame_next_id_) {
    return;
  }
  n -= api->animation_frame_base_id_;
  if (n >= 0 && n < api->animation_frame_callbacks_.size()) {
    api->animation_frame_callbacks_[n].Reset();
  }
}

void JsApi::CallAnimationFrameCallbacks(const JsScope& scope) {
  if (animation_frame_callbacks_.empty()) {
    return;
  }

  animation_frame_base_id_ = animation_frame_next_id_;

  std::vector<v8::Global<v8::Function>> callbacks;
  callbacks.swap(animation_frame_callbacks_);

  v8::TryCatch try_catch(scope.isolate);

  v8::Local<v8::Number> now =
      v8::Number::New(scope.isolate, glfwGetTime() * 1000);
  v8::Local<v8::Value> args[] = {now};

  for (const v8::Global<v8::Function>& callback : callbacks) {
    if (callback.IsEmpty()) {
      // Cancelled by cancelAnimationFrame.
      continue;
    }

    v8::Local<v8::Function> f =
        v8::Local<v8::Function>::New(scope.isolate, callback);
    IGNORE_RESULT(f->Call(scope.context, scope.context->Global(), 1, args));

    if (try_catch.HasCaught()) {
      js_->ReportException(try_catch.Message());
      if (!try_catch.CanContinue()) {
        return;
      }
      try_catch.Reset();
    }
  }
}

v8::Local<v8::Promise> JsApi::PostToBackgroundAndResolve(
    BackgroundFunction background_task) {
  ASSERT(IsMainThread());

  v8::Isolate* isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Promise::Resolver> resolver =
      v8::Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
  size_t index = StorePendingPromise(isolate, resolver);

  WeakPtr<JsApi> weak_this = weak_factory_.MakeWeakPtr();
  TaskQueue* task_queue = task_queue_;

  background_queue_->Post(
      [weak_this, task_queue, index, b = std::move(background_task)] {
        ASSERT(!IsMainThread());

        ResolveFunction resolve_task = b();

        // Subtle: this is safe because the task_queue_ is deleted *after* the
        // background_queue_, and the background_queue_ joins its threads at
        // shutdown. So as long as the background task is executing, the
        // TaskQueue* instance is still valid.
        task_queue->Post([weak_this, index, r = std::move(resolve_task)] {
          ASSERT(IsMainThread());

          JsApi* thiz = weak_this.Get();
          if (!thiz) {
            // The original JsApi instance was deleted while the background task
            // was executing.
            return;
          }

          JsScope scope(thiz->js_);
          v8::TryCatch try_catch(scope.isolate);
          v8::Local<v8::Promise::Resolver> resolver =
              thiz->ReleasePendingPromise(scope.isolate, index);
          r(thiz, scope, *resolver);
          if (try_catch.HasCaught()) {
            if (resolver->GetPromise()->State() == v8::Promise::kPending) {
              IGNORE_RESULT(
                  resolver->Reject(scope.context, try_catch.Message()->Get()));
            }
          }
          ASSERT(resolver->GetPromise()->State() != v8::Promise::kPending);
        });
      });

  return resolver->GetPromise();
}

// static
JsApi::ResolveFunction JsApi::Reject(std::string reason) {
  return [s = std::move(reason)](JsApi* api, const JsScope& scope,
                                 v8::Promise::Resolver* resolver) {
    IGNORE_RESULT(
        resolver->Reject(scope.context, api->js()->MakeString(std::move(s))));
  };
}

// static
JsApi::ResolveFunction JsApi::Resolve() {
  return [](JsApi* api, const JsScope& scope, v8::Promise::Resolver* resolver) {
    IGNORE_RESULT(
        resolver->Resolve(scope.context, v8::Undefined(scope.isolate)));
  };
}

// static
JsApi::ResolveFunction JsApi::Resolve(bool value) {
  return [value](JsApi* api, const JsScope& scope,
                 v8::Promise::Resolver* resolver) {
    IGNORE_RESULT(
        resolver->Resolve(scope.context, value ? v8::True(scope.isolate)
                                               : v8::False(scope.isolate)));
  };
}

// static
JsApi::ResolveFunction JsApi::Resolve(double value) {
  return [value](JsApi* api, const JsScope& scope,
                 v8::Promise::Resolver* resolver) {
    IGNORE_RESULT(resolver->Resolve(scope.context,
                                    v8::Number::New(scope.isolate, value)));
  };
}

// static
JsApi::ResolveFunction JsApi::Resolve(std::string value) {
  return [s = std::move(value)](JsApi* api, const JsScope& scope,
                                v8::Promise::Resolver* resolver) {
    IGNORE_RESULT(
        resolver->Resolve(scope.context, api->js()->MakeString(std::move(s))));
  };
}

size_t JsApi::StorePendingPromise(v8::Isolate* isolate,
                                  v8::Local<v8::Promise::Resolver> resolver) {
  for (size_t index = 0; index < pending_promises_.size(); index++) {
    if (pending_promises_[index].IsEmpty()) {
      pending_promises_[index].Reset(isolate, resolver);
      return index;
    }
  }
  pending_promises_.emplace_back(isolate, resolver);
  return pending_promises_.size() - 1;
}

v8::Local<v8::Promise::Resolver> JsApi::ReleasePendingPromise(
    v8::Isolate* isolate, size_t index) {
  ASSERT(IsMainThread());
  ASSERT(index < pending_promises_.size());
  ASSERT(!pending_promises_[index].IsEmpty());
  v8::Local<v8::Promise::Resolver> resolver =
      pending_promises_[index].Get(isolate);
  pending_promises_[index].Reset();
  return resolver;
}

// static
void JsApi::GetIcon(v8::Local<v8::Name> property,
                    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window_icon_);
}

// static
void JsApi::SetIcon(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                    const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  if (!value->IsArray()) {
    api->js()->ThrowInvalidArgument();
    return;
  }
  v8::Local<v8::Array> array = value.As<v8::Array>();
  std::vector<GLFWimage> icons;
  icons.resize(array->Length());
  for (size_t i = 0; i < array->Length(); i++) {
    v8::Local<v8::Value> icon =
        array->Get(api->js()->context(), i).ToLocalChecked();
    if (!api->IsInstanceOf(icon, api->GetImageDataConstructor())) {
      api->js()->ThrowInvalidArgument();
      return;
    }
    icons[i] = api->GetImageDataApi(icon)->ToGLFWImage();
  }
  api->window_icon_.Reset(api->js()->isolate(), array);
  api->window_->SetIcons(std::move(icons));
}

// static
void JsApi::GetCursor(v8::Local<v8::Name> property,
                      const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->window_cursor_);
}

// static
void JsApi::SetCursor(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                      const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  if (value->IsNull() || value->IsUndefined()) {
    api->cursor_shape_ = 0;
    api->cursor_image_ = nullptr;
  } else if (value->IsString()) {
    std::string shape = api->js()->ToString(value);
    if (shape == "arrow") {
      api->cursor_shape_ = GLFW_ARROW_CURSOR;
    } else if (shape == "ibeam") {
      api->cursor_shape_ = GLFW_IBEAM_CURSOR;
    } else if (shape == "crosshair") {
      api->cursor_shape_ = GLFW_CROSSHAIR_CURSOR;
    } else if (shape == "hand") {
      api->cursor_shape_ = GLFW_HAND_CURSOR;
    } else if (shape == "hresize") {
      api->cursor_shape_ = GLFW_HRESIZE_CURSOR;
    } else if (shape == "vresize") {
      api->cursor_shape_ = GLFW_VRESIZE_CURSOR;
    } else if (shape == "hidden") {
      glfwSetInputMode(api->glfw_window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
      return;
    } else if (shape == "locked") {
      glfwSetInputMode(api->glfw_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      api->window_cursor_.Reset(api->js()->isolate(), value);
      return;
    } else {
      api->js()->ThrowInvalidArgument();
      return;
    }
    api->cursor_image_ = nullptr;
  } else if (api->IsInstanceOf(value, api->GetImageDataConstructor())) {
    api->cursor_shape_ = 0;
    api->cursor_image_ = api->GetImageDataApi(value);
  } else {
    api->js()->ThrowInvalidArgument();
    return;
  }
  glfwSetInputMode(api->glfw_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  api->window_cursor_.Reset(api->js()->isolate(), value);
  api->UpdateCursor();
}

// static
void JsApi::GetCursorOffsetX(v8::Local<v8::Name> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->cursor_x_);
}

// static
void JsApi::SetCursorOffsetX(v8::Local<v8::Name> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  if (value->IsNumber()) {
    api->cursor_x_ = value.As<v8::Number>()->Value();
    if (api->cursor_shape_ == 0 && api->cursor_image_ != nullptr) {
      api->UpdateCursor();
    }
  } else {
    api->js()->ThrowInvalidArgument();
  }
}

// static
void JsApi::GetCursorOffsetY(v8::Local<v8::Name> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  info.GetReturnValue().Set(api->cursor_y_);
}

// static
void JsApi::SetCursorOffsetY(v8::Local<v8::Name> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  if (value->IsNumber()) {
    api->cursor_y_ = value.As<v8::Number>()->Value();
    if (api->cursor_shape_ == 0 && api->cursor_image_ != nullptr) {
      api->UpdateCursor();
    }
  } else {
    api->js()->ThrowInvalidArgument();
  }
}

void JsApi::UpdateCursor() {
  if (cursor_) {
    glfwDestroyCursor(cursor_);
    cursor_ = nullptr;
  }
  if (cursor_shape_ > 0) {
    cursor_ = glfwCreateStandardCursor(cursor_shape_);
  } else if (cursor_image_) {
    GLFWimage image = cursor_image_->ToGLFWImage();
    cursor_ = glfwCreateCursor(&image, cursor_x_, cursor_y_);
  }
  window_->SetCursor(cursor_);
}

// static
void JsApi::AddEventListener(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());

  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    api->js()->ThrowError(
        "addEventListener requires an event type and a callback function");
    return;
  }

  std::string type = api->js()->ToString(args[0]);
  v8::Local<v8::Function> f = args[1].As<v8::Function>();

  api->events_->AddEventListener(type, f, args.GetIsolate());
}

// static
void JsApi::RemoveEventListener(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());

  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    api->js()->ThrowError(
        "removeEventListener requires an event type and a callback function");
    return;
  }

  std::string type = api->js()->ToString(args[0]);
  v8::Local<v8::Function> f = args[1].As<v8::Function>();

  api->events_->RemoveEventListener(type, f);
}

// static
void JsApi::LoadFont(const v8::FunctionCallbackInfo<v8::Value>& args) {
  JsApi* api = JsApi::Get(args.GetIsolate());
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  std::string path = api->js()->ToString(args[0]);
  std::string name = api->js()->ToString(args[1]);

  args.GetReturnValue().Set(api->PostToBackgroundAndResolve(
      [path = std::move(path),
       name = std::move(name)]() -> JsApi::ResolveFunction {
        std::string content;
        std::string error;
        if (!ReadFile(path, &content, &error)) {
          return JsApi::Reject("Failed to load font at " + path + ": " + error);
        }
        return [path = std::move(path), name = std::move(name),
                content = std::move(content)](JsApi* api, const JsScope& scope,
                                              v8::Promise::Resolver* resolver) {
          sk_sp<SkData> data =
              SkData::MakeWithCopy(content.data(), content.size());
          sk_sp<SkTypeface> font = SkTypeface::MakeFromData(data);
          if (!font) {
            IGNORE_RESULT(resolver->Reject(
                scope.context, scope.MakeString("Failed to load font at " +
                                                path + ": failed to decode")));
          }
          api->fonts_[name] = font;
          IGNORE_RESULT(
              resolver->Resolve(scope.context, v8::Undefined(scope.isolate)));
        };
      }));
}

JsApiWrapper::JsApiWrapper(v8::Isolate* isolate, v8::Local<v8::Object> thiz)
    : api_(JsApi::Get(isolate)) {
  thiz->SetAlignedPointerInInternalField(0, this);
  thiz_.Reset(isolate, thiz);
  thiz_.SetWeak(this, JsApiWrapper::Destructor,
                v8::WeakCallbackType::kParameter);
  thiz_.SetWrapperClassId(wrapper_class_id_for_destructors);
}

// static
void JsApiWrapper::Destructor(const v8::WeakCallbackInfo<JsApiWrapper>& info) {
  // This gets called when the Javascript object gets garbage collected.
  JsApiWrapper* thiz = static_cast<JsApiWrapper*>(info.GetParameter());
  delete thiz;
}
