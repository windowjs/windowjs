#include "js_api_canvas.h"

#include <unordered_map>

#include <skia/include/core/SkFontMetrics.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/effects/SkDashPathEffect.h>
#include <skia/include/effects/SkGradientShader.h>
#include <skia/include/effects/SkImageFilters.h>

#include "console.h"
#include "css.h"
#include "fail.h"
#include "js_strings.h"
#include "thread.h"

namespace {

void CanvasRenderingContext2D(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall()) {
    info.GetIsolate()->ThrowError("CanvasRenderingContext2D is a constructor");
    return;
  }
  JsApi* api = JsApi::Get(info.GetIsolate());
  v8::Local<v8::Object> thiz = info.This();
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    api->js()->ThrowInvalidArgument();
    return;
  }
  int width = info[0].As<v8::Number>()->Value();
  int height = info[1].As<v8::Number>()->Value();
  new CanvasApi(api, thiz, width, height);
}

void CanvasGradient(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall()) {
    info.GetIsolate()->ThrowError("CanvasGradient is a constructor");
    return;
  }
  JsApi* api = JsApi::Get(info.GetIsolate());
  v8::Local<v8::Object> thiz = info.This();
  new CanvasGradientApi(api, thiz, info);
}

void ImageData(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall()) {
    info.GetIsolate()->ThrowError("ImageData is a constructor");
    return;
  }

  JsApi* api = JsApi::Get(info.GetIsolate());
  v8::Local<v8::Object> thiz = info.This();

  if (info.Length() >= 2 && info[0]->IsNumber() && info[1]->IsNumber()) {
    new ImageDataApi(api, thiz, info[0].As<v8::Number>()->Value(),
                     info[1].As<v8::Number>()->Value());
    return;
  }

  if (info.Length() >= 1 &&
      api->IsInstanceOf(info[0], api->GetImageDataConstructor())) {
    ImageDataApi* image_data = ImageDataApi::Get(info[0]);
    if (info.Length() >= 5 && info[1]->IsUint32() && info[2]->IsUint32() &&
        info[3]->IsUint32() && info[4]->IsUint32()) {
      int x = info[1].As<v8::Uint32>()->Value();
      int y = info[2].As<v8::Uint32>()->Value();
      int w = info[3].As<v8::Uint32>()->Value();
      int h = info[4].As<v8::Uint32>()->Value();
      if (x < 0 || y < 0 || w <= 0 || h <= 0 || x + w > image_data->width() ||
          y + h >= image_data->height()) {
        api->js()->ThrowInvalidArgument();
        return;
      }
      new ImageDataApi(api, thiz, image_data, x, y, w, h);
    } else {
      new ImageDataApi(api, thiz, image_data->width(), image_data->height());
    }
    return;
  }

  int length;
  if (info.Length() < 2 || !info[1]->IsUint32()) {
    api->js()->ThrowInvalidArgument();
    return;
  } else if (info[0]->IsUint8Array()) {
    length = info[0].As<v8::Uint8Array>()->Length();
  } else if (info[0]->IsUint8ClampedArray()) {
    length = info[0].As<v8::Uint8ClampedArray>()->Length();
  } else if (info[0]->IsUint16Array()) {
    length = info[0].As<v8::Uint16Array>()->Length();
  } else if (info[0]->IsFloat32Array()) {
    length = info[0].As<v8::Float32Array>()->Length();
  } else {
    api->js()->ThrowInvalidArgument();
    return;
  }

  int width = info[1].As<v8::Uint32>()->Value();
  int height = length / (width * 4);

  if (info.Length() >= 3) {
    if (!info[2]->IsUint32()) {
      api->js()->ThrowInvalidArgument();
      return;
    }
    height = info[2].As<v8::Uint32>()->Value();
  }

  if (width * height * 4 != length) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  if (info[0]->IsUint8Array()) {
    new ImageDataApi(api, thiz, info[0].As<v8::Uint8Array>(), width, height);
  } else if (info[0]->IsUint8ClampedArray()) {
    new ImageDataApi(api, thiz, info[0].As<v8::Uint8ClampedArray>(), width,
                     height);
  } else if (info[0]->IsUint16Array()) {
    new ImageDataApi(api, thiz, info[0].As<v8::Uint16Array>(), width, height);
  } else if (info[0]->IsFloat32Array()) {
    new ImageDataApi(api, thiz, info[0].As<v8::Float32Array>(), width, height);
  } else {
    ASSERT(false);
  }
}

void ImageBitmap(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall()) {
    info.GetIsolate()->ThrowError("ImageBitmap is a constructor");
    return;
  }

  JsApi* api = JsApi::Get(info.GetIsolate());
  v8::Local<v8::Object> thiz = info.This();

  if (info.Length() < 1) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  sk_sp<SkImage> texture;

  if (api->IsInstanceOf(info[0], api->GetImageBitmapConstructor())) {
    texture = ImageBitmapApi::Get(info[0])->texture();
  } else if (api->IsInstanceOf(info[0], api->GetImageDataConstructor())) {
    ImageDataApi* image_data = ImageDataApi::Get(info[0]);
    SkImageInfo image_info =
        SkImageInfo::Make(image_data->width(), image_data->height(),
                          kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
    SkPixmap pixmap{image_info, image_data->backing_store()->Data(),
                    image_data->width() * 4u};
    sk_sp<SkImage> image = SkImage::MakeRasterCopy(pixmap);
    texture =
        image->makeTextureImage(api->canvas_shared_context()->skia_context(),
                                GrMipMapped::kNo, SkBudgeted::kNo);
    ASSERT(texture);
    ASSERT(texture->isTextureBacked());
  } else if (info[0]->IsExternal()) {
    texture.reset(static_cast<SkImage*>(info[0].As<v8::External>()->Value()));
    ASSERT(texture->isTextureBacked());
  } else {
    api->js()->ThrowInvalidArgument();
    return;
  }

  if (info.Length() >= 5 && info[1]->IsUint32() && info[2]->IsUint32() &&
      info[3]->IsInt32() && info[4]->IsInt32()) {
    int x = info[1].As<v8::Number>()->Value();
    int y = info[2].As<v8::Number>()->Value();
    int w = info[3].As<v8::Number>()->Value();
    int h = info[4].As<v8::Number>()->Value();
    if (w < 0) {
      x += w;
      w = -w;
    }
    if (h < 0) {
      y += h;
      h = -h;
    }
    if (x < 0 || x >= texture->width() || y < 0 || y >= texture->height() ||
        x + w > texture->width() || y + h > texture->height()) {
      api->js()->ThrowInvalidArgument();
      return;
    }
    SkIRect rect{x, y, x + w, y + h};
    texture =
        texture->makeSubset(rect, api->canvas_shared_context()->skia_context());
    ASSERT(texture);
    ASSERT(texture->isTextureBacked());
  }

  new ImageBitmapApi(api, thiz, texture);
}

void UnrefData(void* ptr, size_t length, void* data) {
  static_cast<SkData*>(data)->unref();
}

v8::Local<v8::Promise> EncodeInBackground(
    sk_sp<SkImage> image, const v8::FunctionCallbackInfo<v8::Value>& info,
    JsApi* api) {
  SkEncodedImageFormat format = SkEncodedImageFormat::kPNG;
  int quality = 100;
  if (info.Length() >= 1 && info[0]->IsString()) {
    std::string s = api->js()->ToString(info[0]);
    if (s == "jpg" || s == "jpeg") {
      format = SkEncodedImageFormat::kJPEG;
    } else if (s == "webp") {
      format = SkEncodedImageFormat::kWEBP;
    }
    if (info.Length() >= 2 && info[1]->IsUint32()) {
      quality = std::clamp<int>(info[1].As<v8::Uint32>()->Value(), 0, 100);
    }
  }

  return api->PostToBackgroundAndResolve([=]() {
    sk_sp<SkData> data = image->encodeToData(format, quality);
    ASSERT(data);
    data->ref();
    return
        [=](JsApi* api, const JsScope& scope, v8::Promise::Resolver* resolver) {
          std::unique_ptr<v8::BackingStore> store =
              v8::ArrayBuffer::NewBackingStore(
                  (void*) data->data(), data->size(), UnrefData, data.get());
          v8::Local<v8::ArrayBuffer> buffer =
              v8::ArrayBuffer::New(api->isolate(), std::move(store));
          (void) resolver->Resolve(scope.context, buffer);
        };
  });
}

sk_sp<SkData> PrepareToDecode(JsApi* api,
                              const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1 ||
      !(info[0]->IsUint8Array() || info[0]->IsUint8ClampedArray() ||
        info[0]->IsArrayBuffer())) {
    api->js()->ThrowInvalidArgument();
    return {};
  }

  if (info[0]->IsArrayBuffer()) {
    std::shared_ptr<v8::BackingStore> store =
        info[0].As<v8::ArrayBuffer>()->GetBackingStore();
    return SkData::MakeWithCopy(store->Data(), store->ByteLength());
  } else {
    v8::Local<v8::TypedArray> array = info[0].As<v8::TypedArray>();
    sk_sp<SkData> data = SkData::MakeUninitialized(array->ByteLength());
    array->CopyContents(data->writable_data(), data->size());
    return data;
  }
}

}  // namespace

