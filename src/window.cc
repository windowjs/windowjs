#include "window.h"

#include "args.h"
#include "fail.h"
#include "platform.h"

// Forward declared because <EGL/egl.h> includes X11 headers on Linux,
// which have a Window struct that clashes with our Window class.
extern "C" unsigned int eglWaitClient();

// static
void Window::Init() {
  // See Main::OnResize. This hint makes GLFW pump the main event loop during
  // resizes, using a timer internally.
  glfwInitHint(GLFW_WIN32_MESSAGES_IN_FIBER, GLFW_TRUE);

  ASSERT(glfwInit());
}

// static
void Window::Shutdown() {
  glfwTerminate();
}

Window::Window(Delegate* delegate, int width, int height)
    : delegate_(delegate),
      width_(width),
      height_(height),
      retina_scale_(1.0f),
      visible_(true),
      vsync_(true),
      keep_aspect_ratio_(false),
      loading_(true),
      reloading_(false),
      block_visibility_for_n_frames_(1),
      post_resize_count_(0),
      canvas_(nullptr),
      console_overlay_(new ConsoleOverlay(this)),
      stats_(new Stats(this)) {
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RED_BITS, 8);
  glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] create window start: " << glfwGetTime();
  }

  window_ = glfwCreateWindow(width, height, "window.js", nullptr, nullptr);

  if (!window_) {
    const char* ptr = nullptr;
    int err = glfwGetError(&ptr);
    Fail("glfwCreateWindow failed (%d): %s\n", err, ptr);
  }

  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] create window end: " << glfwGetTime();
  }

#if defined(WINDOWJS_WIN)
  // TODO: on Windows, the first window paint sometimes flashes white before
  // showing the first frame. This hint prevents that from happening but isn't
  // a proper fix; figure a better solution.
  // glfwWindowHint(GLFW_DECORATED , GL_FALSE);
  // These hacks makes the flicker appear less often.
  block_visibility_for_n_frames_ = 2;
  glfwFocusWindow(window_);
  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] focusing window hack: " << glfwGetTime();
  }
#endif

  glfwSetWindowUserPointer(window_, this);

  glfwSetKeyCallback(window_, KeyCallback);
  glfwSetCharCallback(window_, CharCallback);
  glfwSetCursorPosCallback(window_, CursorPosCallback);
  glfwSetMouseButtonCallback(window_, MouseButtonCallback);
  glfwSetScrollCallback(window_, ScrollCallback);
  glfwSetCursorEnterCallback(window_, CursorEnterCallback);
  glfwSetFramebufferSizeCallback(window_, ResizeCallback);
  glfwSetWindowFocusCallback(window_, FocusCallback);
  glfwSetWindowCloseCallback(window_, CloseCallback);
  glfwSetDropCallback(window_, DropCallback);
  glfwSetWindowIconifyCallback(window_, IconifyCallback);
  glfwSetWindowMaximizeCallback(window_, MaximizeCallback);

  // Skia crashes if it gets zero width of height; force a min size of 1.
  glfwSetWindowSizeLimits(window_, 1, 1, GLFW_DONT_CARE, GLFW_DONT_CARE);

  glfwMakeContextCurrent(window_);

  // Enable waiting for vsync.
  glfwSwapInterval(1);

  int window_width;
  glfwGetFramebufferSize(window_, &width_, &height_);
  glfwGetWindowSize(window_, &window_width, nullptr);
  retina_scale_ = (float) width_ / window_width;

  glViewport(0, 0, width_, height_);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  ASSERT_NO_GL_ERROR();

  if (Args().profile_startup) {
    $(DEV) << "[profile-startup] Created texture shader: " << glfwGetTime();
  }

  shared_context_.reset(new RenderCanvasSharedContext(this));
  screen_canvas_.reset(new RenderCanvas(shared_context_.get(), width_, height_,
                                        RenderCanvas::FRAMEBUFFER_0));
}

Window::~Window() {
  screen_canvas_.reset();
  console_overlay_.reset();
  stats_.reset();
  shared_context_.reset();
  glfwMakeContextCurrent(nullptr);
  glfwDestroyWindow(window_);
}

