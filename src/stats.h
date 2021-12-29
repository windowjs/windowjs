#ifndef WINDOWJS_STATS_H
#define WINDOWJS_STATS_H

#include <memory>

#include <glad/glad.h>

#include "js.h"
#include "render_canvas.h"

class JsApi;
class Window;

class Stats {
 public:
  explicit Stats(Window* window);
  ~Stats();

  bool is_enabled() const { return canvas_ != nullptr; }
  bool is_print_frame_times() const { return print_frame_times_; }

  int width() const;
  int height() const;
  GLuint texture() { return canvas_->GetTextureForDraw(); }

  void SetEnabled(bool enabled);
  void SetPrintFrameTimes(bool print) { print_frame_times_ = print; }
  void SetJs(Js* js, JsApi* api) {
    js_ = js;
    api_ = api;
  }

  void Reset();

  void OnGcFinished() { UpdateTimestamp(&elapsed_gc_); }
  void OnJsFinished() { UpdateTimestamp(&elapsed_js_); }
  void OnRafFinished() { UpdateTimestamp(&elapsed_raf_); }
  void OnSwapFinished() { UpdateTimestamp(&elapsed_swap_); }
  void OnWaitFinished() { UpdateTimestamp(&elapsed_wait_); }

  void OnFrameFinished();

  void Draw();

 private:
  void UpdateTimestamp(double* timestamp);
  void PrintFrameTimes(double elapsed);

  Window* window_;
  Js* js_;
  JsApi* api_;
  std::unique_ptr<RenderCanvas> canvas_;

  int frames_count_;
  double last_stats_update_;
  double fps_;

  double previous_timestamp_;
  double frame_start_timestamp_;
  double elapsed_wait_;
  double elapsed_gc_;
  double elapsed_js_;
  double elapsed_raf_;
  double elapsed_swap_;

  bool redraw_;
  bool print_frame_times_;
};

#endif  // WINDOWJS_STATS_H