CanvasApi::CanvasApi(JsApi* api, v8::Local<v8::Object> thiz, int width,
                     int height)
    : JsApiWrapper(api->isolate(), thiz),
      canvas_(new RenderCanvas(api->canvas_shared_context(), width, height)) {
  // Each RenderCanvas has two render buffers with 4 bytes per pixel
  // each. Tell v8 about the external size used to inform GC.
  // Note that this class ignores resizes to the RenderCanvas.
  allocated_in_bytes_ = 8 * canvas_->width() * canvas_->height();
  api->isolate()->AdjustAmountOfExternalAllocatedMemory(allocated_in_bytes_);

  state_.fill_paint.setStyle(SkPaint::kFill_Style);
  state_.fill_paint.setAntiAlias(true);
  state_.fill_color = SK_ColorBLACK;
  state_.stroke_paint.setStyle(SkPaint::kStroke_Style);
  state_.stroke_paint.setAntiAlias(true);
  state_.stroke_paint.setStrokeWidth(1.0f);
  state_.stroke_paint.setStrokeCap(SkPaint::kButt_Cap);
  state_.stroke_paint.setStrokeJoin(SkPaint::kMiter_Join);
  state_.stroke_paint.setStrokeMiter(10.0f);
  state_.stroke_color = SK_ColorBLACK;
  state_.global_alpha = 1.0f;
  state_.global_composite_op = SkBlendMode::kSrcOver;

  state_.line_dash_offset = 0.0f;

  state_.text_align = StringId::left;
  state_.text_baseline = StringId::alphabetic;

  state_.shadow_color = SK_ColorTRANSPARENT;
  state_.shadow_offsetx = 0;
  state_.shadow_offsety = 0;
  state_.shadow_blur = 0;

  state_.font.setSize(16.0f);
}

CanvasApi::~CanvasApi() {
  api()->isolate()->AdjustAmountOfExternalAllocatedMemory(-allocated_in_bytes_);
}

// static
v8::Local<v8::Function> CanvasApi::GetConstructor(JsApi* api,
                                                  const JsScope& scope) {
  v8::Local<v8::FunctionTemplate> canvas_rendering_context_2d =
      v8::FunctionTemplate::New(scope.isolate, CanvasRenderingContext2D);
  canvas_rendering_context_2d->SetClassName(
      scope.GetConstantString(StringId::CanvasRenderingContext2D));

  v8::Local<v8::ObjectTemplate> instance =
      canvas_rendering_context_2d->InstanceTemplate();
  // Used in JsApiWrapper to track this.
  instance->SetInternalFieldCount(1);

  v8::Local<v8::ObjectTemplate> prototype =
      canvas_rendering_context_2d->PrototypeTemplate();

  // Properties.
  scope.Set(instance, StringId::width, GetWidth, SetWidth);
  scope.Set(instance, StringId::height, GetHeight, SetHeight);
  scope.Set(instance, StringId::fillStyle, GetFillStyle, SetFillStyle);
  scope.Set(instance, StringId::strokeStyle, GetStrokeStyle, SetStrokeStyle);
  scope.Set(instance, StringId::font, GetFont, SetFont);
  scope.Set(instance, StringId::lineWidth, GetLineWidth, SetLineWidth);
  scope.Set(instance, StringId::lineCap, GetLineCap, SetLineCap);
  scope.Set(instance, StringId::lineJoin, GetLineJoin, SetLineJoin);
  scope.Set(instance, StringId::miterLimit, GetMiterLimit, SetMiterLimit);
  scope.Set(instance, StringId::lineDashOffset, GetLineDashOffset,
            SetLineDashOffset);
  scope.Set(instance, StringId::textAlign, GetTextAlign, SetTextAlign);
  scope.Set(instance, StringId::textBaseline, GetTextBaseline, SetTextBaseline);
  scope.Set(instance, StringId::globalAlpha, GetGlobalAlpha, SetGlobalAlpha);
  scope.Set(instance, StringId::globalCompositeOperation,
            GetGlobalCompositeOperation, SetGlobalCompositeOperation);
  scope.Set(instance, StringId::shadowBlur, GetShadowBlur, SetShadowBlur);
  scope.Set(instance, StringId::shadowColor, GetShadowColor, SetShadowColor);
  scope.Set(instance, StringId::shadowOffsetX, GetShadowOffsetX,
            SetShadowOffsetX);
  scope.Set(instance, StringId::shadowOffsetY, GetShadowOffsetY,
            SetShadowOffsetY);
  scope.Set(instance, StringId::antialias, GetAntiAlias, SetAntiAlias);
  scope.Set(instance, StringId::imageSmoothingEnabled, GetImageSmoothingEnabled,
            SetImageSmoothingEnabled);
  scope.Set(instance, StringId::imageSmoothingQuality, GetImageSmoothingQuality,
            SetImageSmoothingQuality);

  // Functions.
  scope.Set(prototype, StringId::clearRect, ClearRect);
  scope.Set(prototype, StringId::fillRect, FillRect);
  scope.Set(prototype, StringId::strokeRect, StrokeRect);
  scope.Set(prototype, StringId::fillText, FillText);
  scope.Set(prototype, StringId::strokeText, StrokeText);
  scope.Set(prototype, StringId::measureText, MeasureText);
  scope.Set(prototype, StringId::getLineDash, GetLineDash);
  scope.Set(prototype, StringId::setLineDash, SetLineDash);
  scope.Set(prototype, StringId::beginPath, BeginPath);
  scope.Set(prototype, StringId::closePath, ClosePath);
  scope.Set(prototype, StringId::moveTo, MoveTo);
  scope.Set(prototype, StringId::lineTo, LineTo);
  scope.Set(prototype, StringId::bezierCurveTo, BezierCurveTo);
  scope.Set(prototype, StringId::quadraticCurveTo, QuadraticCurveTo);
  scope.Set(prototype, StringId::arc, Arc);
  scope.Set(prototype, StringId::arcTo, ArcTo);
  scope.Set(prototype, StringId::ellipse, Ellipse);
  scope.Set(prototype, StringId::rect, Rect);
  scope.Set(prototype, StringId::fill, Fill);
  scope.Set(prototype, StringId::stroke, Stroke);
  scope.Set(prototype, StringId::clip, Clip);
  scope.Set(prototype, StringId::isPointInPath, IsPointInPath);
  scope.Set(prototype, StringId::isPointInStroke, IsPointInStroke);
  scope.Set(prototype, StringId::rotate, Rotate);
  scope.Set(prototype, StringId::scale, Scale);
  scope.Set(prototype, StringId::translate, Translate);
  scope.Set(prototype, StringId::transform, Transform);
  scope.Set(prototype, StringId::getTransform, GetTransform);
  scope.Set(prototype, StringId::setTransform, SetTransform);
  scope.Set(prototype, StringId::resetTransform, ResetTransform);
  scope.Set(prototype, StringId::save, Save);
  scope.Set(prototype, StringId::restore, Restore);
  scope.Set(prototype, StringId::createLinearGradient, CreateLinearGradient);
  scope.Set(prototype, StringId::createRadialGradient, CreateRadialGradient);
  scope.Set(prototype, StringId::createImageData, CreateImageData);
  scope.Set(prototype, StringId::getImageData, GetImageData);
  scope.Set(prototype, StringId::putImageData, PutImageData);
  scope.Set(prototype, StringId::encode, Encode);
  scope.Set(prototype, StringId::drawImage, DrawImage);

  return canvas_rendering_context_2d->GetFunction(scope.context)
      .ToLocalChecked();
}

// static
void CanvasApi::GetWidth(v8::Local<v8::String> property,
                         const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->canvas()->width());
}

// static
void CanvasApi::SetWidth(v8::Local<v8::String> property,
                         v8::Local<v8::Value> value,
                         const v8::PropertyCallbackInfo<void>& info) {
  int width = 0;
  if (value->IsNumber()) {
    width = (int) value.As<v8::Number>()->Value();
    if (width < 0) {
      width = 0;
    }
  }
  JsApi* api = JsApi::Get(info.GetIsolate());
  CanvasApi* canvas_api = CanvasApi::Get(info.This());
  if (canvas_api->canvas() == api->window_canvas()) {
    api->js()->ThrowError("Use window.width to resize the main canvas");
    return;
  }
  canvas_api->canvas()->Resize(width, canvas_api->canvas()->height());
}

// static
void CanvasApi::GetHeight(v8::Local<v8::String> property,
                          const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->canvas()->height());
}

// static
void CanvasApi::SetHeight(v8::Local<v8::String> property,
                          v8::Local<v8::Value> value,
                          const v8::PropertyCallbackInfo<void>& info) {
  int height = 0;
  if (value->IsNumber()) {
    height = (int) value.As<v8::Number>()->Value();
    if (height < 0) {
      height = 0;
    }
  }
  JsApi* api = JsApi::Get(info.GetIsolate());
  CanvasApi* canvas_api = CanvasApi::Get(info.This());
  if (canvas_api->canvas() == api->window_canvas()) {
    api->js()->ThrowError("Use window.height to resize the main canvas");
    return;
  }
  canvas_api->canvas()->Resize(canvas_api->canvas()->width(), height);
}

// static
void CanvasApi::GetFillStyle(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string color = SkColorToCSSColor(api->state_.fill_color);
  info.GetReturnValue().Set(api->js()->MakeString(color));
}

// static
void CanvasApi::SetFillStyle(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  if (value->IsString()) {
    std::string s = api->js()->ToString(value);
    SkColor color;
    if (CSSColorToSkColor(s, &color)) {
      State& state = api->state_;
      state.fill_color = color;
      state.fill_paint.setColor(color);
      if (state.global_alpha < 1.0f) {
        state.fill_paint.setAlphaf(state.fill_paint.getAlphaf() *
                                   state.global_alpha);
      }
    }
  } else if (api->api()->IsInstanceOf(
                 value, api->api()->GetCanvasGradientConstructor())) {
    CanvasGradientApi* gradient = JsApiWrapper::Get<CanvasGradientApi>(value);
    gradient->Ref();
    State& state = api->state_;
    if (state.fill_gradient) {
      state.fill_gradient->Unref();
    }
    state.fill_gradient = gradient;
    state.fill_paint.setShader(gradient->GetShader());
  }
}

