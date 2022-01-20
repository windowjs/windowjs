#include "stats.h"

#include <sstream>

#include <uv.h>

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>

#include "console.h"
#include "css.h"
#include "fail.h"
#include "js_api.h"
#include "platform.h"
#include "window.h"

namespace {

uv_rusage_t g_prev_rusage;

}  // namespace

double DeltaTimevalMs(uv_timeval_t before, uv_timeval_t after) {
  double b = before.tv_sec * 1000 + before.tv_usec / 1000;
  double a = after.tv_sec * 1000 + after.tv_usec / 1000;
  return a - b;
}

Stats::Stats(Window* window)
    : window_(window),
      js_(nullptr),
      frames_count_(0),
      last_stats_update_(-1),
      fps_(0),
      redraw_(false),
      print_frame_times_(false) {}

Stats::~Stats() {}

int Stats::width() const {
  return 160 * window_->device_pixel_ratio();
}

int Stats::height() const {
  return 76 * window_->device_pixel_ratio();
}

void Stats::SetEnabled(bool enabled) {
  if (enabled) {
    if (!canvas_) {
      canvas_.reset(new RenderCanvas(window_->shared_context(), width(),
                                     height(), RenderCanvas::TEXTURE));
      redraw_ = true;
    }
  } else {
    canvas_.reset();
  }
}

void Stats::Reset() {
  frames_count_ = 0;
  last_stats_update_ = -1;
  fps_ = 0;
  redraw_ = true;
}

void Stats::UpdateTimestamp(double* timestamp) {
  double now = glfwGetTime();
  *timestamp = now - previous_timestamp_;
  previous_timestamp_ = now;
}

void Stats::OnFrameFinished() {
  if (print_frame_times_) {
    double now = glfwGetTime();
    double elapsed = now - frame_start_timestamp_;
    frame_start_timestamp_ = now;

    double fps = 1 / elapsed;
    $(DEV) << "Input " << std::fixed << std::setprecision(3)
           << elapsed_wait_ * 1000 << " GC " << elapsed_gc_ * 1000 << " JS "
           << elapsed_js_ * 1000 << " RAF " << elapsed_raf_ * 1000 << " Swap "
           << elapsed_swap_ * 1000 << " Total frame " << elapsed * 1000
           << " FPS " << fps;
  }

  if (!is_enabled()) {
    return;
  }

  frames_count_++;
  double now = glfwGetTime();
  if (now >= last_stats_update_ + 0.5) {
    double elapsed = now - last_stats_update_;
    last_stats_update_ = now;
    fps_ = frames_count_ / elapsed;
    frames_count_ = 0;

    redraw_ = true;
  }
}

void Stats::Draw() {
  ASSERT(is_enabled());

  if (!redraw_) {
    return;
  }

  SkCanvas* canvas = canvas_->canvas();
  canvas->clear(SkColorSetARGB(0x80, 0x00, 0x00, 0x00));

  const float ratio = window_->device_pixel_ratio();

  SkFont font;
  CSSFontToSkFont("11px monospace", &font, api_->fonts(), api_->fonts_cache());
  font.setSize(11 * ratio);
  font.setScaleX(1.1);
  font.setSubpixel(true);
  font.setHinting(SkFontHinting::kFull);

  SkPaint paint;
  paint.setStyle(SkPaint::kFill_Style);

  double y = 14 * ratio;

  {
    std::stringstream ss;
    ss << "FPS " << std::fixed << std::setprecision(1) << fps_;
    std::string s = ss.str();

    paint.setColor(SK_ColorCYAN);
    canvas->drawSimpleText(s.c_str(), s.size(), SkTextEncoding::kUTF8, 4, y,
                           font, paint);

    y += 14 * ratio;
  }

  uv_rusage_t r;
  if (uv_getrusage(&r) == 0) {
    double utime = DeltaTimevalMs(g_prev_rusage.ru_utime, r.ru_utime);
    double stime = DeltaTimevalMs(g_prev_rusage.ru_stime, r.ru_stime);
    uint64_t rss = r.ru_maxrss;
#if defined(WINDOWJS_MAC)
    rss /= 1024;
#endif
    uint64_t in = r.ru_inblock;
    uint64_t din = in - g_prev_rusage.ru_inblock;
    uint64_t out = r.ru_oublock;
    uint64_t dout = out - g_prev_rusage.ru_oublock;

    {
      std::stringstream ss;
      ss << "User " << std::fixed << std::setprecision(1) << utime << " Sys "
         << stime;
      std::string s = ss.str();
      canvas->drawSimpleText(s.c_str(), s.size(), SkTextEncoding::kUTF8, 4, y,
                             font, paint);
      y += 14 * ratio;
    }

    {
      std::stringstream ss;
      ss << "RSS " << rss;
      std::string s = ss.str();
      canvas->drawSimpleText(s.c_str(), s.size(), SkTextEncoding::kUTF8, 4, y,
                             font, paint);
      y += 14 * ratio;
    }

    {
      std::stringstream ss;
      ss << "In " << din << " (" << in << ") Out " << dout << " (" << out
         << ")";
      std::string s = ss.str();
      canvas->drawSimpleText(s.c_str(), s.size(), SkTextEncoding::kUTF8, 4, y,
                             font, paint);
      y += 14 * ratio;
    }

    g_prev_rusage = r;
  }

  if (js_) {
    v8::HeapStatistics stats;
    js_->isolate()->GetHeapStatistics(&stats);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    constexpr double mb = 1024.0 * 1024.0;
    ss << "JS: " << stats.used_heap_size() / mb << "/"
       << stats.total_heap_size() / mb << " MiB";
    std::string s = ss.str();

    paint.setColor(SK_ColorGREEN);
    canvas->drawSimpleText(s.c_str(), s.size(), SkTextEncoding::kUTF8, 4, y,
                           font, paint);

    y += 14 * ratio;
  }

  redraw_ = false;
}
