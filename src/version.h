#ifndef WINDOWJS_VERSION_H
#define WINDOWJS_VERSION_H

#include <string>
#include <string_view>

extern const std::string_view kGitHash;
extern const std::string_view kGitTag;

std::string GetVersionString();

#endif  // WINDOWJS_VERSION_H