// static
void CanvasApi::GetStrokeStyle(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string color = SkColorToCSSColor(api->state_.stroke_color);
  info.GetReturnValue().Set(api->js()->MakeString(color));
}

// static
void CanvasApi::SetStrokeStyle(v8::Local<v8::String> property,
                               v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  if (value->IsString()) {
    std::string s = api->js()->ToString(value);
    SkColor color;
    if (CSSColorToSkColor(s, &color)) {
      State& state = api->state_;
      state.stroke_color = color;
      state.stroke_paint.setColor(color);
      if (state.global_alpha < 1.0f) {
        state.stroke_paint.setAlphaf(state.stroke_paint.getAlphaf() *
                                     state.global_alpha);
      }
    }
  } else if (api->api()->IsInstanceOf(
                 value, api->api()->GetCanvasGradientConstructor())) {
    CanvasGradientApi* gradient = JsApiWrapper::Get<CanvasGradientApi>(value);
    gradient->Ref();
    State& state = api->state_;
    if (state.stroke_gradient) {
      state.stroke_gradient->Unref();
    }
    state.stroke_gradient = gradient;
    state.stroke_paint.setShader(gradient->GetShader());
  }
}

static void DrawRect(const v8::FunctionCallbackInfo<v8::Value>& info,
                     const SkPaint& paint, CanvasApi* api) {
  if (info.Length() < 4 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber()) {
    return;
  }
  SkRect rect = SkRect::MakeXYWH(
      info[0].As<v8::Number>()->Value(), info[1].As<v8::Number>()->Value(),
      info[2].As<v8::Number>()->Value(), info[3].As<v8::Number>()->Value());
  api->canvas()->SetCurrentContext();
  api->skia_canvas()->drawRect(rect, paint);
}

// static
void CanvasApi::ClearRect(const v8::FunctionCallbackInfo<v8::Value>& info) {
  SkPaint paint;
  paint.setColor(SK_ColorTRANSPARENT);
  paint.setBlendMode(SkBlendMode::kSrc);
  CanvasApi* api = CanvasApi::Get(info.This());
  DrawRect(info, paint, api);
}

// static
void CanvasApi::FillRect(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  DrawRect(info, api->state_.fill_paint, api);
}

// static
void CanvasApi::StrokeRect(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  DrawRect(info, api->state_.stroke_paint, api);
}

// static
void CanvasApi::GetFont(v8::Local<v8::String> property,
                        const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string font = SkFontToCSSFont(api->state_.font);
  info.GetReturnValue().Set(api->js()->MakeString(font));
}

// static
void CanvasApi::SetFont(v8::Local<v8::String> property,
                        v8::Local<v8::Value> value,
                        const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string font = api->js()->ToString(value);
  CSSFontToSkFont(font, &api->state_.font, api->api()->fonts(),
                  api->api()->fonts_cache());
}

// static
void CanvasApi::GetLineWidth(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.stroke_paint.getStrokeWidth());
}

// static
void CanvasApi::SetLineWidth(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsNumber()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  api->state_.stroke_paint.setStrokeWidth(value.As<v8::Number>()->Value());
}

// static
void CanvasApi::GetLineCap(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  switch (api->state_.stroke_paint.getStrokeCap()) {
    case SkPaint::kSquare_Cap:
      info.GetReturnValue().Set(api->GetConstantString(StringId::square));
      break;
    case SkPaint::kRound_Cap:
      info.GetReturnValue().Set(api->GetConstantString(StringId::round));
      break;
    case SkPaint::kButt_Cap:
    default:
      info.GetReturnValue().Set(api->GetConstantString(StringId::butt));
      break;
  }
}

// static
void CanvasApi::SetLineCap(v8::Local<v8::String> property,
                           v8::Local<v8::Value> value,
                           const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string cap = api->js()->ToString(value);
  if (cap == "butt") {
    api->state_.stroke_paint.setStrokeCap(SkPaint::kButt_Cap);
  } else if (cap == "round") {
    api->state_.stroke_paint.setStrokeCap(SkPaint::kRound_Cap);
  } else if (cap == "square") {
    api->state_.stroke_paint.setStrokeCap(SkPaint::kSquare_Cap);
  }
}

// static
void CanvasApi::GetLineJoin(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  switch (api->state_.stroke_paint.getStrokeJoin()) {
    case SkPaint::kRound_Join:
      info.GetReturnValue().Set(api->GetConstantString(StringId::round));
      break;
    case SkPaint::kBevel_Join:
      info.GetReturnValue().Set(api->GetConstantString(StringId::bevel));
      break;
    case SkPaint::kMiter_Join:
    default:
      info.GetReturnValue().Set(api->GetConstantString(StringId::miter));
      break;
  }
}

// static
void CanvasApi::SetLineJoin(v8::Local<v8::String> property,
                            v8::Local<v8::Value> value,
                            const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string join = api->js()->ToString(value);
  if (join == "miter") {
    api->state_.stroke_paint.setStrokeJoin(SkPaint::kMiter_Join);
  } else if (join == "round") {
    api->state_.stroke_paint.setStrokeJoin(SkPaint::kRound_Join);
  } else if (join == "bevel") {
    api->state_.stroke_paint.setStrokeJoin(SkPaint::kBevel_Join);
  }
}

// static
void CanvasApi::GetMiterLimit(v8::Local<v8::String> property,
                              const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.stroke_paint.getStrokeMiter());
}

// static
void CanvasApi::SetMiterLimit(v8::Local<v8::String> property,
                              v8::Local<v8::Value> value,
                              const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsNumber()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  api->state_.stroke_paint.setStrokeMiter(value.As<v8::Number>()->Value());
}

// static
void CanvasApi::GetLineDash(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  v8::Isolate* isolate = info.GetIsolate();
  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Array> array =
      v8::Array::New(isolate, api->state_.line_dash.size());
  for (int i = 0; i < api->state_.line_dash.size(); i++) {
    v8::Local<v8::Value> x = v8::Number::New(isolate, api->state_.line_dash[i]);
    ASSERT(array->Set(context, i, x).FromJust());
  }
  info.GetReturnValue().Set(array);
}

// static
void CanvasApi::SetLineDash(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1 || !info[0]->IsArray()) {
    return;
  }
  v8::Local<v8::Array> array = info[0].As<v8::Array>();
  int length = array->Length();

  std::vector<float> line_dash;
  line_dash.resize(length);

  CanvasApi* api = CanvasApi::Get(info.This());
  v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();

  for (int i = 0; i < length; i++) {
    v8::Local<v8::Value> value = array->Get(context, i).ToLocalChecked();
    if (!value->IsNumber()) {
      return;
    }
    line_dash[i] = value.As<v8::Number>()->Value();
  }

  if (length % 2 == 1) {
    // Append a copy, as per spec.
    line_dash.resize(2 * length);
    for (int i = 0; i < length; i++) {
      line_dash[length + i] = line_dash[i];
    }
  }

  api->state_.line_dash.swap(line_dash);
  api->UpdateLineDash();
}

void CanvasApi::UpdateLineDash() {
  if (state_.line_dash.empty()) {
    state_.stroke_paint.setPathEffect(nullptr);
  } else {
    state_.stroke_paint.setPathEffect(
        SkDashPathEffect::Make(state_.line_dash.data(), state_.line_dash.size(),
                               state_.line_dash_offset));
  }
}

// static
void CanvasApi::GetLineDashOffset(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.line_dash_offset);
}

// static
void CanvasApi::SetLineDashOffset(v8::Local<v8::String> property,
                                  v8::Local<v8::Value> value,
                                  const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsNumber()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  api->state_.line_dash_offset = value.As<v8::Number>()->Value();
  api->UpdateLineDash();
}

// static
void CanvasApi::GetTextAlign(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->GetConstantString(api->state_.text_align));
}

// static
void CanvasApi::SetTextAlign(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string align = api->js()->ToString(value);
  if (align == "left") {
    api->state_.text_align = StringId::left;
  } else if (align == "right") {
    api->state_.text_align = StringId::right;
  } else if (align == "center") {
    api->state_.text_align = StringId::center;
  } else if (align == "start") {
    api->state_.text_align = StringId::start;
  } else if (align == "end") {
    api->state_.text_align = StringId::end;
  }
}

// static
void CanvasApi::GetTextBaseline(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->GetConstantString(api->state_.text_baseline));
}

// static
void CanvasApi::SetTextBaseline(v8::Local<v8::String> property,
                                v8::Local<v8::Value> value,
                                const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string baseline = api->js()->ToString(value);
  if (baseline == "top") {
    api->state_.text_baseline = StringId::top;
  } else if (baseline == "hanging") {
    api->state_.text_baseline = StringId::hanging;
  } else if (baseline == "middle") {
    api->state_.text_baseline = StringId::middle;
  } else if (baseline == "alphabetic") {
    api->state_.text_baseline = StringId::alphabetic;
  } else if (baseline == "ideographic") {
    api->state_.text_baseline = StringId::ideographic;
  } else if (baseline == "bottom") {
    api->state_.text_baseline = StringId::bottom;
  }
}

// static
void CanvasApi::GetGlobalAlpha(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.global_alpha);
}

// static
void CanvasApi::SetGlobalAlpha(v8::Local<v8::String> property,
                               v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsNumber()) {
    return;
  }
  float alpha = value.As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  State& state = api->state_;
  if (alpha != state.global_alpha) {
    state.global_alpha = alpha;
    state.fill_paint.setColor(state.fill_color);
    state.fill_paint.setAlphaf(state.fill_paint.getAlphaf() * alpha);
    state.stroke_paint.setColor(state.stroke_color);
    state.stroke_paint.setAlphaf(state.stroke_paint.getAlphaf() * alpha);
  }
}