void Window::OnLoadingStart() {
  if (loading_) {
    // Initial loading.
    return;
  }
  // Reloading.
  loading_ = true;
  reloading_ = true;
  canvas_->Reset();
}

void Window::OnLoadingFinished() {
  ASSERT(loading_);

  loading_ = false;
  reloading_ = false;

  int width = 0;
  int height = 0;
  glfwGetWindowSize(window_, &width, &height);
  width *= retina_scale_;
  height *= retina_scale_;
  if (width != width_ || height != height_) {
    glfwSetWindowSize(window_, width_, height_);
  }
}

void Window::SetWindowCanvas(RenderCanvas* canvas) {
  if (canvas) {
    ASSERT(canvas_ == nullptr);
    canvas_ = canvas;
  } else {
    canvas_ = nullptr;
  }
}

void Window::RenderAndSwapBuffers() {
  if (!canvas_) {
    return;
  }

  // Clear the screen, in case it was resized or swapped and not cleared yet.
  screen_canvas_->canvas()->clear(SK_ColorTRANSPARENT);

  // Blit the main content.
  canvas_->surface()->draw(screen_canvas_->canvas(), 0, 0);

  if (console_overlay_->is_enabled()) {
    console_overlay_->Draw();
    console_overlay_->canvas()->surface()->draw(
        screen_canvas_->canvas(), 0,
        screen_canvas_->height() - console_overlay_->canvas()->height());
  }

  if (stats_->is_enabled()) {
    stats_->Draw();
    stats_->canvas()->surface()->draw(screen_canvas_->canvas(), 0, 0);
  }

  // Make sure that all Skia operations are sent to the GPU before swapping.
  shared_context_->Flush();

  glfwSwapBuffers(window_);

  if (block_visibility_for_n_frames_ > 0) {
    if (Args().profile_startup) {
      $(DEV) << "[profile-startup] swap buffers: " << glfwGetTime();
    }
    block_visibility_for_n_frames_--;
    if (block_visibility_for_n_frames_ == 0 && visible_) {
      SetVisible(true);
      if (Args().profile_startup) {
        $(DEV) << "[profile-startup] set visible and finish: " << glfwGetTime();
      }
    } else if (Args().profile_startup) {
      $(DEV) << "[profile-startup] skipping a frame on startup... "
             << glfwGetTime();
    }
  }

  stats_->OnFrameFinished();

  if (post_resize_count_ > 0) {
    post_resize_count_--;
    if (post_resize_count_ == 0) {
      glfwSwapInterval(vsync_ ? 1 : 0);
    }
    //eglWaitClient();
  }
}

void Window::SetVisible(bool visible) {
  visible_ = visible;
  if (visible_ && block_visibility_for_n_frames_ == 0) {
    glfwShowWindow(window_);
  } else {
    glfwHideWindow(window_);
  }
}

bool Window::focused() const {
  return glfwGetWindowAttrib(window_, GLFW_FOCUSED);
}

void Window::Focus() {
  glfwFocusWindow(window_);
}

void Window::RequestAttention() {
  glfwRequestWindowAttention(window_);
}

bool Window::maximized() const {
  return glfwGetWindowAttrib(window_, GLFW_MAXIMIZED);
}

bool Window::minimized() const {
  return glfwGetWindowAttrib(window_, GLFW_ICONIFIED);
}

void Window::Maximize() {
  glfwMaximizeWindow(window_);
}

void Window::Minimize() {
  glfwIconifyWindow(window_);
}

void Window::Restore() {
  glfwRestoreWindow(window_);
}

bool Window::decorated() const {
  return glfwGetWindowAttrib(window_, GLFW_DECORATED);
}

