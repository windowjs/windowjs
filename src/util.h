#ifndef WINDOWJS_UTIL_H
#define WINDOWJS_UTIL_H

#include <string_view>

inline bool StartsWith(std::string_view s, std::string_view t) {
  return s.size() >= t.size() && s.compare(0, t.size(), t) == 0;
}

inline bool EndsWith(std::string_view s, std::string_view t) {
  return s.size() >= t.size() &&
         s.compare(s.size() - t.size(), std::string_view::npos, t) == 0;
}

#endif  // WINDOWJS_UTIL_H