// static
void CanvasApi::GetGlobalCompositeOperation(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  StringId op = StringId::sourceOver;
  switch (api->state_.global_composite_op) {
    case SkBlendMode::kClear:
      // Not supported.
      op = StringId::sourceOver;
      break;
    case SkBlendMode::kSrc: op = StringId::copy; break;
    case SkBlendMode::kDst:
      // Not supported.
      op = StringId::sourceOver;
      break;
    case SkBlendMode::kSrcOver: op = StringId::sourceOver; break;
    case SkBlendMode::kDstOver: op = StringId::destinationOver; break;
    case SkBlendMode::kSrcIn: op = StringId::sourceIn; break;
    case SkBlendMode::kDstIn: op = StringId::destinationIn; break;
    case SkBlendMode::kSrcOut: op = StringId::sourceOut; break;
    case SkBlendMode::kDstOut: op = StringId::destinationOut; break;
    case SkBlendMode::kSrcATop: op = StringId::sourceAtop; break;
    case SkBlendMode::kDstATop: op = StringId::destinationAtop; break;
    case SkBlendMode::kXor: op = StringId::_XOR_; break;
    case SkBlendMode::kPlus: op = StringId::lighter; break;
    case SkBlendMode::kModulate:
      // Not supported;
      op = StringId::sourceOver;
      break;
    case SkBlendMode::kScreen: op = StringId::screen; break;
    case SkBlendMode::kOverlay: op = StringId::overlay; break;
    case SkBlendMode::kDarken: op = StringId::darken; break;
    case SkBlendMode::kLighten: op = StringId::lighten; break;
    case SkBlendMode::kColorDodge: op = StringId::colorDodge; break;
    case SkBlendMode::kColorBurn: op = StringId::colorBurn; break;
    case SkBlendMode::kHardLight: op = StringId::hardLight; break;
    case SkBlendMode::kSoftLight: op = StringId::softLight; break;
    case SkBlendMode::kDifference: op = StringId::difference; break;
    case SkBlendMode::kExclusion: op = StringId::exclusion; break;
    case SkBlendMode::kMultiply: op = StringId::multiply; break;
    case SkBlendMode::kHue: op = StringId::hue; break;
    case SkBlendMode::kSaturation: op = StringId::saturation; break;
    case SkBlendMode::kColor: op = StringId::color; break;
    case SkBlendMode::kLuminosity: op = StringId::luminosity; break;
  }
  info.GetReturnValue().Set(api->GetConstantString(op));
}

// static
void CanvasApi::SetGlobalCompositeOperation(
    v8::Local<v8::String> property, v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string s = api->js()->ToString(value);
  static const std::unordered_map<std::string, SkBlendMode> ops{
      {"source-over", SkBlendMode::kSrcOver},
      {"source-in", SkBlendMode::kSrcIn},
      {"source-out", SkBlendMode::kSrcOut},
      {"source-atop", SkBlendMode::kSrcATop},
      {"destination-over", SkBlendMode::kDstOver},
      {"destination-in", SkBlendMode::kDstIn},
      {"destination-out", SkBlendMode::kDstOut},
      {"destination-atop", SkBlendMode::kDstATop},
      {"lighter", SkBlendMode::kPlus},
      {"copy", SkBlendMode::kSrc},
      {"xor", SkBlendMode::kXor},
      {"multiply", SkBlendMode::kMultiply},
      {"screen", SkBlendMode::kScreen},
      {"overlay", SkBlendMode::kOverlay},
      {"darken", SkBlendMode::kDarken},
      {"lighten", SkBlendMode::kLighten},
      {"color-dodge", SkBlendMode::kColorDodge},
      {"color-burn", SkBlendMode::kColorBurn},
      {"hard-light", SkBlendMode::kHardLight},
      {"soft-light", SkBlendMode::kSoftLight},
      {"difference", SkBlendMode::kDifference},
      {"exclusion", SkBlendMode::kExclusion},
      {"hue", SkBlendMode::kHue},
      {"saturation", SkBlendMode::kSaturation},
      {"color", SkBlendMode::kColor},
      {"luminosity", SkBlendMode::kLuminosity},
  };
  auto it = ops.find(s);
  if (it != ops.end()) {
    SkBlendMode op = it->second;
    if (op != api->state_.global_composite_op) {
      api->state_.global_composite_op = op;
      api->state_.fill_paint.setBlendMode(op);
      api->state_.stroke_paint.setBlendMode(op);
    }
  }
}

// static
void CanvasApi::GetShadowBlur(v8::Local<v8::String> property,
                              const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.shadow_blur);
}

// static
void CanvasApi::SetShadowBlur(v8::Local<v8::String> property,
                              v8::Local<v8::Value> value,
                              const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsNumber()) {
    return;
  }
  float blur = value.As<v8::Number>()->Value();
  if (blur < 0) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  State& state = api->state_;
  if (blur != state.shadow_blur) {
    state.shadow_blur = blur;
    api->UpdateShadow();
  }
}

// static
void CanvasApi::GetShadowColor(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string color = SkColorToCSSColor(api->state_.shadow_color);
  info.GetReturnValue().Set(api->js()->MakeString(color));
}

// static
void CanvasApi::SetShadowColor(v8::Local<v8::String> property,
                               v8::Local<v8::Value> value,
                               const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string css = api->js()->ToString(value);
  SkColor color;
  if (!CSSColorToSkColor(css, &color)) {
    return;
  }
  State& state = api->state_;
  if (color != state.shadow_color) {
    state.shadow_color = color;
    api->UpdateShadow();
  }
}

// static
void CanvasApi::GetShadowOffsetX(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.shadow_offsetx);
}

// static
void CanvasApi::SetShadowOffsetX(v8::Local<v8::String> property,
                                 v8::Local<v8::Value> value,
                                 const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsNumber()) {
    return;
  }
  float offset = value.As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  State& state = api->state_;
  if (offset != state.shadow_offsetx) {
    state.shadow_offsetx = offset;
    api->UpdateShadow();
  }
}

// static
void CanvasApi::GetShadowOffsetY(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.shadow_offsety);
}

// static
void CanvasApi::SetShadowOffsetY(v8::Local<v8::String> property,
                                 v8::Local<v8::Value> value,
                                 const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsNumber()) {
    return;
  }
  float offset = value.As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  State& state = api->state_;
  if (offset != state.shadow_offsety) {
    state.shadow_offsety = offset;
    api->UpdateShadow();
  }
}

// static
void CanvasApi::GetAntiAlias(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.antialias);
}

// static
void CanvasApi::SetAntiAlias(v8::Local<v8::String> property,
                             v8::Local<v8::Value> value,
                             const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsBoolean()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  api->state_.antialias = value.As<v8::Boolean>()->Value();
  api->state_.fill_paint.setAntiAlias(api->state_.antialias);
  api->state_.stroke_paint.setAntiAlias(api->state_.antialias);
}

// static
void CanvasApi::GetImageSmoothingEnabled(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  info.GetReturnValue().Set(api->state_.image_smoothing);
}

// static
void CanvasApi::SetImageSmoothingEnabled(
    v8::Local<v8::String> property, v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsBoolean()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  api->state_.image_smoothing = value.As<v8::Boolean>()->Value();
  api->UpdateImageSmoothing();
}

// static
void CanvasApi::GetImageSmoothingQuality(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  v8::Local<v8::String> quality;
  if (api->state_.image_smoothing_quality == 0) {
    quality = api->js()->MakeString("low");
  } else if (api->state_.image_smoothing_quality == 1) {
    quality = api->js()->MakeString("medium");
  } else {
    quality = api->js()->MakeString("high");
  }
  info.GetReturnValue().Set(quality);
}

// static
void CanvasApi::SetImageSmoothingQuality(
    v8::Local<v8::String> property, v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<void>& info) {
  if (!value->IsString()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  std::string quality = api->js()->ToString(value);
  if (quality == "low") {
    api->state_.image_smoothing_quality = 0;
  } else if (quality == "medium") {
    api->state_.image_smoothing_quality = 1;
  } else if (quality == "high") {
    api->state_.image_smoothing_quality = 2;
  } else {
    return;
  }
  api->UpdateImageSmoothing();
}

void CanvasApi::UpdateImageSmoothing() {
  if (!state_.image_smoothing) {
    state_.sampling_options = SkSamplingOptions{};
  } else if (state_.image_smoothing_quality == 0) {
    state_.sampling_options = SkSamplingOptions{SkFilterMode::kLinear};
  } else if (state_.image_smoothing_quality == 1) {
    state_.sampling_options =
        SkSamplingOptions{SkFilterMode::kLinear, SkMipmapMode::kLinear};
  } else {
    state_.sampling_options = SkSamplingOptions(SkCubicResampler::Mitchell());
  }
}

void CanvasApi::UpdateShadow() {
  sk_sp<SkImageFilter> filter;

  if (state_.shadow_color != SK_ColorTRANSPARENT &&
      (state_.shadow_blur > 0 || state_.shadow_offsetx != 0 ||
       state_.shadow_offsety != 0)) {
    // Sigma is half of the shadowBlur value, according to the spec:
    // https://html.spec.whatwg.org/multipage/canvas.html#shadows
    float blur = state_.shadow_blur / 2;
    filter =
        SkImageFilters::DropShadow(state_.shadow_offsetx, state_.shadow_offsety,
                                   blur, blur, state_.shadow_color, nullptr);
  }

  state_.fill_paint.setImageFilter(filter);
  state_.stroke_paint.setImageFilter(filter);
}

// static
void CanvasApi::DrawText(const v8::FunctionCallbackInfo<v8::Value>& info,
                         const SkPaint& paint, CanvasApi* api) {
  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsNumber() ||
      !info[2]->IsNumber()) {
    return;
  }

  std::string text = api->js()->ToString(info[0]);
  double x = info[1].As<v8::Number>()->Value();
  double y = info[2].As<v8::Number>()->Value();

  State& state = api->state_;
  if (state.text_align == StringId::center ||
      state.text_align == StringId::right ||
      state.text_align == StringId::end) {
    SkRect bounds = SkRect::MakeEmpty();
    float advance = state.font.measureText(
        text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds, &paint);
    if (state.text_align == StringId::center) {
      x -= advance / 2;
    } else {
      x -= advance;
    }
  }

  if (state.text_baseline != StringId::alphabetic) {
    SkFontMetrics metrics;
    state.font.getMetrics(&metrics);
    if (state.text_baseline == StringId::top ||
        state.text_baseline == StringId::hanging) {
      y += fabs(metrics.fCapHeight);
    } else if (state.text_baseline == StringId::middle) {
      y += fabs(metrics.fXHeight) / 2;
    } else if (state.text_baseline == StringId::ideographic ||
               state.text_baseline == StringId::bottom) {
      y -= fabs(metrics.fDescent);
    }
  }

  api->canvas()->SetCurrentContext();
  api->skia_canvas()->drawSimpleText(text.c_str(), text.size(),
                                     SkTextEncoding::kUTF8, x, y, state.font,
                                     paint);
}

