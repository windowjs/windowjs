#ifndef WINDOWJS_WINDOW_H
#define WINDOWJS_WINDOW_H

#include <memory>
#include <string>
#include <vector>

#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "console.h"
#include "render_canvas.h"
#include "stats.h"

class Window final {
 public:
  static void Init();
  static void Shutdown();

  class Delegate {
   public:
    virtual ~Delegate() = default;

    virtual void OnKey(int key, int scancode, int action, int mods) {}
    virtual void OnCharacter(unsigned int codepoint) {}

    virtual void OnMouseMove(double x, double y) {}
    virtual void OnMouseButton(int button, bool pressed, double x, double y) {}
    virtual void OnMouseWheel(double x, double y) {}
    virtual void OnMouseEnter(bool entered) {}

    virtual void OnFocus(bool focused) {}
    virtual void OnResize(int width, int height) {}
    virtual void OnClose() {}
    virtual void OnDrop(std::vector<std::string> paths) {}
    virtual void OnMaximize() {}
    virtual void OnMinimize() {}
    virtual void OnRestore() {}

    virtual void OnTitleChanged() {}
  };

  Window(Delegate* delegate, int width, int height);
  ~Window();

  GLFWwindow* window() const { return window_; }
  RenderCanvas* canvas() const { return canvas_; }
  RenderCanvasSharedContext* shared_context() const {
    return shared_context_.get();
  }
  ConsoleOverlay* console_overlay() { return console_overlay_.get(); }
  Stats* stats() { return stats_.get(); }

  static Window* Get(GLFWwindow* window);

  void SetDelegate(Delegate* delegate) { delegate_ = delegate; }

  void OnLoadingStart();
  void OnLoadingFinished();

  void SetWindowCanvas(RenderCanvas* canvas);

  void RenderAndSwapBuffers();

  bool wants_frames() const { return block_visibility_for_n_frames_ > 0; }

  bool visible() const { return visible_; }
  void SetVisible(bool visible);

  bool focused() const;
  void Focus();
  void RequestAttention();

  bool maximized() const;
  bool minimized() const;
  void Maximize();
  void Minimize();
  void Restore();

  bool decorated() const;
  void SetDecorated(bool decorated);

  const std::string& title() const { return title_; }
  void SetTitle(std::string title);

  void SetIcons(std::vector<GLFWimage> icons);
  void SetCursor(GLFWcursor* cursor);

  int width() const { return width_; }
  int height() const { return height_; }
  void SetWidth(int width);
  void SetHeight(int height);
  float retina_scale() const { return retina_scale_; }

  bool keep_aspect_ratio() const { return keep_aspect_ratio_; }
  void SetKeepAspectRatio(bool keep);

  bool resizable() const;
  void SetResizable(bool resizable);

  bool always_on_top() const;
  void SetAlwaysOnTop(bool always_on_top);

  bool fullscreen() const;
  void SetFullscreen(bool fullscreen);

  bool vsync() const;
  void SetVsync(bool wait_for_vsync);

  int frame_left() const;
  int frame_right() const;
  int frame_top() const;
  int frame_bottom() const;
  int avail_width() const;
  int avail_height() const;
  int screen_width() const;
  int screen_height() const;
  float device_pixel_ratio() const;

  int x() const;
  int y() const;
  void SetX(int x);
  void SetY(int y);

 private:
  void OnResize(int width, int height);

  static Delegate* GetDelegate(GLFWwindow* window);

  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                          int mods);
  static void CharCallback(GLFWwindow* window, unsigned int codepoint);
  static void CursorPosCallback(GLFWwindow* window, double x, double y);
  static void MouseButtonCallback(GLFWwindow* window, int button, int action,
                                  int mods);
  static void ScrollCallback(GLFWwindow* window, double x, double y);
  static void CursorEnterCallback(GLFWwindow* window, int entered);
  static void ResizeCallback(GLFWwindow* window, int width, int height);
  static void FocusCallback(GLFWwindow* window, int focused);
  static void CloseCallback(GLFWwindow* window);
  static void DropCallback(GLFWwindow* window, int count, const char** paths);
  static void IconifyCallback(GLFWwindow* window, int iconified);
  static void MaximizeCallback(GLFWwindow* window, int maximized);

  Delegate* delegate_;
  GLFWwindow* window_;
  std::string title_;

  // Width and height in pixel sizes; this matches the OpenGL sizes.
  // On Retina displays, the window sizes might be different from these
  // sizes; retina_scale_ indicates the scaling factor.
  // Window.js exposes all sizes in pixels units to the application.
  int width_;
  int height_;
  float retina_scale_;

  bool visible_;
  bool vsync_;
  bool keep_aspect_ratio_;
  bool loading_;
  bool reloading_;
  int block_visibility_for_n_frames_;
  int post_resize_count_;

  std::unique_ptr<RenderCanvasSharedContext> shared_context_;

  // The canvas that renders to the screen.
  std::unique_ptr<RenderCanvas> screen_canvas_;

  // The canvas that backs window.canvas in Javascript.
  RenderCanvas* canvas_;

  std::unique_ptr<ConsoleOverlay> console_overlay_;
  std::unique_ptr<Stats> stats_;
};

#endif  // WINDOWJS_WINDOW_H
