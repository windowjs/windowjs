#ifndef WINDOWJS_JS_API_CANVAS_H
#define WINDOWJS_JS_API_CANVAS_H

#include <deque>
#include <functional>
#include <memory>

#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkShader.h>
#include <v8/include/v8.h>

#include "js_api.h"
#include "js_scope.h"
#include "render_canvas.h"

class CanvasGradientApi;
class CanvasPatternApi;

class CanvasApi final : public JsApiWrapper, public JsApiTracker<CanvasApi> {
 public:
  CanvasApi(JsApi* api, v8::Local<v8::Object> thiz, int width, int height);
  ~CanvasApi() override;

  RenderCanvas* canvas() const { return canvas_.get(); }
  SkCanvas* skia_canvas() const { return canvas_->canvas(); }

  void OnGradientUpdated(CanvasGradientApi* gradient);
  void OnPatternUpdated(CanvasPatternApi* pattern);

  static v8::Local<v8::Function> GetConstructor(JsApi* api,
                                                const JsScope& scope);

 private:
  static void GetWidth(v8::Local<v8::String> property,
                       const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetWidth(v8::Local<v8::String> property,
                       v8::Local<v8::Value> value,
                       const v8::PropertyCallbackInfo<void>& info);
  static void GetHeight(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetHeight(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info);
  static void GetFillStyle(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetFillStyle(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void>& info);
  static void GetStrokeStyle(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetStrokeStyle(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info);
  static void ClearRect(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void FillRect(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void StrokeRect(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void GetFont(v8::Local<v8::String> property,
                      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetFont(v8::Local<v8::String> property,
                      v8::Local<v8::Value> value,
                      const v8::PropertyCallbackInfo<void>& info);
  static void GetLineWidth(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetLineWidth(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void>& info);
  static void GetLineCap(v8::Local<v8::String> property,
                         const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetLineCap(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value,
                         const v8::PropertyCallbackInfo<void>& info);
  static void GetLineJoin(v8::Local<v8::String> property,
                          const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetLineJoin(v8::Local<v8::String> property,
                          v8::Local<v8::Value> value,
                          const v8::PropertyCallbackInfo<void>& info);
  static void GetMiterLimit(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetMiterLimit(v8::Local<v8::String> property,
                            v8::Local<v8::Value> value,
                            const v8::PropertyCallbackInfo<void>& info);
  static void GetLineDash(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void SetLineDash(const v8::FunctionCallbackInfo<v8::Value>& info);
  void UpdateLineDash();
  static void GetLineDashOffset(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetLineDashOffset(v8::Local<v8::String> property,
                                v8::Local<v8::Value> value,
                                const v8::PropertyCallbackInfo<void>& info);
  static void GetTextAlign(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetTextAlign(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void>& info);
  static void GetTextBaseline(v8::Local<v8::String> property,
                              const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetTextBaseline(v8::Local<v8::String> property,
                              v8::Local<v8::Value> value,
                              const v8::PropertyCallbackInfo<void>& info);
  static void GetGlobalAlpha(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetGlobalAlpha(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info);
  static void GetGlobalCompositeOperation(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetGlobalCompositeOperation(
      v8::Local<v8::String> property, v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<void>& info);
  static void GetShadowBlur(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetShadowBlur(v8::Local<v8::String> property,
                            v8::Local<v8::Value> value,
                            const v8::PropertyCallbackInfo<void>& info);
  static void GetShadowColor(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetShadowColor(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info);
  static void GetShadowOffsetX(v8::Local<v8::String> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetShadowOffsetX(v8::Local<v8::String> property,
                               v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);
  static void GetShadowOffsetY(v8::Local<v8::String> property,
                               const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetShadowOffsetY(v8::Local<v8::String> property,
                               v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info);
  void UpdateShadow();
  static void GetAntiAlias(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetAntiAlias(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void>& info);
  static void GetImageSmoothingEnabled(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetImageSmoothingEnabled(
      v8::Local<v8::String> property, v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<void>& info);
  static void GetImageSmoothingQuality(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void SetImageSmoothingQuality(
      v8::Local<v8::String> property, v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<void>& info);
  void UpdateImageSmoothing();
  static void DrawText(const v8::FunctionCallbackInfo<v8::Value>& info,
                       const SkPaint& paint, CanvasApi* api);
  static void FillText(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void StrokeText(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void MeasureText(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void BeginPath(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void ClosePath(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void MoveTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void LineTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Arc(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void ArcTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Ellipse(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Rect(const v8::FunctionCallbackInfo<v8::Value>& info);
  bool ParseFillParameters(const v8::FunctionCallbackInfo<v8::Value>& info,
                           SkPath* path);
  static void Fill(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Stroke(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Clip(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void IsPointInPath(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void IsPointInStroke(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Rotate(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Scale(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Translate(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Transform(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void GetTransform(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void SetTransform(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void ResetTransform(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Save(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Restore(const v8::FunctionCallbackInfo<v8::Value>& info);

  static void CreateGradient(const v8::FunctionCallbackInfo<v8::Value>& info,
                             int size, const char* function);
  static void CreateLinearGradient(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void CreateRadialGradient(
      const v8::FunctionCallbackInfo<v8::Value>& info);
  static void CreatePattern(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void CreateImageData(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void GetImageData(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void PutImageData(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Encode(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void DrawImage(const v8::FunctionCallbackInfo<v8::Value>& info);

  std::unique_ptr<RenderCanvas> canvas_;
  int64_t allocated_in_bytes_;

  SkPath path_;

  struct State {
    SkPaint fill_paint;
    SkColor fill_color;
    CanvasGradientApi* fill_gradient = nullptr;
    CanvasPatternApi* fill_pattern = nullptr;
    SkPaint stroke_paint;
    SkColor stroke_color;
    CanvasGradientApi* stroke_gradient = nullptr;
    CanvasPatternApi* stroke_pattern = nullptr;
    float global_alpha;
    SkBlendMode global_composite_op;
    SkFont font;
    std::vector<float> line_dash;
    float line_dash_offset;
    StringId text_align;
    StringId text_baseline;
    SkColor shadow_color;
    float shadow_offsetx;
    float shadow_offsety;
    float shadow_blur;
    bool antialias = true;
    SkSamplingOptions sampling_options =
        SkSamplingOptions(SkCubicResampler::Mitchell());
    bool image_smoothing = true;
    int image_smoothing_quality = 2;

    void ResetFillStyle();
    void ResetStrokeStyle();
  };

  State state_;
  std::deque<State> saved_state_;
};

class CanvasGradientApi final : public JsApiWrapper {
 public:
  CanvasGradientApi(JsApi* api, v8::Local<v8::Object> thiz,
                    const v8::FunctionCallbackInfo<v8::Value>& info);
  ~CanvasGradientApi() override;

  void Ref() { ref_count_++; }
  void Unref() { ref_count_--; }

  sk_sp<SkShader> GetShader();

  static v8::Local<v8::Function> GetConstructor(JsApi* api,
                                                const JsScope& scope);

 private:
  static void AddColorStop(const v8::FunctionCallbackInfo<v8::Value>& info);

  enum GradientType {
    kLinear,
    kRadial,
  };

  sk_sp<SkShader> shader_;
  GradientType type_;
  std::vector<float> params_;
  std::vector<SkColor> colors_;
  std::vector<SkScalar> positions_;
  int ref_count_;
};

class CanvasPatternApi final : public JsApiWrapper {
 public:
  CanvasPatternApi(JsApi* api, v8::Local<v8::Object> thiz,
                   const v8::FunctionCallbackInfo<v8::Value>& info);
  ~CanvasPatternApi() override;

  void Ref() { ref_count_++; }
  void Unref() { ref_count_--; }

  sk_sp<SkShader> GetShader();

  static v8::Local<v8::Function> GetConstructor(JsApi* api,
                                                const JsScope& scope);

 private:
  static void SetTransform(const v8::FunctionCallbackInfo<v8::Value>& info);

  sk_sp<SkShader> shader_;
  sk_sp<SkImage> pattern_;
  int ref_count_;
};

class ImageDataApi final : public JsApiWrapper {
 public:
  ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz, int width, int height);
  ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
               v8::Local<v8::Uint8Array> data, int width, int height);
  ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
               v8::Local<v8::Uint8ClampedArray> data, int width, int height);
  ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
               v8::Local<v8::Uint16Array> data, int width, int height);
  ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
               v8::Local<v8::Float32Array> data, int width, int height);
  ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
               const ImageDataApi* source, int x, int y, int w, int h);
  ~ImageDataApi() override;

  std::shared_ptr<v8::BackingStore> backing_store() const {
    return backing_store_;
  }
  int width() const { return width_; }
  int height() const { return height_; }

  GLFWimage ToGLFWImage() const {
    return GLFWimage{
        width_,
        height_,
        static_cast<unsigned char*>(backing_store_->Data()),
    };
  }

  static v8::Local<v8::Function> GetConstructor(JsApi* api,
                                                const JsScope& scope);

 private:
  static void GetData(v8::Local<v8::String> property,
                      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetWidth(v8::Local<v8::String> property,
                       const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetHeight(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info);
  static void Encode(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Decode(const v8::FunctionCallbackInfo<v8::Value>& info);

  v8::Global<v8::Object> data_;
  std::shared_ptr<v8::BackingStore> backing_store_;
  int width_;
  int height_;
};

class ImageBitmapApi final : public JsApiWrapper {
 public:
  ImageBitmapApi(JsApi* api, v8::Local<v8::Object> thiz,
                 sk_sp<SkImage> texture);
  ~ImageBitmapApi() override;

  sk_sp<SkImage> texture() const { return texture_; }
  int width() const { return texture_->width(); }
  int height() const { return texture_->height(); }

  static v8::Local<v8::Function> GetConstructor(JsApi* api,
                                                const JsScope& scope);

 private:
  static void GetWidth(v8::Local<v8::String> property,
                       const v8::PropertyCallbackInfo<v8::Value>& info);
  static void GetHeight(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info);
  static void Encode(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Decode(const v8::FunctionCallbackInfo<v8::Value>& info);

  sk_sp<SkImage> texture_;
};

class Path2DApi final : public JsApiWrapper {
 public:
  Path2DApi(JsApi* api, v8::Local<v8::Object> thiz, const SkPath& path);
  ~Path2DApi() override;

  const SkPath& path() const { return path_; }

  static v8::Local<v8::Function> GetConstructor(JsApi* api,
                                                const JsScope& scope);

 private:
  static void AddPath(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void ClosePath(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void MoveTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void LineTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Arc(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void ArcTo(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Ellipse(const v8::FunctionCallbackInfo<v8::Value>& info);
  static void Rect(const v8::FunctionCallbackInfo<v8::Value>& info);

  SkPath path_;
};

#endif  // WINDOWJS_JS_API_CANVAS_H