// static
void CanvasApi::FillText(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  DrawText(info, api->state_.fill_paint, api);
}

// static
void CanvasApi::StrokeText(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  DrawText(info, api->state_.stroke_paint, api);
}

// static
void CanvasApi::MeasureText(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1 || !info[0]->IsString()) {
    return;
  }

  CanvasApi* api = CanvasApi::Get(info.This());
  std::string text = api->js()->ToString(info[0]);

  SkRect bounds = SkRect::MakeEmpty();
  float advance = api->state_.font.measureText(text.c_str(), text.size(),
                                               SkTextEncoding::kUTF8, &bounds,
                                               &api->state_.fill_paint);

  JsScope scope(api->js());

  v8::Local<v8::Object> metrics = v8::Object::New(scope.isolate);
  scope.Set(metrics, StringId::width, advance);
  scope.Set(metrics, StringId::actualBoundingBoxAscent, bounds.top());
  scope.Set(metrics, StringId::actualBoundingBoxDescent, bounds.bottom());
  scope.Set(metrics, StringId::actualBoundingBoxLeft, bounds.left());
  scope.Set(metrics, StringId::actualBoundingBoxRight, bounds.right());

  info.GetReturnValue().Set(metrics);
}

// static
void CanvasApi::BeginPath(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.rewind();
}

// static
void CanvasApi::ClosePath(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.close();
}

// static
void CanvasApi::MoveTo(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return;
  }
  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.moveTo(x, y);
}

// static
void CanvasApi::LineTo(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return;
  }
  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  if (api->path_.isEmpty() || api->path_.isLastContourClosed()) {
    api->path_.moveTo(x, y);
  } else {
    api->path_.lineTo(x, y);
  }
}

// static
void CanvasApi::BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 6 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber() || !info[4]->IsNumber() ||
      !info[5]->IsNumber()) {
    return;
  }
  float c1x = info[0].As<v8::Number>()->Value();
  float c1y = info[1].As<v8::Number>()->Value();
  float c2x = info[2].As<v8::Number>()->Value();
  float c2y = info[3].As<v8::Number>()->Value();
  float ex = info[4].As<v8::Number>()->Value();
  float ey = info[5].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.cubicTo(c1x, c1y, c2x, c2y, ex, ey);
}

// static
void CanvasApi::QuadraticCurveTo(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 4 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber()) {
    return;
  }
  float cx = info[0].As<v8::Number>()->Value();
  float cy = info[1].As<v8::Number>()->Value();
  float ex = info[2].As<v8::Number>()->Value();
  float ey = info[3].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.quadTo(cx, cy, ex, ey);
}

constexpr double PI = 3.14159265358979323846;

static float RadiansToDegrees(float rad) {
  return (rad / PI) * 180.0f;
}

static float AdjustEndAngle(float start, float end, bool ccw) {
  if (!ccw && end - start >= 2 * PI) {
    return start + 2 * PI;
  } else if (ccw && start - end >= 2 * PI) {
    return start - 2 * PI;
  } else if (!ccw && start > end) {
    return start + (2 * PI - fmodf(start - end, 2 * PI));
  } else if (ccw && start < end) {
    return start - (2 * PI - fmodf(end - start, 2 * PI));
  } else {
    return end;
  }
}

// static
void CanvasApi::Arc(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 5 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber() || !info[4]->IsNumber()) {
    return;
  }
  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  float r = info[2].As<v8::Number>()->Value();
  float start = info[3].As<v8::Number>()->Value();
  float end = info[4].As<v8::Number>()->Value();
  bool ccw = info.Length() >= 6 && info[5]->IsBoolean() &&
             info[5].As<v8::Boolean>()->Value();
  end = AdjustEndAngle(start, end, ccw);
  float sweep = end - start;
  SkRect oval = SkRect::MakeLTRB(x - r, y - r, x + r, y + r);
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.addArc(oval, RadiansToDegrees(start), RadiansToDegrees(sweep));
}

// static
void CanvasApi::ArcTo(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 5 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber() || !info[4]->IsNumber()) {
    return;
  }
  float x1 = info[0].As<v8::Number>()->Value();
  float y1 = info[1].As<v8::Number>()->Value();
  float x2 = info[2].As<v8::Number>()->Value();
  float y2 = info[3].As<v8::Number>()->Value();
  float r = info[4].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.arcTo(x1, y1, x2, y2, r);
}

// static
void CanvasApi::Ellipse(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 7 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber() || !info[4]->IsNumber() ||
      !info[5]->IsNumber() || !info[6]->IsNumber()) {
    return;
  }
  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  float rx = info[2].As<v8::Number>()->Value();
  float ry = info[3].As<v8::Number>()->Value();
  float rotation = info[4].As<v8::Number>()->Value();
  float start = info[5].As<v8::Number>()->Value();
  float end = info[6].As<v8::Number>()->Value();
  bool ccw = info.Length() >= 8 && info[7]->IsBoolean() &&
             info[7].As<v8::Boolean>()->Value();
  end = AdjustEndAngle(start, end, ccw);

  CanvasApi* api = CanvasApi::Get(info.This());

  SkMatrix transform;
  transform.postRotate(RadiansToDegrees(rotation));
  transform.postTranslate(x, y);
  api->path_.transform(transform);

  float sweep = end - start;
  api->path_.addArc(SkRect::MakeLTRB(-rx, -ry, rx, ry), RadiansToDegrees(start),
                    RadiansToDegrees(sweep));

  SkMatrix inverse;
  ASSERT(transform.invert(&inverse));
  api->path_.transform(transform);
}

// static
void CanvasApi::Rect(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 4 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber()) {
    return;
  }
  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  float w = info[2].As<v8::Number>()->Value();
  float h = info[3].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  api->path_.addRect(SkRect::MakeXYWH(x, y, w, h), SkPathDirection::kCW, 0);
}

// static
void CanvasApi::Fill(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  if (info.Length() >= 1 && info[0]->IsString()) {
    std::string fill_type = api->js()->ToString(info[0]);
    if (fill_type == "evenodd") {
      api->path_.setFillType(SkPathFillType::kEvenOdd);
    } else {
      api->path_.setFillType(SkPathFillType::kWinding);
    }
  }
  api->canvas()->SetCurrentContext();
  api->skia_canvas()->drawPath(api->path_, api->state_.fill_paint);

  api->path_.setFillType(SkPathFillType::kWinding);
}

// static
void CanvasApi::Stroke(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  api->canvas()->SetCurrentContext();
  api->skia_canvas()->drawPath(api->path_, api->state_.stroke_paint);
}

// static
void CanvasApi::Clip(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());

  SkPathFillType fill_type = SkPathFillType::kWinding;
  if (info.Length() >= 1 && info[0]->IsString()) {
    std::string fill_rule = api->js()->ToString(info[0]);
    if (fill_rule == "evenodd") {
      fill_type = SkPathFillType::kEvenOdd;
    }
  }

  SkPathFillType before = api->path_.getFillType();
  api->path_.setFillType(fill_type);

  api->skia_canvas()->clipPath(api->path_);

  api->path_.setFillType(before);
}

// static
void CanvasApi::IsPointInPath(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return;
  }

  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  SkPathFillType fill_type = SkPathFillType::kWinding;

  CanvasApi* api = CanvasApi::Get(info.This());
  if (info.Length() >= 3 && info[2]->IsString()) {
    std::string rule = api->js()->ToString(info[2]);
    if (rule == "evenodd") {
      fill_type = SkPathFillType::kEvenOdd;
    }
  }

  SkPathFillType before = api->path_.getFillType();
  api->path_.setFillType(fill_type);
  bool result = api->path_.contains(x, y);
  api->path_.setFillType(before);

  info.GetReturnValue().Set(result);
}

// static
void CanvasApi::IsPointInStroke(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return;
  }

  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();

  CanvasApi* api = CanvasApi::Get(info.This());
  SkPath stroke;
  api->state_.stroke_paint.getFillPath(api->path_, &stroke);
  bool result = stroke.contains(x, y);

  info.GetReturnValue().Set(result);
}

