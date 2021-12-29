#include <cstdlib>
#include <fstream>
#include <iostream>

#include "../fail.h"
#include "../zip.h"
#include "zlib.h"

static unsigned char ToHex(unsigned char x) {
  return x < 10 ? '0' + x : 'a' + (x - 10);
}

int main(int argc, const char* argv[]) {
  if (argc < 4 || argc % 2 != 0) {
    std::cerr << "Usage: embed <output> [<symbol> <input>]+\n";
    std::exit(1);
  }

  std::ofstream out(argv[1], std::ios::binary);
  if (!out) {
    std::cerr << "Couldn't open " << argv[3] << " for writing.\n";
    std::exit(1);
  }

  out << "#include <string_view>\n";

  for (int i = 2; i < argc; i += 2) {
    const char* symbol = argv[i];
    const char* file = argv[i + 1];

    std::ifstream in(file, std::ios::binary);
    if (!in) {
      std::cerr << "Failed to read " << file << "\n";
      std::exit(1);
    }

    std::string content((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());

    std::string compressed = GzipCompress(content, Z_BEST_COMPRESSION);

    out << "\n// Gzip-compressed contents of " << file << "\n";
    out << "extern const std::string_view " << symbol << "{\n";
    out << "    \"";
    int count = 0;
    for (unsigned char c : compressed) {
      if (count == 16) {
        out << "\"\n    \"";
        count = 0;
      }
      out << "\\x" << ToHex(c >> 4) << ToHex(c & 0xf);
      count++;
    }
    out << "\",\n    " << compressed.size() << "\n};\n";
  }

  out.close();

  return 0;
}
