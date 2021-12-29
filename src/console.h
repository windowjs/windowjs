#ifndef WINDOWJS_CONSOLE_H
#define WINDOWJS_CONSOLE_H

#include <deque>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <skia/include/core/SkColor.h>
#include <v8/include/v8.h>

#define $(level) ConsoleLogHelper(::ConsoleLogLevel::CONSOLE_##level).ss()

enum class ConsoleLogLevel {
  CONSOLE_DEBUG,
  CONSOLE_LOG,
  CONSOLE_INFO,
  CONSOLE_WARN,
  CONSOLE_ERROR,
  // Log level for developer hints and internals:
  CONSOLE_DEV,
};

const char* ConsoleLogLevelToString(ConsoleLogLevel level);

class LogHandler {
 public:
  virtual ~LogHandler() {}

  virtual void OnLog(std::string message, ConsoleLogLevel level) = 0;
  virtual void OnClearLogs() = 0;
};

// Can be called from any thread.
void Log(std::string message, ConsoleLogLevel level);
void ClearLogs();

void InitLog();
void ShutdownLog();
void SetLogHandler(LogHandler* handler);

class ConsoleLogHelper {
 public:
  explicit ConsoleLogHelper(ConsoleLogLevel level) : level_(level) {}

  ~ConsoleLogHelper() { Log(ss_.str(), level_); }

  std::stringstream& ss() { return ss_; }

 private:
  std::stringstream ss_;
  ConsoleLogLevel level_;
};

class RenderCanvas;
class Window;

class ConsoleOverlay {
 public:
  explicit ConsoleOverlay(Window* window);
  ~ConsoleOverlay();

  bool is_enabled() const { return canvas_ != nullptr; }
  bool is_enable_on_errors() const { return enable_on_errors_; }

  SkColor text_color() const { return text_color_; }
  void SetTextColor(SkColor color) { text_color_ = color; }

  int width() const;
  int height() const;
  GLuint texture();

  void SetEnabled(bool enabled);
  void SetEnableOnErrors(bool enable);

  void OnLog(std::string message, ConsoleLogLevel level);
  void Clear();

  void Draw();

 private:
  Window* window_;
  std::unique_ptr<RenderCanvas> canvas_;
  std::deque<std::pair<std::string, SkColor>> messages_;
  SkColor text_color_;
  bool redraw_;
  bool enable_on_errors_;
};

namespace v8::debug {
class ConsoleDelegate;
}

class Js;

std::unique_ptr<v8::debug::ConsoleDelegate> MakeConsoleDelegate(Js* js);

#endif  // WINDOWJS_CONSOLE_H
