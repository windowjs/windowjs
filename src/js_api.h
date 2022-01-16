#ifndef WINDOWJS_JS_API_H
#define WINDOWJS_JS_API_H

#include <functional>
#include <unordered_map>
#include <vector>

#include <skia/include/core/SkRefCnt.h>

#include "fail.h"
#include "js.h"
#include "js_events.h"
#include "task_queue.h"
#include "thread.h"
#include "weak.h"
#include "window.h"

class CanvasApi;
class CanvasGradientApi;
class ImageBitmapApi;
class ImageDataApi;
class ProcessApi;
class SkTypeface;

class JsApi final : public v8::PersistentHandleVisitor {
 public:
  // All of these dependencies must outlive JsApi.
  // If the JsApi is deleted, then TaskQueue must *not* run any pending tasks
  // anymore.
  JsApi(Window* window, Js* js, JsEvents* events, TaskQueue* task_queue,
        ThreadPoolTaskQueue* background_queue);
  ~JsApi();

  Window* window() const { return window_; }
  GLFWwindow* glfw_window() const { return window_->window(); }
  Js* js() const { return js_; }
  v8::Isolate* isolate() const { return js_->isolate(); }
  TaskQueue* task_queue() const { return task_queue_; }
  ThreadPoolTaskQueue* background_queue() const { return background_queue_; }
  ProcessApi* parent_process() const { return parent_process_; }
  RenderCanvas* window_canvas() const { return window_->canvas(); }
  RenderCanvasSharedContext* canvas_shared_context() const {
    return window_->shared_context();
  }

  const std::unordered_map<std::string, sk_sp<SkTypeface>>& fonts() const {
    return fonts_;
  }

  std::unordered_map<std::string, sk_sp<SkTypeface>>* fonts_cache() {
    return &fonts_cache_;
  }

  bool has_animation_frame_callbacks() const {
    return !animation_frame_callbacks_.empty();
  }

  void CallAnimationFrameCallbacks(const JsScope& scope);

  using ResolveFunction =
      std::function<void(JsApi*, const JsScope&, v8::Promise::Resolver*)>;
  using BackgroundFunction = std::function<ResolveFunction()>;

  // Posts a "background_task" that gets executed in a background thread. Its
  // return value is a "foreground_task", that gets executed in the main thread
  // and is passed a v8::Promise::Resolver to resolve the promise returned by
  // PostToBackgroundAndResolve. Example usage:
  //
  // args.GetReturnValue().Set(api->PostToBackgroundAndResolve([] {
  //   // Runs on a background thread.
  //   std::string content;
  //   ReadFile(some_path_from_args, &content);
  //   return [content = std::move(content)](
  //      JsApi* api, const JsScope& scope, v8::Promise::Resolver* resolver) {
  //     // Runs on the main thread.
  //     resolver->Resolver(scope.context, api->js()->MakeString(content));
  //   };
  // }));
  //
  // Either task gets dropped if the Js object that owns us gets deleted.
  v8::Local<v8::Promise> PostToBackgroundAndResolve(
      BackgroundFunction background_task);

  // Helper to return a failure from PostToBackgroundAndResolve.
  static ResolveFunction Reject(std::string reason);

  // Helpers to return simple values from PostToBackgroundAndResolve.
  static ResolveFunction Resolve();
  static ResolveFunction Resolve(bool value);
  static ResolveFunction Resolve(double value);
  static ResolveFunction Resolve(std::string value);

  static JsApi* Get(v8::Isolate* isolate) {
    return static_cast<JsApi*>(isolate->GetData(1));
  }

  bool IsInstanceOf(v8::Local<v8::Value> object,
                    v8::Local<v8::Function> constructor) {
    return object->InstanceOf(isolate()->GetCurrentContext(), constructor)
        .FromMaybe(false);
  }

  v8::Local<v8::Function> GetCanvasRenderingContext2DConstructor() {
    return canvas_rendering_context_2d_constructor_.Get(js_->isolate());
  }

  CanvasApi* GetCanvasApi(v8::Local<v8::Value> thiz) {
    return GetWrappedInstanceOrThrow<CanvasApi>(
        thiz, GetCanvasRenderingContext2DConstructor());
  }

  v8::Local<v8::Function> GetCanvasGradientConstructor() {
    return canvas_gradient_constructor_.Get(js_->isolate());
  }

  CanvasGradientApi* GetCanvasGradientApi(v8::Local<v8::Value> thiz) {
    return GetWrappedInstanceOrThrow<CanvasGradientApi>(
        thiz, GetCanvasGradientConstructor());
  }

  v8::Local<v8::Function> GetImageDataConstructor() {
    return image_data_constructor_.Get(js_->isolate());
  }

  ImageDataApi* GetImageDataApi(v8::Local<v8::Value> thiz) {
    return GetWrappedInstanceOrThrow<ImageDataApi>(thiz,
                                                   GetImageDataConstructor());
  }

  v8::Local<v8::Function> GetImageBitmapConstructor() {
    return image_bitmap_constructor_.Get(js_->isolate());
  }

  ImageBitmapApi* GetImageBitmapApi(v8::Local<v8::Value> thiz) {
    return GetWrappedInstanceOrThrow<ImageBitmapApi>(
        thiz, GetImageBitmapConstructor());
  }

