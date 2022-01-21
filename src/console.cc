#include "console.h"

#include <iostream>
#include <thread>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <v8/src/debug/debug-interface.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>

#include "args.h"
#include "css.h"
#include "fail.h"
#include "js.h"
#include "platform.h"
#include "render_canvas.h"
#include "window.h"

#if defined(WINDOWJS_WIN)
#include <iostream>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#endif

// Forwards logs from v8 to ProcessConsole.
class ConsoleDelegate : public v8::debug::ConsoleDelegate {
 public:
  explicit ConsoleDelegate(Js* js) : js_(js) {}

  void Clear(const v8::debug::ConsoleCallArguments& args,
             const v8::debug::ConsoleContext& context) override {
    ClearLogs();
  }

  void Debug(const v8::debug::ConsoleCallArguments& args,
             const v8::debug::ConsoleContext& context) override {
    ::Log(GetConsoleOutput(args), ConsoleLogLevel::CONSOLE_DEBUG);
  }

  void Log(const v8::debug::ConsoleCallArguments& args,
           const v8::debug::ConsoleContext& context) override {
    ::Log(GetConsoleOutput(args), ConsoleLogLevel::CONSOLE_LOG);
  }

  void Info(const v8::debug::ConsoleCallArguments& args,
            const v8::debug::ConsoleContext& context) override {
    ::Log(GetConsoleOutput(args), ConsoleLogLevel::CONSOLE_INFO);
  }

  void Warn(const v8::debug::ConsoleCallArguments& args,
            const v8::debug::ConsoleContext& context) override {
    ::Log(GetConsoleOutput(args), ConsoleLogLevel::CONSOLE_WARN);
  }

  void Error(const v8::debug::ConsoleCallArguments& args,
             const v8::debug::ConsoleContext& context) override {
    ::Log(GetConsoleOutput(args), ConsoleLogLevel::CONSOLE_ERROR);
  }

  void Dir(const v8::debug::ConsoleCallArguments& args,
           const v8::debug::ConsoleContext& console_context) override {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    std::stringstream ss;

    v8::Local<v8::Value> arg =
        args.Length() == 0 ? context->Global().As<v8::Value>() : args[0];

    std::string type = js_->ToString(arg->TypeOf(isolate));

    if (arg->IsArray()) {
      ss << "Array length: " << arg.As<v8::Array>()->Length() << "\n";
    } else if (arg->IsFunction()) {
      ss << "Function\n";
    } else if (arg->IsObject()) {
      ss << "Object\n";
      auto object = arg.As<v8::Object>();
      v8::MaybeLocal<v8::Array> maybe_props =
          object->GetOwnPropertyNames(context, v8::ALL_PROPERTIES);
      if (!maybe_props.IsEmpty()) {
        v8::Local<v8::Array> props = maybe_props.ToLocalChecked();
        std::map<std::string, std::string> map;
        for (unsigned j = 0; j < props->Length(); j++) {
          v8::Local<v8::Value> prop = props->Get(context, j).ToLocalChecked();
          std::string key = js_->ToString(prop);
          if (key.empty()) {
            continue;
          }
          auto maybe_value = object->Get(context, prop);
          if (!maybe_value.IsEmpty()) {
            v8::Local<v8::Value> value = maybe_value.ToLocalChecked();
            if (value->IsNull() || value->IsUndefined() || value->IsBoolean() ||
                value->IsNumber()) {
              map[key] = js_->ToString(value);
            } else if (value->IsString()) {
              std::string quote = "\"";
              map[key] = quote + js_->ToString(value) + quote;
            } else if (value->IsFunction()) {
              map[key] = "function";
            } else {
              map[key] = js_->ToString(value->TypeOf(isolate));
            }
          }
        }
        for (auto it = map.begin(); it != map.end(); ++it) {
          ss << "    " << it->first << ": " << it->second << "\n";
        }
      }
    } else {
      ss << type << "\n";
    }

    std::string out = ss.str();
    if (!out.empty() && out.back() == '\n') {
      out.pop_back();
    }

    ::Log(std::move(out), ConsoleLogLevel::CONSOLE_LOG);
  }

  std::string GetConsoleOutput(const v8::debug::ConsoleCallArguments& args) {
    std::stringstream ss;
    for (int i = 0; i < args.Length(); i++) {
      if (i > 0) {
        ss << " ";
      }
      ss << js_->ToString(args[i]);
    }
    return ss.str();
  }

