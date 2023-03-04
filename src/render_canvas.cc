#include "render_canvas.h"

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <skia/include/core/SkColorSpace.h>
#include <skia/include/gpu/gl/egl/GrGLMakeEGLInterface.h>

#include "args.h"
#include "console.h"
#include "fail.h"
#include "window.h"

RenderCanvasSharedContext::RenderCanvasSharedContext(Window* window)
    : owner_(window) {
  static bool first_render_context = true;
  if (Args().profile_startup && first_render_context) {
    $(DEV) << "[profile-startup] create RenderCanvasSharedContext start: "
           << glfwGetTime();
  }

  gr_interface_ = GrGLMakeEGLInterface();
  ASSERT(gr_interface_);
  gr_context_ = GrDirectContext::MakeGL(gr_interface_);
  ASSERT(gr_context_);

  if (Args().profile_startup && first_render_context) {
    first_render_context = false;
    $(DEV) << "[profile-startup] create RenderCanvasSharedContext end: "
           << glfwGetTime();
  }
}

RenderCanvasSharedContext::~RenderCanvasSharedContext() {}

void RenderCanvasSharedContext::Flush() {
  gr_context_->flush();
}

RenderCanvas::RenderCanvas(RenderCanvasSharedContext* shared_context, int width,
                           int height, Target target)
    : shared_context_(shared_context),
      width_(-1),
      height_(-1),
      target_(target) {
  Resize(width, height);
}

RenderCanvas::~RenderCanvas() {
  surface_.reset();
  if (texture_.isValid()) {
    shared_context_->skia_context()->deleteBackendTexture(texture_);
  }
}

void RenderCanvas::Reset() {
  SkCanvas* canvas = surface_->getCanvas();
  canvas->resetMatrix();
  canvas->clear(SK_ColorBLACK);
}

void RenderCanvas::Resize(int width, int height) {
  if (width == width_ && height == height_) {
    return;
  }

  const SkColorType color_type = kRGBA_8888_SkColorType;
  // TODO: consider using 1, 2 or 4 sample_counts.
  // See also the GLFW_SAMPLES hint.
  const int sample_count = 0;

  std::vector<SkM44> transforms;

  if (surface_) {
    SkCanvas* canvas = surface_->getCanvas();
    int count = canvas->getSaveCount();
    transforms.resize(count);
    for (int i = 0; i < count; i++) {
      transforms[i] = canvas->getLocalToDevice();
      canvas->restore();
    }
  }

  if (target_ == FRAMEBUFFER_0) {
    GrGLFramebufferInfo info;
    // Framebuffer 0 is the screen/window buffer.
    info.fFBOID = 0;
    info.fFormat = GL_RGBA8;

    int stencil_bits = 8;
    GrBackendRenderTarget target(width, height, sample_count, stencil_bits,
                                 info);
    ASSERT(target.isValid());

    surface_ = SkSurface::MakeFromBackendRenderTarget(
        shared_context_->skia_context(), target, kBottomLeft_GrSurfaceOrigin,
        color_type, nullptr, nullptr);
    ASSERT(surface_);
  } else {
    GrBackendTexture texture =
        shared_context_->skia_context()->createBackendTexture(
            width, height, color_type, GrMipmapped::kNo, GrRenderable::kYes);
    ASSERT(texture.isValid());

    sk_sp<SkSurface> surface = SkSurface::MakeFromBackendTexture(
        shared_context_->skia_context(), texture, kBottomLeft_GrSurfaceOrigin,
        sample_count, color_type, nullptr, nullptr);
    ASSERT(surface);
    surface->getCanvas()->clear(SK_ColorBLACK);

    if (surface_) {
      // Copy the old contents into the new texture,
      // to preserve the pixels on resizes in applications that don't redraw.
      surface_->draw(surface->getCanvas(), 0, 0);
      // Nothing gets drawn before the texture is deleted without this flush:
      surface_->flush();

      surface_.reset();
      shared_context_->skia_context()->deleteBackendTexture(texture_);
    }

    surface_ = surface;
    texture_ = texture;
  }

  // Retain any transforms and clips across resizes.
  if (!transforms.empty()) {
    SkCanvas* canvas = surface_->getCanvas();
    int count = transforms.size();
    for (int i = count - 1; i >= 0; i--) {
      canvas->setMatrix(transforms[i]);
      if (i > 0) {
        canvas->save();
      }
    }
    ASSERT(canvas->getSaveCount() == count);
  }

  width_ = width;
  height_ = height;
}

sk_sp<SkImage> RenderCanvas::MakeImageSnapshot() {
  shared_context_->Flush();
  return surface_->makeImageSnapshot();
}

void RenderCanvas::ReadPixels(int x, int y, int width, int height,
                              void* destination) {
  ASSERT(x >= 0);
  ASSERT(y >= 0);
  ASSERT(x + width <= width_);
  ASSERT(y + height <= height_);

  shared_context_->Flush();

  SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType,
                                       kUnpremul_SkAlphaType);
  int stride = width * 4;
  bool result = surface_->readPixels(info, destination, stride, x, y);
  ASSERT(result);
}

void RenderCanvas::WritePixels(const void* pixels, int x, int y, int width,
                               int height, int row_stride) {
  SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType,
                                       kUnpremul_SkAlphaType);
  SkPixmap pixmap{info, pixels, (size_t) row_stride};
  surface_->writePixels(pixmap, x, y);
}
