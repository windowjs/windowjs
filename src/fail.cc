#include "fail.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <v8/src/base/debug/stack_trace.h>

#include "platform.h"
#include "version.h"

#if defined(WINDOWJS_WIN)
#include <windows.h>
#else
#include <signal.h>
#endif

static const char intro[] =
    "The program has crashed.\n\nPlease file a bug report at "
    "https://github.com/windowjs/windowjs/issues.\n\n";

#if defined(WINDOWJS_WIN)

static LONG OnUnhandledException(_EXCEPTION_POINTERS* ExceptionInfo) {
  std::string description = intro;

#if !defined(WINDOWJS_RELEASE_BUILD)
  description +=
      "Use Ctrl+C to copy the stack trace below and include it "
      "in the bug report.\n\n";
  description += v8::base::debug::StackTrace().ToString();
#endif

  description += "\n\n" + GetVersionString();

  UINT flags = MB_OK | MB_ICONERROR | MB_SETFOREGROUND;
  MessageBox(nullptr, description.c_str(), "Crash in Window.js", flags);
  return EXCEPTION_EXECUTE_HANDLER;
}

void InitFail() {
  SetUnhandledExceptionFilter(OnUnhandledException);
}

void Fail(const char* reason, ...) {
  char buffer[1024];
  va_list ap;
  va_start(ap, reason);
  vsnprintf(buffer, 1024, reason, ap);

  std::string description = intro;
  description +=
      "Use Ctrl+C to copy the message below and include it in the "
      "bug report.\n\n";
  description += buffer;
  description += "\n\n" + GetVersionString();

  UINT flags = MB_OK | MB_ICONERROR | MB_SETFOREGROUND;
  MessageBox(nullptr, description.c_str(), "Crash in Window.js", flags);

  abort();
}

#else

static void HandleSegfault(int ignored) {
  fprintf(stderr, "%s", intro);

#if !defined(WINDOWJS_RELEASE_BUILD)
  fprintf(stderr, "Include the stack trace below in the bug report:\n\n%s\n",
          v8::base::debug::StackTrace().ToString().c_str());
#endif

  fprintf(stderr, "%s\n", GetVersionString().c_str());

  abort();
}

void InitFail() {
  signal(SIGSEGV, HandleSegfault);
}

void Fail(const char* reason, ...) {
  fprintf(stderr, "%s", intro);

  va_list ap;
  va_start(ap, reason);
  vfprintf(stderr, reason, ap);
  fprintf(stderr, "\n");

  fprintf(stderr, "%s\n", GetVersionString().c_str());

  abort();
}

#endif  // WINDOWJS_WIN

void ErrorQuit(const char* reason, ...) {
  va_list ap;
  va_start(ap, reason);
  vfprintf(stderr, reason, ap);
  exit(1);
}
