#include "css.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

#include <skia/include/core/SkFontMgr.h>

#include "fail.h"
#include "platform.h"

namespace {

int ToHex(int x) {
  return x < 10 ? '0' + x : 'a' + (x - 10);
}

bool ValidHex(const std::string& color) {
  for (unsigned i = 1; i < color.size(); i++) {
    if (!isxdigit(color[i])) {
      return false;
    }
  }
  return true;
}

int FromHex(char c) {
  if (isdigit(c)) {
    return c - '0';
  } else {
    return tolower(c) - 'a' + 10;
  }
}

}  // namespace

//   "colorname"
//   #rgb
//   #rgba
//   #rrggbb
//   #rrggbbaa
//
//   /* <rgb()> values */
//   color: rgb(34, 12, 64, 0.6);
//   color: rgba(34, 12, 64, 0.6);
//   color: rgb(34 12 64 / 0.6);
//   color: rgba(34 12 64 / 0.3);
//   color: rgb(34.0 12 64 / 60%);
//   color: rgba(34.6 12 64 / 30%);
//
//   /* <hsl()> values */
//   color: hsl(30, 100%, 50%, 0.6);
//   color: hsla(30, 100%, 50%, 0.6);
//   color: hsl(30 100% 50% / 0.6);
//   color: hsla(30 100% 50% / 0.6);
//   color: hsl(30.0 100% 50% / 60%);
//   color: hsla(30.2 100% 50% / 60%);
bool CSSColorToSkColor(std::string color, SkColor* out) {
  if (color.empty()) {
    return false;
  }

  if (color[0] == '#') {
    if (!ValidHex(color)) {
      return false;
    }

    int a = 0xff;
    int r;
    int g;
    int b;

    if (color.size() == 4 || color.size() == 5) {
      r = FromHex(color[1]);
      g = FromHex(color[2]);
      b = FromHex(color[3]);
      r = (r << 4) + r;
      g = (g << 4) + g;
      b = (b << 4) + b;
      if (color.size() == 5) {
        a = FromHex(color[4]);
        a = (a << 4) + a;
      }
    } else if (color.size() == 7 || color.size() == 9) {
      r = (FromHex(color[1]) << 4) + FromHex(color[2]);
      g = (FromHex(color[3]) << 4) + FromHex(color[4]);
      b = (FromHex(color[5]) << 4) + FromHex(color[6]);
      if (color.size() == 9) {
        a = (FromHex(color[7]) << 4) + FromHex(color[8]);
      }
    } else {
      return false;
    }

    *out = SkColorSetARGB(a, r, g, b);
    return true;
  }

  for (auto& c : color) {
    c = tolower(c);
  }

  if (strncmp(color.c_str(), "rgb(", 4) == 0 ||
      strncmp(color.c_str(), "rgba(", 5) == 0) {
    // TODO: parse more formats.
    static const std::regex re{
        "rgba?\\( *([0-9]+) *, *([0-9]+) *, *([0-9]+)( *, *(0?.?([0-9]+)?))? "
        "*\\)"};
    std::smatch m;
    if (std::regex_match(color, m, re)) {
      int r = std::clamp(std::stoi(m[1]), 0, 255);
      int g = std::clamp(std::stoi(m[2]), 0, 255);
      int b = std::clamp(std::stoi(m[3]), 0, 255);
      int a = 255;
      if (m.size() >= 6 && m[5].length() > 0) {
        a = std::clamp(static_cast<int>(255 * std::stof(m[5])), 0, 255);
      }
      *out = SkColorSetARGB(a, r, g, b);
      return true;
    } else {
      return false;
    }
  } else if (strncmp(color.c_str(), "hsv(", 4) == 0 ||
             strncmp(color.c_str(), "hsva(", 5) == 0) {
    // TODO: parse hsv() color formats.
    return false;
  }

  static const std::unordered_map<std::string, SkColor> map{
      {"aliceblue", /*            */ SkColorSetRGB(0xf0, 0xf8, 0xff)},
      {"antiquewhite", /*         */ SkColorSetRGB(0xfa, 0xeb, 0xd7)},
      {"aqua", /*                 */ SkColorSetRGB(0x00, 0xff, 0xff)},
      {"aquamarine", /*           */ SkColorSetRGB(0x7f, 0xff, 0xd4)},
      {"azure", /*                */ SkColorSetRGB(0xf0, 0xff, 0xff)},
      {"beige", /*                */ SkColorSetRGB(0xf5, 0xf5, 0xdc)},
      {"bisque", /*               */ SkColorSetRGB(0xff, 0xe4, 0xc4)},
      {"black", /*                */ SkColorSetRGB(0x00, 0x00, 0x00)},
      {"blanchedalmond", /*       */ SkColorSetRGB(0xff, 0xeb, 0xcd)},
      {"blue", /*                 */ SkColorSetRGB(0x00, 0x00, 0xff)},
      {"blueviolet", /*           */ SkColorSetRGB(0x8a, 0x2b, 0xe2)},
      {"brown", /*                */ SkColorSetRGB(0xa5, 0x2a, 0x2a)},
      {"burlywood", /*            */ SkColorSetRGB(0xde, 0xb8, 0x87)},
      {"cadetblue", /*            */ SkColorSetRGB(0x5f, 0x9e, 0xa0)},
      {"chartreuse", /*           */ SkColorSetRGB(0x7f, 0xff, 0x00)},
      {"chocolate", /*            */ SkColorSetRGB(0xd2, 0x69, 0x1e)},
      {"coral", /*                */ SkColorSetRGB(0xff, 0x7f, 0x50)},
      {"cornflowerblue", /*       */ SkColorSetRGB(0x64, 0x95, 0xed)},
      {"cornsilk", /*             */ SkColorSetRGB(0xff, 0xf8, 0xdc)},
      {"crimson", /*              */ SkColorSetRGB(0xdc, 0x14, 0x3c)},
      {"cyan", /*                 */ SkColorSetRGB(0x00, 0xff, 0xff)},
      {"darkblue", /*             */ SkColorSetRGB(0x00, 0x00, 0x8b)},
      {"darkcyan", /*             */ SkColorSetRGB(0x00, 0x8b, 0x8b)},
      {"darkgoldenrod", /*        */ SkColorSetRGB(0xb8, 0x86, 0x0b)},
      {"darkgray", /*             */ SkColorSetRGB(0xa9, 0xa9, 0xa9)},
      {"darkgreen", /*            */ SkColorSetRGB(0x00, 0x64, 0x00)},
      {"darkgrey", /*             */ SkColorSetRGB(0xa9, 0xa9, 0xa9)},
      {"darkkhaki", /*            */ SkColorSetRGB(0xbd, 0xb7, 0x6b)},
      {"darkmagenta", /*          */ SkColorSetRGB(0x8b, 0x00, 0x8b)},
      {"darkolivegreen", /*       */ SkColorSetRGB(0x55, 0x6b, 0x2f)},
      {"darkorange", /*           */ SkColorSetRGB(0xff, 0x8c, 0x00)},
      {"darkorchid", /*           */ SkColorSetRGB(0x99, 0x32, 0xcc)},
      {"darkred", /*              */ SkColorSetRGB(0x8b, 0x00, 0x00)},
      {"darksalmon", /*           */ SkColorSetRGB(0xe9, 0x96, 0x7a)},
      {"darkseagreen", /*         */ SkColorSetRGB(0x8f, 0xbc, 0x8f)},
      {"darkslateblue", /*        */ SkColorSetRGB(0x48, 0x3d, 0x8b)},
      {"darkslategray", /*        */ SkColorSetRGB(0x2f, 0x4f, 0x4f)},
      {"darkslategrey", /*        */ SkColorSetRGB(0x2f, 0x4f, 0x4f)},
      {"darkturquoise", /*        */ SkColorSetRGB(0x00, 0xce, 0xd1)},
      {"darkviolet", /*           */ SkColorSetRGB(0x94, 0x00, 0xd3)},
      {"deeppink", /*             */ SkColorSetRGB(0xff, 0x14, 0x93)},
      {"deepskyblue", /*          */ SkColorSetRGB(0x00, 0xbf, 0xff)},
      {"dimgray", /*              */ SkColorSetRGB(0x69, 0x69, 0x69)},
      {"dimgrey", /*              */ SkColorSetRGB(0x69, 0x69, 0x69)},
      {"dodgerblue", /*           */ SkColorSetRGB(0x1e, 0x90, 0xff)},
      {"firebrick", /*            */ SkColorSetRGB(0xb2, 0x22, 0x22)},
      {"floralwhite", /*          */ SkColorSetRGB(0xff, 0xfa, 0xf0)},
      {"forestgreen", /*          */ SkColorSetRGB(0x22, 0x8b, 0x22)},
      {"fuchsia", /*              */ SkColorSetRGB(0xff, 0x00, 0xff)},
      {"gainsboro", /*            */ SkColorSetRGB(0xdc, 0xdc, 0xdc)},
      {"ghostwhite", /*           */ SkColorSetRGB(0xf8, 0xf8, 0xff)},
      {"gold", /*                 */ SkColorSetRGB(0xff, 0xd7, 0x00)},
      {"goldenrod", /*            */ SkColorSetRGB(0xda, 0xa5, 0x20)},
      {"gray", /*                 */ SkColorSetRGB(0x80, 0x80, 0x80)},
      {"green", /*                */ SkColorSetRGB(0x00, 0x80, 0x00)},
      {"greenyellow", /*          */ SkColorSetRGB(0xad, 0xff, 0x2f)},
      {"grey", /*                 */ SkColorSetRGB(0x80, 0x80, 0x80)},
      {"honeydew", /*             */ SkColorSetRGB(0xf0, 0xff, 0xf0)},
      {"hotpink", /*              */ SkColorSetRGB(0xff, 0x69, 0xb4)},
      {"indianred", /*            */ SkColorSetRGB(0xcd, 0x5c, 0x5c)},
      {"indigo", /*               */ SkColorSetRGB(0x4b, 0x00, 0x82)},
      {"ivory", /*                */ SkColorSetRGB(0xff, 0xff, 0xf0)},
      {"khaki", /*                */ SkColorSetRGB(0xf0, 0xe6, 0x8c)},
      {"lavender", /*             */ SkColorSetRGB(0xe6, 0xe6, 0xfa)},
      {"lavenderblush", /*        */ SkColorSetRGB(0xff, 0xf0, 0xf5)},
      {"lawngreen", /*            */ SkColorSetRGB(0x7c, 0xfc, 0x00)},
      {"lemonchiffon", /*         */ SkColorSetRGB(0xff, 0xfa, 0xcd)},
      {"lightblue", /*            */ SkColorSetRGB(0xad, 0xd8, 0xe6)},
      {"lightcoral", /*           */ SkColorSetRGB(0xf0, 0x80, 0x80)},
      {"lightcyan", /*            */ SkColorSetRGB(0xe0, 0xff, 0xff)},
      {"lightgoldenrodyellow", /* */ SkColorSetRGB(0xfa, 0xfa, 0xd2)},
      {"lightgray", /*            */ SkColorSetRGB(0xd3, 0xd3, 0xd3)},
      {"lightgreen", /*           */ SkColorSetRGB(0x90, 0xee, 0x90)},
      {"lightgrey", /*            */ SkColorSetRGB(0xd3, 0xd3, 0xd3)},
      {"lightpink", /*            */ SkColorSetRGB(0xff, 0xb6, 0xc1)},
      {"lightsalmon", /*          */ SkColorSetRGB(0xff, 0xa0, 0x7a)},
      {"lightseagreen", /*        */ SkColorSetRGB(0x20, 0xb2, 0xaa)},
      {"lightskyblue", /*         */ SkColorSetRGB(0x87, 0xce, 0xfa)},
      {"lightslategray", /*       */ SkColorSetRGB(0x77, 0x88, 0x99)},
      {"lightslategrey", /*       */ SkColorSetRGB(0x77, 0x88, 0x99)},
      {"lightsteelblue", /*       */ SkColorSetRGB(0xb0, 0xc4, 0xde)},
      {"lightyellow", /*          */ SkColorSetRGB(0xff, 0xff, 0xe0)},
      {"lime", /*                 */ SkColorSetRGB(0x00, 0xff, 0x00)},
      {"limegreen", /*            */ SkColorSetRGB(0x32, 0xcd, 0x32)},
      {"linen", /*                */ SkColorSetRGB(0xfa, 0xf0, 0xe6)},
      {"magenta", /*              */ SkColorSetRGB(0xff, 0x00, 0xff)},
      {"maroon", /*               */ SkColorSetRGB(0x80, 0x00, 0x00)},
      {"mediumaquamarine", /*     */ SkColorSetRGB(0x66, 0xcd, 0xaa)},
      {"mediumblue", /*           */ SkColorSetRGB(0x00, 0x00, 0xcd)},
      {"mediumorchid", /*         */ SkColorSetRGB(0xba, 0x55, 0xd3)},
      {"mediumpurple", /*         */ SkColorSetRGB(0x93, 0x70, 0xdb)},
      {"mediumseagreen", /*       */ SkColorSetRGB(0x3c, 0xb3, 0x71)},
      {"mediumslateblue", /*      */ SkColorSetRGB(0x7b, 0x68, 0xee)},
      {"mediumspringgreen", /*    */ SkColorSetRGB(0x00, 0xfa, 0x9a)},
      {"mediumturquoise", /*      */ SkColorSetRGB(0x48, 0xd1, 0xcc)},
      {"mediumvioletred", /*      */ SkColorSetRGB(0xc7, 0x15, 0x85)},
      {"midnightblue", /*         */ SkColorSetRGB(0x19, 0x19, 0x70)},
      {"mintcream", /*            */ SkColorSetRGB(0xf5, 0xff, 0xfa)},
      {"mistyrose", /*            */ SkColorSetRGB(0xff, 0xe4, 0xe1)},
      {"moccasin", /*             */ SkColorSetRGB(0xff, 0xe4, 0xb5)},
      {"navajowhite", /*          */ SkColorSetRGB(0xff, 0xde, 0xad)},
      {"navy", /*                 */ SkColorSetRGB(0x00, 0x00, 0x80)},
      {"oldlace", /*              */ SkColorSetRGB(0xfd, 0xf5, 0xe6)},
      {"olive", /*                */ SkColorSetRGB(0x80, 0x80, 0x00)},
      {"olivedrab", /*            */ SkColorSetRGB(0x6b, 0x8e, 0x23)},
      {"orange", /*               */ SkColorSetRGB(0xff, 0xa5, 0x00)},
      {"orangered", /*            */ SkColorSetRGB(0xff, 0x45, 0x00)},
      {"orchid", /*               */ SkColorSetRGB(0xda, 0x70, 0xd6)},
      {"palegoldenrod", /*        */ SkColorSetRGB(0xee, 0xe8, 0xaa)},
      {"palegreen", /*            */ SkColorSetRGB(0x98, 0xfb, 0x98)},
      {"paleturquoise", /*        */ SkColorSetRGB(0xaf, 0xee, 0xee)},
      {"palevioletred", /*        */ SkColorSetRGB(0xdb, 0x70, 0x93)},
      {"papayawhip", /*           */ SkColorSetRGB(0xff, 0xef, 0xd5)},
      {"peachpuff", /*            */ SkColorSetRGB(0xff, 0xda, 0xb9)},
      {"peru", /*                 */ SkColorSetRGB(0xcd, 0x85, 0x3f)},
      {"pink", /*                 */ SkColorSetRGB(0xff, 0xc0, 0xcb)},
      {"plum", /*                 */ SkColorSetRGB(0xdd, 0xa0, 0xdd)},
      {"powderblue", /*           */ SkColorSetRGB(0xb0, 0xe0, 0xe6)},
      {"purple", /*               */ SkColorSetRGB(0x80, 0x00, 0x80)},
      {"rebeccapurple", /*        */ SkColorSetRGB(0x66, 0x33, 0x99)},
      {"red", /*                  */ SkColorSetRGB(0xff, 0x00, 0x00)},
      {"rosybrown", /*            */ SkColorSetRGB(0xbc, 0x8f, 0x8f)},
      {"royalblue", /*            */ SkColorSetRGB(0x41, 0x69, 0xe1)},
      {"saddlebrown", /*          */ SkColorSetRGB(0x8b, 0x45, 0x13)},
      {"salmon", /*               */ SkColorSetRGB(0xfa, 0x80, 0x72)},
      {"sandybrown", /*           */ SkColorSetRGB(0xf4, 0xa4, 0x60)},
      {"seagreen", /*             */ SkColorSetRGB(0x2e, 0x8b, 0x57)},
      {"seashell", /*             */ SkColorSetRGB(0xff, 0xf5, 0xee)},
      {"sienna", /*               */ SkColorSetRGB(0xa0, 0x52, 0x2d)},
      {"silver", /*               */ SkColorSetRGB(0xc0, 0xc0, 0xc0)},
      {"skyblue", /*              */ SkColorSetRGB(0x87, 0xce, 0xeb)},
      {"slateblue", /*            */ SkColorSetRGB(0x6a, 0x5a, 0xcd)},
      {"slategray", /*            */ SkColorSetRGB(0x70, 0x80, 0x90)},
      {"slategrey", /*            */ SkColorSetRGB(0x70, 0x80, 0x90)},
      {"snow", /*                 */ SkColorSetRGB(0xff, 0xfa, 0xfa)},
      {"springgreen", /*          */ SkColorSetRGB(0x00, 0xff, 0x7f)},
      {"steelblue", /*            */ SkColorSetRGB(0x46, 0x82, 0xb4)},
      {"tan", /*                  */ SkColorSetRGB(0xd2, 0xb4, 0x8c)},
      {"teal", /*                 */ SkColorSetRGB(0x00, 0x80, 0x80)},
      {"thistle", /*              */ SkColorSetRGB(0xd8, 0xbf, 0xd8)},
      {"tomato", /*               */ SkColorSetRGB(0xff, 0x63, 0x47)},
      {"turquoise", /*            */ SkColorSetRGB(0x40, 0xe0, 0xd0)},
      {"violet", /*               */ SkColorSetRGB(0xee, 0x82, 0xee)},
      {"wheat", /*                */ SkColorSetRGB(0xf5, 0xde, 0xb3)},
      {"white", /*                */ SkColorSetRGB(0xff, 0xff, 0xff)},
      {"whitesmoke", /*           */ SkColorSetRGB(0xf5, 0xf5, 0xf5)},
      {"yellow", /*               */ SkColorSetRGB(0xff, 0xff, 0x00)},
      {"yellowgreen", /*          */ SkColorSetRGB(0x9a, 0xcd, 0x32)},
  };

  auto it = map.find(color);
  if (it != map.end()) {
    *out = it->second;
    return true;
  }

  return false;
}