// static
void CanvasApi::Rotate(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    return;
  }
  CanvasApi* api = CanvasApi::Get(info.This());
  float radians = info[0].As<v8::Number>()->Value();
  float degrees = RadiansToDegrees(radians);
  api->skia_canvas()->rotate(degrees);
}

// static
void CanvasApi::Scale(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return;
  }
  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  api->skia_canvas()->scale(x, y);
}

// static
void CanvasApi::Translate(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    return;
  }
  float x = info[0].As<v8::Number>()->Value();
  float y = info[1].As<v8::Number>()->Value();
  CanvasApi* api = CanvasApi::Get(info.This());
  api->skia_canvas()->translate(x, y);
}

// static
void CanvasApi::Transform(const v8::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 6 || !info[0]->IsNumber() || !info[1]->IsNumber() ||
      !info[2]->IsNumber() || !info[3]->IsNumber() || !info[4]->IsNumber() ||
      !info[5]->IsNumber()) {
    return;
  }
  float a = info[0].As<v8::Number>()->Value();
  float b = info[1].As<v8::Number>()->Value();
  float c = info[2].As<v8::Number>()->Value();
  float d = info[3].As<v8::Number>()->Value();
  float e = info[4].As<v8::Number>()->Value();
  float f = info[5].As<v8::Number>()->Value();
  SkMatrix m = SkMatrix::MakeAll(a, c, e, b, d, f, 0, 0, 1);
  CanvasApi* api = CanvasApi::Get(info.This());
  api->skia_canvas()->concat(m);
}

// static
void CanvasApi::GetTransform(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  SkMatrix m = api->skia_canvas()->getLocalToDeviceAs3x3();

  v8::Isolate* isolate = info.GetIsolate();

  v8::Local<v8::Value> v[6];
  v[0] = v8::Number::New(isolate, m.getScaleX());
  v[1] = v8::Number::New(isolate, m.getSkewY());
  v[2] = v8::Number::New(isolate, m.getSkewX());
  v[3] = v8::Number::New(isolate, m.getScaleY());
  v[4] = v8::Number::New(isolate, m.getTranslateX());
  v[5] = v8::Number::New(isolate, m.getTranslateY());

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  v8::Local<v8::Array> array = v8::Array::New(isolate, 6);
  for (int i = 0; i < 6; i++) {
    ASSERT(array->Set(context, i, v[i]).FromJust());
  }

  info.GetReturnValue().Set(array);
}

// static
void CanvasApi::SetTransform(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());

  float v[6];

  if (info.Length() >= 1 && info[0]->IsArray()) {
    v8::Local<v8::Array> array = info[0].As<v8::Array>();
    if (array->Length() < 6) {
      return;
    }
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    for (int i = 0; i < 6; i++) {
      v8::Local<v8::Value> value = array->Get(context, i).ToLocalChecked();
      if (!value->IsNumber()) {
        return;
      }
      v[i] = value.As<v8::Number>()->Value();
    }
  } else if (info.Length() >= 6 && info[0]->IsNumber() && info[1]->IsNumber() &&
             info[2]->IsNumber() && info[3]->IsNumber() &&
             info[4]->IsNumber() && info[5]->IsNumber()) {
    for (int i = 0; i < 6; i++) {
      v[i] = info[i].As<v8::Number>()->Value();
    }
  } else {
    return;
  }

  SkMatrix m = SkMatrix::MakeAll(v[0], v[2], v[4], v[1], v[3], v[5], 0, 0, 1);
  api->skia_canvas()->setMatrix(m);
}

// static
void CanvasApi::ResetTransform(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  api->skia_canvas()->resetMatrix();
}

// static
void CanvasApi::Save(const v8::FunctionCallbackInfo<v8::Value>& info) {
  // Context to save:
  // 1. transformation matrix (saved in SkCanvas)
  // 2. clipping region (saved in SkCanvas)
  // 3. all styles, dash, font, etc. (saved in saved_state_)
  CanvasApi* api = CanvasApi::Get(info.This());
  api->skia_canvas()->save();
  api->saved_state_.emplace_back(api->state_);
}

// static
void CanvasApi::Restore(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* api = CanvasApi::Get(info.This());
  if (!api->saved_state_.empty()) {
    api->skia_canvas()->restore();
    api->state_ = api->saved_state_.back();
    api->saved_state_.pop_back();
  }
}

// static
void CanvasApi::CreateGradient(const v8::FunctionCallbackInfo<v8::Value>& info,
                               int size, const char* name) {
  CanvasApi* api = CanvasApi::Get(info.This());
  std::vector<v8::Local<v8::Value>> args;
  args.resize(size);
  for (int i = 0; i < size; i++) {
    if (info.Length() < size || !info[i]->IsNumber()) {
      std::stringstream ss;
      ss << name << " requires " << size << " number arguments.";
      api->js()->ThrowError(ss.str());
      return;
    }
    args[i] = info[i];
  }
  v8::Local<v8::Object> gradient =
      api->api()
          ->GetCanvasGradientConstructor()
          ->NewInstance(info.GetIsolate()->GetCurrentContext(), size,
                        args.data())
          .ToLocalChecked();
  info.GetReturnValue().Set(gradient);
}

// static
void CanvasApi::CreateLinearGradient(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  CreateGradient(info, 4, "createLinearGradient");
}

// static
void CanvasApi::CreateRadialGradient(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  CreateGradient(info, 6, "createRadialGradient");
}

// static
void CanvasApi::CreateImageData(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());

  if (info.Length() < 1) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  v8::Local<v8::Object> image_data;

  if (api->IsInstanceOf(info[0], api->GetImageDataConstructor())) {
    v8::Local<v8::Value> args[1] = {info[0]};
    image_data =
        api->GetImageDataConstructor()
            ->NewInstance(info.GetIsolate()->GetCurrentContext(), 1, args)
            .ToLocalChecked();
  } else if (info.Length() >= 2 && info[0]->IsUint32() && info[1]->IsUint32()) {
    v8::Local<v8::Value> args[2] = {info[0], info[1]};
    image_data =
        api->GetImageDataConstructor()
            ->NewInstance(info.GetIsolate()->GetCurrentContext(), 2, args)
            .ToLocalChecked();
  } else {
    api->js()->ThrowInvalidArgument();
    return;
  }

  info.GetReturnValue().Set(image_data);
}

// static
void CanvasApi::GetImageData(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* canvas = CanvasApi::Get(info.This());
  JsApi* api = JsApi::Get(info.GetIsolate());

  int x = 0;
  int y = 0;
  int width = canvas->canvas()->width();
  int height = canvas->canvas()->height();
  int w = width;
  int h = height;

  if (info.Length() >= 1) {
    if (!info[0]->IsNumber()) {
      api->js()->ThrowInvalidArgument();
      return;
    }
    x = info[0].As<v8::Number>()->Value();
    if (info.Length() >= 2) {
      if (!info[1]->IsNumber()) {
        api->js()->ThrowInvalidArgument();
        return;
      }
      y = info[1].As<v8::Number>()->Value();
      if (info.Length() >= 3) {
        if (!info[2]->IsNumber()) {
          api->js()->ThrowInvalidArgument();
          return;
        }
        w = info[2].As<v8::Number>()->Value();
        if (info.Length() >= 4) {
          if (!info[3]->IsNumber()) {
            api->js()->ThrowInvalidArgument();
            return;
          }
          h = info[3].As<v8::Number>()->Value();
        }
      }
    }
  }

  if (w == 0 || h == 0) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  if (x < 0) {
    x = 0;
  } else if (x >= width) {
    x = width - 1;
  }

  if (y < 0) {
    y = 0;
  } else if (y >= height) {
    y = height - 1;
  }

  if (w > 0 && x + w > width) {
    w = width - x;
  } else if (w < 0) {
    if (x + w < 0) {
      w = -x;
    }
    x += w;
    w = -w;
  }

  if (h > 0 && y + h > height) {
    h = height - y;
  } else if (h < 0) {
    if (y + h < 0) {
      h = -y;
    }
    y += h;
    h = -h;
  }

  v8::Local<v8::Value> args[2] = {
      v8::Number::New(api->isolate(), w),
      v8::Number::New(api->isolate(), h),
  };

  v8::Local<v8::Object> object =
      api->GetImageDataConstructor()
          ->NewInstance(info.GetIsolate()->GetCurrentContext(), 2, args)
          .ToLocalChecked();

  ImageDataApi* image_data = ImageDataApi::Get(object);
  canvas->canvas()->ReadPixels(x, y, w, h, image_data->backing_store()->Data());

  info.GetReturnValue().Set(object);
}

// static
void CanvasApi::PutImageData(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* canvas = CanvasApi::Get(info.This());
  JsApi* api = canvas->api();

  if (info.Length() < 3 ||
      !api->IsInstanceOf(info[0], api->GetImageDataConstructor()) ||
      !info[1]->IsUint32() || !info[2]->IsUint32()) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  ImageDataApi* image_data = ImageDataApi::Get(info[0]);
  int dx = info[1].As<v8::Uint32>()->Value();
  int dy = info[2].As<v8::Uint32>()->Value();
  int sx = 0;
  int sy = 0;
  int w = image_data->width();
  int h = image_data->height();

  if (info.Length() > 3) {
    if (info.Length() < 7 || !info[3]->IsUint32() || !info[4]->IsUint32() ||
        !info[5]->IsUint32() || !info[6]->IsUint32()) {
      api->js()->ThrowInvalidArgument();
      return;
    }
    dx = info[3].As<v8::Uint32>()->Value();
    dy = info[4].As<v8::Uint32>()->Value();
    w = info[5].As<v8::Uint32>()->Value();
    h = info[6].As<v8::Uint32>()->Value();
  }

  if (sx < 0 || sx >= image_data->width() || sy < 0 ||
      sy >= image_data->height() || w <= 0 || h <= 0 ||
      sx + w > image_data->width() || sy + h > image_data->height()) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  if (dx < 0 || dx >= canvas->canvas()->width() || dy < 0 ||
      dy >= canvas->canvas()->height()) {
    // Nothing to copy but not a failure.
    return;
  }

  uint8_t* data = static_cast<uint8_t*>(image_data->backing_store()->Data());
  int stride = image_data->width() * 4;
  uint8_t* pixels = data + sy * stride + sx * 4;
  canvas->canvas()->WritePixels(pixels, dx, dy, w, h, stride);
}