void Window::SetDecorated(bool decorated) {
  glfwSetWindowAttrib(window_, GLFW_DECORATED,
                      decorated ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SetTitle(std::string title) {
  glfwSetWindowTitle(window_, title.c_str());
  title_ = title;
  delegate_->OnTitleChanged();
}

void Window::SetIcons(std::vector<GLFWimage> icons) {
  glfwSetWindowIcon(window_, icons.size(), icons.data());
}

void Window::SetCursor(GLFWcursor* cursor) {
  glfwSetCursor(window_, cursor);
}

void Window::SetWidth(int width) {
  if (loading_) {
    if (!reloading_) {
      OnResize(width, height_);
    }
  } else {
    glfwSetWindowSize(window_, width / retina_scale_, height_ / retina_scale_);
  }
}

void Window::SetHeight(int height) {
  if (loading_) {
    if (!reloading_) {
      OnResize(width_, height);
    }
  } else {
    glfwSetWindowSize(window_, width_ / retina_scale_, height / retina_scale_);
  }
}

void Window::SetKeepAspectRatio(bool keep) {
  keep_aspect_ratio_ = keep;
  if (keep) {
    if (loading_) {
      // Make sure that the window size reflects its intended size before
      // forcing the aspect ratio, otherwise it will resize to unexpected sizes.
      glfwSetWindowSize(window_, width_ / retina_scale_,
                        height_ / retina_scale_);
    }
    if (!reloading_) {
      glfwSetWindowAspectRatio(window_, width_, height_);
    }
  } else {
    glfwSetWindowAspectRatio(window_, GLFW_DONT_CARE, GLFW_DONT_CARE);
  }
}

bool Window::resizable() const {
  return glfwGetWindowAttrib(window_, GLFW_RESIZABLE) == GLFW_TRUE;
}

void Window::SetResizable(bool resizable) {
  glfwSetWindowAttrib(window_, GLFW_RESIZABLE,
                      resizable ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::always_on_top() const {
  return glfwGetWindowAttrib(window_, GLFW_FLOATING) == GLFW_TRUE;
}

void Window::SetAlwaysOnTop(bool always_on_top) {
  glfwSetWindowAttrib(window_, GLFW_FLOATING,
                      always_on_top ? GLFW_TRUE : GLFW_FALSE);
}

bool Window::fullscreen() const {
  return glfwGetWindowMonitor(window_) != nullptr;
}

void Window::SetFullscreen(bool fullscreen) {
  if (fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window_, monitor, 0, 0, mode->width, mode->height,
                         mode->refreshRate);
  } else {
    int w = width_;
    int h = height_;
    glfwSetWindowMonitor(window_, nullptr, 0, 0, w, h, GLFW_DONT_CARE);
  }
}

bool Window::vsync() const {
  return vsync_;
}

void Window::SetVsync(bool wait_for_vsync) {
  vsync_ = wait_for_vsync;
  glfwSwapInterval(wait_for_vsync ? 1 : 0);
}

int Window::frame_left() const {
  int value;
  glfwGetWindowFrameSize(window_, &value, nullptr, nullptr, nullptr);
  value *= retina_scale_;
  return value;
}

int Window::frame_right() const {
  int value;
  glfwGetWindowFrameSize(window_, nullptr, nullptr, &value, nullptr);
  value *= retina_scale_;
  return value;
}

int Window::frame_top() const {
  int value;
  glfwGetWindowFrameSize(window_, nullptr, &value, nullptr, nullptr);
  value *= retina_scale_;
  return value;
}

int Window::frame_bottom() const {
  int value;
  glfwGetWindowFrameSize(window_, nullptr, nullptr, nullptr, &value);
  value *= retina_scale_;
  return value;
}

int Window::avail_width() const {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  ASSERT(monitor);
  int value;
  glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &value, nullptr);
  value *= retina_scale_;
  return value;
}

int Window::avail_height() const {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  ASSERT(monitor);
  int value;
  glfwGetMonitorWorkarea(monitor, nullptr, nullptr, nullptr, &value);
  value *= retina_scale_;
  return value;
}

int Window::screen_width() const {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  ASSERT(monitor);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  ASSERT(mode);
  return mode->width * retina_scale_;
}

int Window::screen_height() const {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  ASSERT(monitor);
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  ASSERT(mode);
  return mode->height * retina_scale_;
}

float Window::device_pixel_ratio() const {
  float value;
  glfwGetWindowContentScale(window_, &value, nullptr);
  return value;
}

int Window::x() const {
  int value;
  glfwGetWindowPos(window_, &value, nullptr);
  return value;
}

int Window::y() const {
  int value;
  glfwGetWindowPos(window_, nullptr, &value);
  return value;
}

void Window::SetX(int x) {
  glfwSetWindowPos(window_, x, y());
}

void Window::SetY(int y) {
  glfwSetWindowPos(window_, x(), y);
}

void Window::OnResize(int width, int height) {
  width_ = width;
  height_ = height;
  ASSERT_NO_GL_ERROR();
  if (canvas_ && (width != canvas_->width() || height != canvas_->height())) {
    canvas_->Resize(width, height);
    screen_canvas_->Resize(width, height);
    ASSERT_NO_GL_ERROR();
  }
}

// static
Window* Window::Get(GLFWwindow* window) {
  return static_cast<Window*>(glfwGetWindowUserPointer(window));
}

// static
Window::Delegate* Window::GetDelegate(GLFWwindow* window) {
  return Get(window)->delegate_;
}

// static
void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action,
                         int mods) {
  GetDelegate(window)->OnKey(key, scancode, action, mods);
}

// static
void Window::CharCallback(GLFWwindow* window, unsigned int codepoint) {
  GetDelegate(window)->OnCharacter(codepoint);
}

// static
void Window::CursorPosCallback(GLFWwindow* window, double x, double y) {
  Window* w = Get(window);
  x *= w->retina_scale_;
  y *= w->retina_scale_;
  w->delegate_->OnMouseMove(x, y);
}

// static
void Window::MouseButtonCallback(GLFWwindow* window, int button, int action,
                                 int mods) {
  double x = 0;
  double y = 0;
  glfwGetCursorPos(window, &x, &y);
  Window* w = Get(window);
  x *= w->retina_scale_;
  y *= w->retina_scale_;
  w->delegate_->OnMouseButton(button, action == GLFW_PRESS, x, y);
}

// static
void Window::ScrollCallback(GLFWwindow* window, double x, double y) {
  GetDelegate(window)->OnMouseWheel(x, y);
}

// static
void Window::CursorEnterCallback(GLFWwindow* window, int entered) {
  GetDelegate(window)->OnMouseEnter(entered != 0);
}

// static
void Window::ResizeCallback(GLFWwindow* window, int width, int height) {
  // Note: this is the callback for _framebuffer_ resizes.
  // The width and height are already scaled appropriately.

  Window* w = Get(window);
  if (width == 0 && height == 0 && w->minimized()) {
    return;
  }

  if (!w->loading_) {
    //glfwSwapInterval(0);
    // 10 still glitches occasionally.
    // 100 seems good.
    //w->post_resize_count_ = 3;
  }

  if (width != w->width_ || height != w->height_) {
    w->OnResize(width, height);

    if (!w->loading_) {
      // This call makes the Javascript callbacks see the updated sizes.
      w->shared_context_->Flush();
      eglWaitClient();

      w->delegate_->OnResize(width, height);
    }
  }
}

// static
void Window::FocusCallback(GLFWwindow* window, int focused) {
  GetDelegate(window)->OnFocus(focused != 0);
}

// static
void Window::CloseCallback(GLFWwindow* window) {
  GetDelegate(window)->OnClose();
}

// static
void Window::DropCallback(GLFWwindow* window, int count, const char** paths) {
  std::vector<std::string> list;
  list.resize(count);
  for (int i = 0; i < count; i++) {
    list[i] = paths[i];
  }
  GetDelegate(window)->OnDrop(std::move(list));
}

// static
void Window::IconifyCallback(GLFWwindow* window, int iconified) {
  if (iconified) {
    GetDelegate(window)->OnMinimize();
  } else {
    GetDelegate(window)->OnRestore();
  }
}

// static
void Window::MaximizeCallback(GLFWwindow* window, int maximized) {
  if (maximized) {
    GetDelegate(window)->OnMaximize();
  } else {
    GetDelegate(window)->OnRestore();
  }
}