std::string SkColorToCSSColor(SkColor color) {
  std::string result;
  int a = SkColorGetA(color);
  int r = SkColorGetR(color);
  int g = SkColorGetG(color);
  int b = SkColorGetB(color);
  result.resize(a == SK_AlphaOPAQUE ? 7 : 9);
  result[0] = '#';
  result[1] = ToHex(r >> 4);
  result[2] = ToHex(r & 0xf);
  result[3] = ToHex(g >> 4);
  result[4] = ToHex(g & 0xf);
  result[5] = ToHex(b >> 4);
  result[6] = ToHex(b & 0xf);
  if (a != SK_AlphaOPAQUE) {
    result[7] = ToHex(a >> 4);
    result[8] = ToHex(a & 0xf);
  }
  return result;
}

// [italic] [bold] [Npx] font-name | "font name"
bool CSSFontToSkFont(
    const std::string& css_font, SkFont* out,
    const std::unordered_map<std::string, sk_sp<SkTypeface>>& fonts,
    std::unordered_map<std::string, sk_sp<SkTypeface>>* cache) {
  bool italic = false;
  bool bold = false;
  float size = 12.0f;

  const char* s = css_font.c_str();

  while (*s == ' ') {
    s++;
  }
  if (strncmp(s, "italic ", 7) == 0) {
    italic = true;
    s += 7;
    while (*s == ' ') {
      s++;
    }
  }
  if (strncmp(s, "bold ", 5) == 0) {
    bold = true;
    s += 5;
    while (*s == ' ') {
      s++;
    }
  }
  if (isdigit(*s)) {
    size = std::strtof(s, const_cast<char**>(&s));
    if (size < 0.0f) {
      size = 0.0f;
    }
    if (s[0] != 'p' || s[1] != 'x') {
      return false;
    }
    s += 2;
    while (*s == ' ') {
      s++;
    }
  }

  const char* end = s + strlen(s) - 1;
  while (end > s && *end == ' ') {
    end--;
  }

  if (end > s && *s == '"' && *end == '"') {
    s++;
    end--;
  }

  std::string font(s, end - s + 1);

  if (font == "sans" || font == "sans-serif") {
    font = "Arial";
  } else if (font == "serif") {
    font = "Times New Roman";
  } else if (font == "monospace") {
#if defined(WINDOWJS_MAC)
    font = "Monaco";
#elif defined(WINDOWJS_LINUX)
    font = "DejaVu Sans Mono";
#else
    font = "Consolas";
#endif
  }

  sk_sp<SkTypeface> typeface;

  auto it = fonts.find(font);
  if (it != fonts.end()) {
    typeface = it->second;
  } else {
    std::string key = font + (bold ? "\tb" : "") + (italic ? "\ti" : "");

    bool has_cached_result = false;
    if (cache) {
      auto it = cache->find(key);
      if (it != cache->end()) {
        typeface = it->second;
        has_cached_result = true;
      }
    }

    if (!has_cached_result) {
      sk_sp<SkFontMgr> font_manager = SkFontMgr::RefDefault();
      sk_sp<SkFontStyleSet> set(font_manager->matchFamily(font.c_str()));

      SkFontStyle font_style(
          bold ? SkFontStyle::kBold_Weight : SkFontStyle::kNormal_Weight,
          SkFontStyle::kNormal_Width,
          italic ? SkFontStyle::kItalic_Slant : SkFontStyle::kUpright_Slant);

      typeface.reset(set->matchStyle(font_style));

      // Note: cache missing typefaces as well, so that future lookups fail
      // fast.
      if (cache) {
        cache->emplace(key, typeface);
      }
    }
  }

  if (typeface) {
    out->setTypeface(typeface);
  }

  out->setSize(size);

  return true;
}

std::string SkFontToCSSFont(const SkFont& font) {
  std::stringstream css;

  SkTypeface* typeface = font.getTypefaceOrDefault();

  if (typeface->isItalic()) {
    css << "italic ";
  }
  if (typeface->isBold()) {
    css << "bold ";
  }
  css << font.getSize() << "px ";

  SkString name;
  typeface->getFamilyName(&name);
  if (name.contains(' ')) {
    css << '"' << name.c_str() << '"';
  } else {
    css << name.c_str();
  }

  return css.str();
}
