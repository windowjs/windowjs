#ifndef WINDOWJS_PLATFORM_H
#define WINDOWJS_PLATFORM_H

// To dump the predefined processor macros:
// $ echo | /usr/bin/clang++ -dM -E -

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#define WINDOWJS_WIN 1

#if defined(_WIN64) && defined(__x86_64__) || defined(_M_X64)
#define WINDOWJS_WIN_X64 1
#else
#error "Only Window x64 is supported."
#endif

#elif defined(__APPLE__)

#define WINDOWJS_MAC 1

#include <TargetConditionals.h>

#if TARGET_OS_MAC

#if defined(__arm64__)
#define WINDOWJS_MAC_ARM64 1
#elif defined(__x86_64__)
#define WINDOWJS_MAC_X64 1
#else
#error "Unknown macOS processor"
#endif

#else
#error "Only macOS is supported."
#endif

#elif defined(__linux__)

#define WINDOWJS_LINUX 1

#if defined(__ANDROID__)
#error "Only desktop Linux x64 is supported."
#elif defined(__x86_64__)
#define WINDOWJS_LINUX_X64 1
#else
#error "Unknown linux processor."
#endif

#else

#error "Unknown platform."

#endif

#endif  // WINDOWJS_PLATFORM_H