void CanvasApi::Encode(const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasApi* canvas = CanvasApi::Get(info.This());

  sk_sp<SkImage> image = canvas->canvas()->MakeImageSnapshot();
  ASSERT(image);
  image = image->makeNonTextureImage();
  ASSERT(image);

  info.GetReturnValue().Set(EncodeInBackground(image, info, canvas->api()));
}

// static
void CanvasApi::DrawImage(const v8::FunctionCallbackInfo<v8::Value>& info) {
  ASSERT(IsMainThread());
  JsApi* api = JsApi::Get(info.GetIsolate());
  // Length must be 3, 4 or 5 (image, dx, dy, [dw, dh]),
  // or 7, 8 or 9 (image sx, sy, sw, sh, dx, dy, [dw, dh]).
  if (info.Length() < 3 || info.Length() == 6 ||
      !(api->IsInstanceOf(info[0], api->GetImageBitmapConstructor()) ||
        api->IsInstanceOf(info[0],
                          api->GetCanvasRenderingContext2DConstructor()))) {
    api->js()->ThrowInvalidArgument();
    return;
  }

  sk_sp<SkImage> source_image;
  float source_width = 0;
  float source_height = 0;

  if (api->IsInstanceOf(info[0], api->GetImageBitmapConstructor())) {
    ImageBitmapApi* source_bitmap = ImageBitmapApi::Get(info[0]);
    source_width = source_bitmap->width();
    source_height = source_bitmap->height();
    source_image = source_bitmap->texture();
  } else {
    CanvasApi* source_canvas = CanvasApi::Get(info[0]);
    source_width = source_canvas->canvas()->width();
    source_height = source_canvas->canvas()->height();
    source_image = source_canvas->canvas()->MakeImageSnapshot();
    ASSERT(source_image->isTextureBacked());
  }

  float sx = 0;
  float sy = 0;
  float sw = source_width;
  float sh = source_height;
  float dx = 0;
  float dy = 0;
  float dw = sw;
  float dh = sh;

  int length = std::min(info.Length(), 9);
  for (int i = 1; i < length; i++) {
    if (!info[i]->IsNumber()) {
      api->js()->ThrowInvalidArgument();
      return;
    }
  }

  if (length <= 5) {
    dx = info[1].As<v8::Number>()->Value();
    dy = info[2].As<v8::Number>()->Value();
    if (length >= 4) {
      dw = info[3].As<v8::Number>()->Value();
      if (length == 5) {
        dh = info[4].As<v8::Number>()->Value();
      }
    }
  } else {
    sx = info[1].As<v8::Number>()->Value();
    sy = info[2].As<v8::Number>()->Value();
    sw = info[3].As<v8::Number>()->Value();
    sh = info[4].As<v8::Number>()->Value();
    dx = info[5].As<v8::Number>()->Value();
    dy = info[6].As<v8::Number>()->Value();
    dw = sw;
    dh = sh;
    if (length >= 8) {
      dw = info[7].As<v8::Number>()->Value();
      if (length == 9) {
        dh = info[8].As<v8::Number>()->Value();
      }
    }
  }

  CanvasApi* canvas = CanvasApi::Get(info.This());
  canvas->canvas()->SetCurrentContext();

  if (sx == 0 && sy == 0 && sw == source_width && sh == source_height &&
      dw == source_width && dh == source_height) {
    canvas->skia_canvas()->drawImage(source_image.get(), dx, dy);
  } else {
    SkRect src = SkRect::MakeXYWH(sx, sy, sw, sh);
    SkRect dst = SkRect::MakeXYWH(dx, dy, dw, dh);
    canvas->skia_canvas()->drawImageRect(
        source_image.get(), src, dst, canvas->state_.sampling_options, nullptr,
        SkCanvas::kFast_SrcRectConstraint);
  }
}

CanvasGradientApi::CanvasGradientApi(
    JsApi* api, v8::Local<v8::Object> thiz,
    const v8::FunctionCallbackInfo<v8::Value>& info)
    : JsApiWrapper(info.GetIsolate(), thiz), ref_count_(0) {
  if (info.Length() == 4) {
    type_ = SkShader::kLinear_GradientType;
  } else if (info.Length() == 6) {
    type_ = SkShader::kRadial_GradientType;
  } else {
    api->js()->ThrowIllegalConstructor();
  }
  params_.resize(info.Length());
  for (int i = 0; i < params_.size(); i++) {
    params_[i] = info[i].As<v8::Number>()->Value();
  }
}

CanvasGradientApi::~CanvasGradientApi() {}

sk_sp<SkShader> CanvasGradientApi::GetShader() {
  ASSERT(ref_count_ > 0);
  if (!shader_) {
    if (type_ == SkShader::kLinear_GradientType) {
      SkPoint points[2];
      points[0].fX = params_[0];
      points[0].fY = params_[1];
      points[1].fX = params_[2];
      points[1].fY = params_[3];
      const SkColor* colors = colors_.data();
      const SkScalar* positions = positions_.data();
      shader_ = SkGradientShader::MakeLinear(
          points, colors, positions, colors_.size(), SkTileMode::kClamp);
    } else if (type_ == SkShader::kRadial_GradientType) {
      SkPoint start;
      start.fX = params_[0];
      start.fY = params_[1];
      float start_radius = params_[2];
      SkPoint end;
      end.fX = params_[3];
      end.fY = params_[4];
      float end_radius = params_[5];
      const SkColor* colors = colors_.data();
      const SkScalar* positions = positions_.data();
      shader_ = SkGradientShader::MakeTwoPointConical(
          start, start_radius, end, end_radius, colors, positions,
          colors_.size(), SkTileMode::kClamp);
    } else {
      ASSERT(false);
    }
  }
  return shader_;
}

// static
v8::Local<v8::Function> CanvasGradientApi::GetConstructor(
    JsApi* api, const JsScope& scope) {
  v8::Local<v8::FunctionTemplate> canvas_gradient =
      v8::FunctionTemplate::New(scope.isolate, CanvasGradient);
  canvas_gradient->SetClassName(
      scope.GetConstantString(StringId::CanvasGradient));

  v8::Local<v8::ObjectTemplate> instance = canvas_gradient->InstanceTemplate();
  // Used in JsApiWrapper to track this.
  instance->SetInternalFieldCount(1);

  v8::Local<v8::ObjectTemplate> prototype =
      canvas_gradient->PrototypeTemplate();

  scope.Set(prototype, StringId::addColorStop, AddColorStop);

  return canvas_gradient->GetFunction(scope.context).ToLocalChecked();
}

void CanvasApi::OnGradientUpdated(CanvasGradientApi* gradient) {
  if (state_.fill_gradient == gradient) {
    state_.fill_paint.setShader(gradient->GetShader());
  }
  if (state_.stroke_gradient == gradient) {
    state_.stroke_paint.setShader(gradient->GetShader());
  }
  for (State& state : saved_state_) {
    if (state.fill_gradient == gradient) {
      state.fill_paint.setShader(gradient->GetShader());
    }
    if (state.stroke_gradient == gradient) {
      state.stroke_paint.setShader(gradient->GetShader());
    }
  }
}

// static
void CanvasGradientApi::AddColorStop(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  CanvasGradientApi* gradient = CanvasGradientApi::Get(info.This());

  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsString()) {
    gradient->js()->ThrowError(
        "addColorStop requires a number and a CSS color string.");
    return;
  }

  float offset = info[0].As<v8::Number>()->Value();
  if (offset < 0 || offset > 1) {
    gradient->js()->ThrowError("offset must be between 0 and 1, inclusive.");
    return;
  }

  std::string css = gradient->js()->ToString(info[1]);
  SkColor color;
  if (!CSSColorToSkColor(css, &color)) {
    gradient->js()->ThrowError("Invalid color.");
    return;
  }

  gradient->colors_.push_back(color);
  gradient->positions_.push_back(offset);

  // Invalidate the shader.
  gradient->shader_ = nullptr;

  if (gradient->ref_count_ > 0) {
    // This gradient is the fillStyle or strokeStyle of an existing canvas.
    // Update their SkPaint instances to reflect the new color stop.
    JsApiTracker<CanvasApi>::ForEach([gradient](CanvasApi* canvas_api) {
      canvas_api->OnGradientUpdated(gradient);
    });
  }
}

ImageDataApi::ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz, int width,
                           int height)
    : JsApiWrapper(api->isolate(), thiz), width_(width), height_(height) {
  v8::Local<v8::ArrayBuffer> buffer =
      v8::ArrayBuffer::New(api->isolate(), width * height * 4);
  backing_store_ = buffer->GetBackingStore();
  data_.Reset(api->isolate(),
              v8::Uint8ClampedArray::New(buffer, 0, width * height * 4));
}

ImageDataApi::ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
                           v8::Local<v8::Uint8Array> data, int width,
                           int height)
    : ImageDataApi(api, thiz, width, height) {
  data->CopyContents(backing_store_->Data(), backing_store_->ByteLength());
}

