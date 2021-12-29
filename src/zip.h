#ifndef WINDOWJS_ZIP_H
#define WINDOWJS_ZIP_H

#include <string>
#include <string_view>

std::string GzipCompress(std::string_view input, int compression_level = -1);

std::string GzipUncompress(std::string_view input);

#endif  // WINDOWJS_ZIP_H
