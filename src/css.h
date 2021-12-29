#ifndef WINDOWJS_CSS_H
#define WINDOWJS_CSS_H

#include <string>
#include <unordered_map>

#include <skia/include/core/SkColor.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkTypeface.h>

bool CSSColorToSkColor(std::string color, SkColor* out);

std::string SkColorToCSSColor(SkColor color);

bool CSSFontToSkFont(
    const std::string& font, SkFont* out,
    const std::unordered_map<std::string, sk_sp<SkTypeface>>& fonts = {},
    std::unordered_map<std::string, sk_sp<SkTypeface>>* cache = nullptr);

std::string SkFontToCSSFont(const SkFont& font);

#endif  // WINDOWJS_CSS_H