  Js* js_;
};

const char* ConsoleLogLevelToString(ConsoleLogLevel level) {
  switch (level) {
    case ConsoleLogLevel::CONSOLE_DEBUG: return "debug";
    case ConsoleLogLevel::CONSOLE_LOG: return "log";
    case ConsoleLogLevel::CONSOLE_INFO: return "info";
    case ConsoleLogLevel::CONSOLE_WARN: return "warn";
    case ConsoleLogLevel::CONSOLE_ERROR: return "error";
    case ConsoleLogLevel::CONSOLE_DEV: return "dev";
  }
  ASSERT(false);
  return nullptr;
}

struct LogStore {
  using Log = std::pair<std::string, ConsoleLogLevel>;
  std::mutex lock;
  std::deque<Log> logs;
  LogHandler* handler = nullptr;
  bool in_handler = false;
};

static LogStore* g_log_store = nullptr;

void InitLog() {
  ASSERT(!g_log_store);
  g_log_store = new LogStore;

#if defined(WINDOWJS_WIN) && defined(WINDOWJS_RELEASE_BUILD)
  // Window.js is not a console application (see the /subsystem:windows flag
  // in src/CMakeLists.txt). This code redirects stdin/stdout/stderr to the
  // parent process console, if it's available, so that logging prints to the
  // windows terminal instead.
  // TODO: this works but doesn't update the cursor position. It is left at
  // its starting position when the process terminates, which is annoying.
  if (!Args().is_child_process && Args().log) {
    if (_get_osfhandle(0) < 0) {
      _close(0);
    }
    freopen("//./NUL", "r", stdin);
    setvbuf(stdin, NULL, _IONBF, 0);
    if (_get_osfhandle(1) < 0) {
      _close(1);
    }
    freopen("//./NUL", "w", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);
    if (_get_osfhandle(2) < 0) {
      _close(2);
    }
    freopen("//./NUL", "w", stderr);
    setvbuf(stderr, NULL, _IONBF, 0);

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
      HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
      int fd = _open_osfhandle((intptr_t) handle, _O_TEXT);
      _dup2(fd, _fileno(stdin));
      SetStdHandle(STD_INPUT_HANDLE, (HANDLE) _get_osfhandle(_fileno(stdin)));
      _close(fd);

      handle = GetStdHandle(STD_OUTPUT_HANDLE);
      fd = _open_osfhandle((intptr_t) handle, _O_TEXT);
      _dup2(fd, _fileno(stdout));
      SetStdHandle(STD_OUTPUT_HANDLE, (HANDLE) _get_osfhandle(_fileno(stdout)));
      _close(fd);

      handle = GetStdHandle(STD_ERROR_HANDLE);
      fd = _open_osfhandle((intptr_t) handle, _O_TEXT);
      _dup2(fd, _fileno(stderr));
      SetStdHandle(STD_ERROR_HANDLE, (HANDLE) _get_osfhandle(_fileno(stderr)));
      _close(fd);
    }
  }
#endif
}

void ShutdownLog() {
  ASSERT(g_log_store);
  delete g_log_store;
  g_log_store = nullptr;
}

void Log(std::string message, ConsoleLogLevel level) {
  if (Args().log) {
    std::cerr << "[" << ConsoleLogLevelToString(level) << "] " << message
              << "\n";
  }

  std::lock_guard lock(g_log_store->lock);

  ASSERT(!g_log_store->in_handler);

  if (g_log_store->handler) {
    g_log_store->in_handler = true;
    g_log_store->handler->OnLog(std::move(message), level);
    g_log_store->in_handler = false;
  } else {
    while (g_log_store->logs.size() > 1000) {
      g_log_store->logs.pop_front();
    }
    g_log_store->logs.emplace_back(std::move(message), level);
  }
}

void ClearLogs() {
  std::lock_guard lock(g_log_store->lock);
  if (g_log_store->handler) {
    g_log_store->handler->OnClearLogs();
  }
}

void SetLogHandler(LogHandler* handler) {
  std::lock_guard lock(g_log_store->lock);
  g_log_store->handler = handler;
  if (handler) {
    g_log_store->in_handler = true;
    for (auto& p : g_log_store->logs) {
      handler->OnLog(std::move(p.first), p.second);
    }
    g_log_store->logs.clear();
    g_log_store->in_handler = false;
  }
}

