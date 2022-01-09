#include "render_canvas.h"

#include "args.h"
#include "console.h"
#include "fail.h"
#include "window.h"

RenderCanvasSharedContext::RenderCanvasSharedContext(Window* window)
    : owner_(window) {
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Note: not double buffered.
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  // Note: never visible.
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);

  static bool first_render_context = true;
  if (Args().profile_startup && first_render_context) {
    $(DEV) << "[profile-startup] create CanvasContext start: " << glfwGetTime();
  }

  // Note: share our GL context with the owner window.
  context_ = glfwCreateWindow(window->width(), window->height(), "", nullptr,
                              window->window());

  if (!context_) {
    const char* ptr = nullptr;
    int err = glfwGetError(&ptr);
    Fail("glfwCreateWindow failed (%d): %s\n", err, ptr);
  }

  gr_interface_ = GrGLMakeNativeInterface();
  ASSERT(gr_interface_);
  gr_context_ = GrDirectContext::MakeGL(gr_interface_);
  ASSERT(gr_context_);

  if (Args().profile_startup && first_render_context) {
    first_render_context = false;
    $(DEV) << "[profile-startup] create CanvasContext end: " << glfwGetTime();
  }
}

RenderCanvasSharedContext::~RenderCanvasSharedContext() {
  gr_context_.reset();
  gr_interface_.reset();
  glfwDestroyWindow(context_);
}

void RenderCanvasSharedContext::SetCurrentContext() {
  owner_->SetCurrentContext(context_);
}

void RenderCanvasSharedContext::Flush() {
  SetCurrentContext();
  gr_context_->flushAndSubmit();
  // flushAndSubmit doesn't call glFlush; this causes rendering issues on MacOS.
  glFlush();
}

RenderCanvas::RenderCanvas(RenderCanvasSharedContext* shared_context, int width,
                           int height)
    : shared_context_(shared_context),
      texture_(0),
      previous_texture_(0),
      create_generation_(0),
      width_(-1),
      height_(-1),
      previous_width_(-1),
      previous_height_(-1) {
  Resize(width, height);
}

RenderCanvas::~RenderCanvas() {
  SetCurrentContext();
  surface_.reset();
  glDeleteTextures(1, &texture_);
  DropPreviousTexture();
}

void RenderCanvas::Reset() {
  SkCanvas* canvas = surface_->getCanvas();
  canvas->resetMatrix();
  canvas->clear(SK_ColorBLACK);
  DropPreviousTexture();
}

void RenderCanvas::SetCurrentContext() {
  shared_context_->SetCurrentContext();
}

int RenderCanvas::GetWidthForDraw() {
  if (GetTextureForDraw() == texture_) {
    return width_;
  } else {
    return previous_width_;
  }
}

int RenderCanvas::GetHeightForDraw() {
  if (GetTextureForDraw() == texture_) {
    return height_;
  } else {
    return previous_height_;
  }
}

GLuint RenderCanvas::GetTextureForDraw() {
  // Return a previous texture until the new one has been painted by the app.
  // This makes Window.js show the old content after resizes for apps that
  // don't redraw continuously.
  if (previous_texture_) {
    if (surface_->generationID() == create_generation_) {
      return previous_texture_;
    }
    DropPreviousTexture();
  }
  return texture_;
}

void RenderCanvas::Resize(int width, int height) {
  if (width == width_ && height == height_) {
    return;
  }

  SetCurrentContext();

  SkM44 matrix;

  if (surface_) {
    matrix = surface_->getCanvas()->getLocalToDevice();
    surface_.reset();

    if (previous_texture_ == 0) {
      previous_texture_ = texture_;
      previous_width_ = width_;
      previous_height_ = height_;
    } else {
      // The current texture was never updated; drop it instead.
      glDeleteTextures(1, &texture_);
    }
  }

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  ASSERT_NO_GL_ERROR();

  GrGLTextureInfo texture_info;
  texture_info.fTarget = GL_TEXTURE_2D;
  texture_info.fID = texture_;
  texture_info.fFormat = GL_RGBA8;

  GrBackendTexture gr_backend(width, height, GrMipmapped::kNo, texture_info);
  ASSERT(gr_backend.isValid());

  SkColorType color_type = kRGBA_8888_SkColorType;
  int sample_count = 0;
  surface_ = SkSurface::MakeFromBackendTexture(
      shared_context_->skia_context(), gr_backend, kBottomLeft_GrSurfaceOrigin,
      sample_count, color_type, nullptr, nullptr);
  ASSERT(surface_);

  surface_->getCanvas()->clear(SK_ColorBLACK);
  surface_->getCanvas()->setMatrix(matrix);

  create_generation_ = surface_->generationID();

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
  SetCurrentContext();
  SkImageInfo info = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType,
                                       kUnpremul_SkAlphaType);
  SkPixmap pixmap{info, pixels, (size_t) row_stride};
  surface_->writePixels(pixmap, x, y);
}

void RenderCanvas::DropPreviousTexture() {
  if (previous_texture_) {
    glDeleteTextures(1, &previous_texture_);
    previous_texture_ = 0;
  }
}