ImageDataApi::ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
                           v8::Local<v8::Uint8ClampedArray> data, int width,
                           int height)
    : ImageDataApi(api, thiz, width, height) {
  data->CopyContents(backing_store_->Data(), backing_store_->ByteLength());
}

ImageDataApi::ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
                           v8::Local<v8::Uint16Array> data, int width,
                           int height)
    : ImageDataApi(api, thiz, width, height) {
  int length = width * height * 4;
  std::shared_ptr<v8::BackingStore> store = data->Buffer()->GetBackingStore();
  uint16_t* src = static_cast<uint16_t*>(store->Data());
  uint8_t* dst = static_cast<uint8_t*>(backing_store_->Data());
  for (int i = 0; i < length; i++) {
    dst[i] = src[i] / 256;
  }
}

ImageDataApi::ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
                           v8::Local<v8::Float32Array> data, int width,
                           int height)
    : ImageDataApi(api, thiz, width, height) {
  int length = width * height * 4;
  std::shared_ptr<v8::BackingStore> store = data->Buffer()->GetBackingStore();
  float* src = static_cast<float*>(store->Data());
  uint8_t* dst = static_cast<uint8_t*>(backing_store_->Data());
  for (int i = 0; i < length; i++) {
    dst[i] = src[i] * 255;
  }
}

ImageDataApi::ImageDataApi(JsApi* api, v8::Local<v8::Object> thiz,
                           const ImageDataApi* source, int x, int y, int width,
                           int height)
    : JsApiWrapper(api->isolate(), thiz), width_(width), height_(height) {
  v8::Local<v8::ArrayBuffer> buffer =
      v8::ArrayBuffer::New(api->isolate(), width * height * 4);
  backing_store_ = buffer->GetBackingStore();
  data_.Reset(api->isolate(),
              v8::Uint8ClampedArray::New(buffer, 0, width * height * 4));

  uint8_t* dst = static_cast<uint8_t*>(backing_store_->Data());
  const uint8_t* src = static_cast<uint8_t*>(source->backing_store()->Data());
  src += y * source->width() * 4 + x * 4;

  for (int yy = 0; yy < height; yy++) {
    std::memcpy(dst, src, width * 4);
    dst += width * 4;
    src += source->width() * 4;
  }
}

ImageDataApi::~ImageDataApi() {}

// static
v8::Local<v8::Function> ImageDataApi::GetConstructor(JsApi* api,
                                                     const JsScope& scope) {
  v8::Local<v8::FunctionTemplate> image_data =
      v8::FunctionTemplate::New(scope.isolate, ImageData);
  image_data->SetClassName(scope.GetConstantString(StringId::ImageData));

  v8::Local<v8::ObjectTemplate> instance = image_data->InstanceTemplate();
  // Used in JsApiWrapper to track this.
  instance->SetInternalFieldCount(1);

  scope.Set(image_data, StringId::decode, Decode);

  v8::Local<v8::ObjectTemplate> prototype = image_data->PrototypeTemplate();

  scope.Set(prototype, StringId::data, GetData);
  scope.Set(prototype, StringId::width, GetWidth);
  scope.Set(prototype, StringId::height, GetHeight);
  scope.Set(prototype, StringId::encode, Encode);

  return image_data->GetFunction(scope.context).ToLocalChecked();
}

// static
void ImageDataApi::GetData(v8::Local<v8::String> property,
                           const v8::PropertyCallbackInfo<v8::Value>& info) {
  ImageDataApi* image_data = ImageDataApi::Get(info.This());
  info.GetReturnValue().Set(image_data->data_.Get(info.GetIsolate()));
}

// static
void ImageDataApi::GetWidth(v8::Local<v8::String> property,
                            const v8::PropertyCallbackInfo<v8::Value>& info) {
  ImageDataApi* image_data = ImageDataApi::Get(info.This());
  info.GetReturnValue().Set(image_data->width_);
}

// static
void ImageDataApi::GetHeight(v8::Local<v8::String> property,
                             const v8::PropertyCallbackInfo<v8::Value>& info) {
  ImageDataApi* image_data = ImageDataApi::Get(info.This());
  info.GetReturnValue().Set(image_data->height_);
}

// static
void ImageDataApi::Encode(const v8::FunctionCallbackInfo<v8::Value>& info) {
  ImageDataApi* image_data = ImageDataApi::Get(info.This());

  SkImageInfo image_info =
      SkImageInfo::Make(image_data->width(), image_data->height(),
                        kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
  size_t row_stride = image_data->width() * 4;
  const void* pixels = image_data->backing_store()->Data();
  SkPixmap pixmap{image_info, pixels, row_stride};
  sk_sp<SkImage> image = SkImage::MakeFromRaster(pixmap, nullptr, nullptr);
  ASSERT(image);
  image = image->makeNonTextureImage();
  ASSERT(image);

  info.GetReturnValue().Set(EncodeInBackground(image, info, image_data->api()));
}

// static
void ImageDataApi::Decode(const v8::FunctionCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());
  sk_sp<SkData> data = PrepareToDecode(api, info);

  if (!data) {
    return;
  }

  info.GetReturnValue().Set(
      api->PostToBackgroundAndResolve([data]() -> JsApi::ResolveFunction {
        sk_sp<SkImage> image = SkImage::MakeFromEncoded(data);
        if (!image) {
          return JsApi::Reject("Failed to decode image");
        }
        return [image](JsApi* api, const JsScope& scope,
                       v8::Promise::Resolver* resolver) {
          v8::Local<v8::Value> args[] = {
              v8::Number::New(api->isolate(), image->width()),
              v8::Number::New(api->isolate(), image->height()),
          };
          v8::Local<v8::Object> object =
              api->GetImageDataConstructor()
                  ->NewInstance(scope.context, 2, args)
                  .ToLocalChecked();
          ImageDataApi* image_data = ImageDataApi::Get(object);
          SkImageInfo image_info =
              SkImageInfo::Make(image->width(), image->height(),
                                kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
          image->readPixels(nullptr, image_info,
                            image_data->backing_store()->Data(),
                            image->width() * 4, 0, 0);
          (void) resolver->Resolve(scope.context, object);
        };
      }));
}

ImageBitmapApi::ImageBitmapApi(JsApi* api, v8::Local<v8::Object> thiz,
                               sk_sp<SkImage> texture)
    : JsApiWrapper(api->isolate(), thiz), texture_(texture) {
  ASSERT(texture->isTextureBacked());
}

ImageBitmapApi::~ImageBitmapApi() {}

// static
v8::Local<v8::Function> ImageBitmapApi::GetConstructor(JsApi* api,
                                                       const JsScope& scope) {
  v8::Local<v8::FunctionTemplate> image_bitmap =
      v8::FunctionTemplate::New(scope.isolate, ImageBitmap);
  image_bitmap->SetClassName(scope.GetConstantString(StringId::ImageBitmap));

  v8::Local<v8::ObjectTemplate> instance = image_bitmap->InstanceTemplate();
  // Used in JsApiWrapper to track this.
  instance->SetInternalFieldCount(1);

  scope.Set(image_bitmap, StringId::decode, Decode);

  v8::Local<v8::ObjectTemplate> prototype = image_bitmap->PrototypeTemplate();

  scope.Set(prototype, StringId::width, GetWidth);
  scope.Set(prototype, StringId::height, GetHeight);
  scope.Set(prototype, StringId::encode, Encode);

  return image_bitmap->GetFunction(scope.context).ToLocalChecked();
}

// static
void ImageBitmapApi::GetWidth(v8::Local<v8::String> property,
                              const v8::PropertyCallbackInfo<v8::Value>& info) {
  ImageBitmapApi* image_bitmap = ImageBitmapApi::Get(info.This());
  info.GetReturnValue().Set(image_bitmap->width());
}

// static
void ImageBitmapApi::GetHeight(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  ImageBitmapApi* image_bitmap = ImageBitmapApi::Get(info.This());
  info.GetReturnValue().Set(image_bitmap->height());
}

// static
void ImageBitmapApi::Encode(const v8::FunctionCallbackInfo<v8::Value>& info) {
  ImageBitmapApi* image_bitmap = ImageBitmapApi::Get(info.This());
  sk_sp<SkImage> image = image_bitmap->texture()->makeNonTextureImage();
  ASSERT(image);
  info.GetReturnValue().Set(
      EncodeInBackground(image, info, image_bitmap->api()));
}

// static
void ImageBitmapApi::Decode(const v8::FunctionCallbackInfo<v8::Value>& info) {
  JsApi* api = JsApi::Get(info.GetIsolate());
  sk_sp<SkData> data = PrepareToDecode(api, info);

  if (!data) {
    return;
  }

  info.GetReturnValue().Set(
      api->PostToBackgroundAndResolve([data]() -> JsApi::ResolveFunction {
        sk_sp<SkImage> image = SkImage::MakeFromEncoded(data);
        if (!image) {
          return JsApi::Reject("Failed to decode image");
        }
        return [image](JsApi* api, const JsScope& scope,
                       v8::Promise::Resolver* resolver) {
          RenderCanvasSharedContext* context = api->canvas_shared_context();
          context->SetCurrentContext();
          sk_sp<SkImage> texture = image->makeTextureImage(
              context->skia_context(), GrMipMapped::kNo, SkBudgeted::kNo);
          ASSERT(texture);
          ASSERT(texture->isTextureBacked());
          v8::Local<v8::Value> args[] = {
              v8::External::New(api->isolate(), texture.release()),
          };
          v8::Local<v8::Object> object =
              api->GetImageBitmapConstructor()
                  ->NewInstance(scope.context, 1, args)
                  .ToLocalChecked();
          (void) resolver->Resolve(scope.context, object);
        };
      }));
}