ConsoleOverlay::ConsoleOverlay(Window* window)
    : window_(window),
      text_color_(SK_ColorWHITE),
      redraw_(false),
      enable_on_errors_(true) {}

ConsoleOverlay::~ConsoleOverlay() {}

int ConsoleOverlay::width() const {
  return 512 * window_->device_pixel_ratio();
}

int ConsoleOverlay::height() const {
  return 144 * window_->device_pixel_ratio();
}

GLuint ConsoleOverlay::texture() {
  return canvas_->GetTextureForDraw();
}

void ConsoleOverlay::SetEnabled(bool enabled) {
  if (enabled) {
    if (!canvas_) {
      canvas_.reset(
          new RenderCanvas(window_->shared_context(), width(), height()));
      redraw_ = true;
    }
  } else {
    canvas_.reset();
  }
}

void ConsoleOverlay::SetEnableOnErrors(bool enable) {
  enable_on_errors_ = enable;
}

void ConsoleOverlay::OnLog(std::string message, ConsoleLogLevel level) {
  static const SkColor debug = SkColorSetRGB(0x80, 0x80, 0x80);
  static const SkColor info = SkColorSetRGB(0x00, 0xff, 0xff);
  static const SkColor warn = SkColorSetRGB(0xff, 0xa5, 0x00);
  static const SkColor error = SkColorSetRGB(0xff, 0x00, 0x00);
  static const SkColor dev = SkColorSetRGB(0xff, 0x00, 0xff);

  while (messages_.size() >= 10) {
    messages_.pop_front();
  }
  SkColor color = text_color_;
  if (level == ConsoleLogLevel::CONSOLE_DEBUG) {
    color = debug;
  } else if (level == ConsoleLogLevel::CONSOLE_INFO) {
    color = info;
  } else if (level == ConsoleLogLevel::CONSOLE_WARN) {
    color = warn;
  } else if (level == ConsoleLogLevel::CONSOLE_ERROR) {
    color = error;
  } else if (level == ConsoleLogLevel::CONSOLE_DEV) {
    color = dev;
  }
  messages_.emplace_back(std::move(message), color);

  redraw_ = true;

  if (level == ConsoleLogLevel::CONSOLE_ERROR && enable_on_errors_) {
    SetEnabled(true);
  }
}

void ConsoleOverlay::Clear() {
  if (!messages_.empty()) {
    messages_.clear();
    redraw_ = true;
  }
}

void ConsoleOverlay::Draw() {
  ASSERT(is_enabled());

  if (!redraw_) {
    return;
  }

  canvas_->SetCurrentContext();
  SkCanvas* canvas = canvas_->canvas();
  canvas->clear(SkColorSetARGB(0x80, 0x00, 0x00, 0x00));

  const float ratio = window_->device_pixel_ratio();

  SkFont font;
  CSSFontToSkFont("11px monospace", &font);
  font.setSize(11 * ratio);
  font.setScaleX(1.1);
  font.setSubpixel(true);
  font.setHinting(SkFontHinting::kFull);

  SkPaint paint;
  paint.setStyle(SkPaint::kFill_Style);

  std::vector<std::string_view> lines;
  lines.reserve(10);
  std::vector<SkColor> colors;
  colors.reserve(10);
  for (const auto& p : messages_) {
    std::string_view line = p.first;
    size_t k = line.find('\n');
    while (k != std::string_view::npos) {
      lines.push_back(line.substr(0, k));
      colors.push_back(p.second);
      line.remove_prefix(k + 1);
      k = line.find('\n');
    }
    lines.push_back(line);
    colors.push_back(p.second);
  }

  int count = std::min((int) lines.size(), 10);
  int y = height() - 6 * ratio;

  for (int i = 0; i < count; i++) {
    paint.setColor(colors[colors.size() - 1 - i]);
    std::string text{lines[lines.size() - 1 - i]};
    canvas->drawSimpleText(text.c_str(), text.size(), SkTextEncoding::kUTF8, 4,
                           y, font, paint);
    y -= 14 * ratio;
  }

  redraw_ = false;
}

std::unique_ptr<v8::debug::ConsoleDelegate> MakeConsoleDelegate(Js* js) {
  return std::make_unique<ConsoleDelegate>(js);
}