  v8::Local<v8::Function> GetProcessConstructor() {
    return process_constructor_.Get(js_->isolate());
  }

  ProcessApi* GetProcessApi(v8::Local<v8::Value> thiz) {
    return GetWrappedInstanceOrThrow<ProcessApi>(thiz, GetProcessConstructor());
  }

 private:
  template <typename T>
  T* GetWrappedInstanceOrThrow(v8::Local<v8::Value> thiz,
                               v8::Local<v8::Function> constructor) {
    return static_cast<T*>(GetWrappedInstanceOrThrow(thiz, constructor));
  }

  void* GetWrappedInstanceOrThrow(v8::Local<v8::Value> thiz,
                                  v8::Local<v8::Function> constructor);

  void VisitPersistentHandle(v8::Persistent<v8::Value>* value,
                             uint16_t class_id) override;

  static void SetTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void ClearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
  void CallTimeout(uint32_t id);

  static void RequestAnimationFrame(
      const v8::FunctionCallbackInfo<v8::Value>& args);
  static void CancelAnimationFrame(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  static void GetIcon(v8::Local<v8::Name> property,
                      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetIcon(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                      const v8::PropertyCallbackInfo<void>& info);

  static void GetCursor(v8::Local<v8::Name> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetCursor(v8::Local<v8::Name> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info);
  static void GetCursorOffsetX(v8::Local<v8::Name> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetCursorOffsetX(v8::Local<v8::Name> property,
                               v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);
  static void GetCursorOffsetY(v8::Local<v8::Name> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetCursorOffsetY(v8::Local<v8::Name> property,
                               v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);
  void UpdateCursor();

  static void AddEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void RemoveEventListener(
      const v8::FunctionCallbackInfo<v8::Value>& args);

  static void LoadFont(const v8::FunctionCallbackInfo<v8::Value>& args);

  size_t StorePendingPromise(v8::Isolate* isolate,
                             v8::Local<v8::Promise::Resolver> resolver);

  v8::Local<v8::Promise::Resolver> ReleasePendingPromise(v8::Isolate* isolate,
                                                         size_t index);

  WeakPtrFactory<JsApi> weak_factory_;
  Window* window_;
  Js* js_;
  JsEvents* events_;
  TaskQueue* task_queue_;
  ThreadPoolTaskQueue* background_queue_;

  std::unordered_map<uint32_t, v8::Global<v8::Function>> timeouts_;
  uint32_t next_timeout_id_;

  std::vector<v8::Global<v8::Function>> animation_frame_callbacks_;
  uint32_t animation_frame_base_id_;
  uint32_t animation_frame_next_id_;

  std::vector<v8::Global<v8::Promise::Resolver>> pending_promises_;

  v8::Global<v8::Function> canvas_rendering_context_2d_constructor_;
  v8::Global<v8::Function> canvas_gradient_constructor_;
  v8::Global<v8::Function> image_data_constructor_;
  v8::Global<v8::Function> image_bitmap_constructor_;
  v8::Global<v8::Function> process_constructor_;

  v8::Global<v8::Array> window_icon_;
  v8::Global<v8::Value> window_cursor_;
  int cursor_shape_;
  ImageDataApi* cursor_image_;
  int cursor_x_;
  int cursor_y_;
  GLFWcursor* cursor_;

  std::unordered_map<std::string, sk_sp<SkTypeface>> fonts_;
  std::unordered_map<std::string, sk_sp<SkTypeface>> fonts_cache_;

  CanvasApi* window_canvas_;
  ProcessApi* parent_process_;
};

// All API objects that wrap a C++ native object must extend this class,
// so that they also get cleaned up at shutdown.
class JsApiWrapper {
 public:
  JsApiWrapper(v8::Isolate* isolate, v8::Local<v8::Object> thiz);
  virtual ~JsApiWrapper() {}

  JsApi* api() const { return api_; }
  Js* js() const { return api_->js(); }

  v8::Local<v8::String> GetConstantString(StringId id) const {
    return api_->js()->GetConstantString(id);
  }

 private:
  static void Destructor(const v8::WeakCallbackInfo<JsApiWrapper>& info);

  JsApi* api_;
  v8::Global<v8::Object> thiz_;
};

// Maintains a linked-list of all instances of a given API.
template <typename T>
class JsApiTracker {
 public:
  JsApiTracker() : next_(list), prev_(nullptr) {
    ASSERT(IsMainThread());
    list = this;
    if (next_) {
      next_->prev_ = this;
    }
  }

  virtual ~JsApiTracker() {
    ASSERT(IsMainThread());
    if (list == this) {
      list = next_;
    }
    if (next_) {
      next_->prev_ = prev_;
    }
    if (prev_) {
      prev_->next_ = next_;
    }
  }

  static void ForEach(std::function<void(T*)> f) {
    for (JsApiTracker* tracker = list; tracker != nullptr;
         tracker = tracker->next_) {
      f(static_cast<T*>(tracker));
    }
  }

 private:
  static inline JsApiTracker* list = nullptr;

  JsApiTracker* next_;
  JsApiTracker* prev_;
};

#endif  // WINDOWJS_JS_API_H
