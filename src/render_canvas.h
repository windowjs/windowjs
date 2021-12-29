#ifndef WINDOWJS_RENDER_CANVAS_H
#define WINDOWJS_RENDER_CANVAS_H

// Must come before glfw3.h.
#include <glad/glad.h>
// Comes after glad.h.
#include <GLFW/glfw3.h>

// Skia includes.
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/gpu/GrDirectContext.h>
#include <skia/include/gpu/gl/GrGLInterface.h>

class Window;

class RenderCanvasSharedContext {
 public:
  explicit RenderCanvasSharedContext(Window* window);
  ~RenderCanvasSharedContext();

  void SetCurrentContext();
  void Flush();

  GLFWwindow* context() const { return context_; }
  Window* owner() const { return owner_; }
  GrDirectContext* skia_context() const { return gr_context_.get(); }

 private:
  Window* owner_;
  GLFWwindow* context_;

  sk_sp<const GrGLInterface> gr_interface_;
  sk_sp<GrDirectContext> gr_context_;
};

class RenderCanvas final {
 public:
  RenderCanvas(RenderCanvasSharedContext* shared_context, int width,
               int height);
  ~RenderCanvas();

  int width() const { return width_; }
  int height() const { return height_; }
  SkCanvas* canvas() const { return surface_->getCanvas(); }
  RenderCanvasSharedContext* shared_context() const { return shared_context_; }

  void Reset();
  void SetCurrentContext();

  int GetWidthForDraw();
  int GetHeightForDraw();
  GLuint GetTextureForDraw();

  void Resize(int width, int height);

  sk_sp<SkImage> MakeImageSnapshot();

  void ReadPixels(int x, int y, int width, int height, void* destination);
  void WritePixels(const void* pixels, int x, int y, int width, int height,
                   int row_stride);

 private:
  void DropPreviousTexture();

  RenderCanvasSharedContext* shared_context_;

  GLuint texture_;
  GLuint previous_texture_;

  sk_sp<SkSurface> surface_;
  uint32_t create_generation_;

  int width_;
  int height_;
  int previous_width_;
  int previous_height_;
};

#endif  // WINDOWJS_RENDER_CANVAS_H
