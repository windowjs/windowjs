#include "zip.h"

#include <v8/third_party/zlib/google/compression_utils_portable.h>

#include "fail.h"

std::string GzipCompress(std::string_view input, int compression_level) {
  std::string output;
  output.resize(zlib_internal::GzipExpectedCompressedSize(input.size()));
  Cr_z_Bytef* dest = reinterpret_cast<Cr_z_Bytef*>(output.data());
  Cr_z_uLongf size = output.size();
  const Cr_z_Bytef* source = reinterpret_cast<const Cr_z_Bytef*>(input.data());
  int result = zlib_internal::CompressHelper(
      zlib_internal::GZIP, dest, &size, source, input.size(), compression_level,
      nullptr, nullptr);
  ASSERT(result == Z_OK);
  output.resize(size);
  return output;
}

std::string GzipUncompress(std::string_view input) {
  const Cr_z_Bytef* source = reinterpret_cast<const Cr_z_Bytef*>(input.data());
  std::string output;
  output.resize(zlib_internal::GetGzipUncompressedSize(source, input.size()));
  Cr_z_Bytef* dest = reinterpret_cast<Cr_z_Bytef*>(output.data());
  Cr_z_uLongf size = output.size();
  int result = zlib_internal::UncompressHelper(zlib_internal::GZIP, dest, &size,
                                               source, input.size());
  ASSERT(result == Z_OK);
  ASSERT(size == output.size());
  return output;
}
