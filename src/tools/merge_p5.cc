#include <cstdlib>
#include <fstream>
#include <iostream>

#include <errno.h>

std::string ReadFile(const std::string& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to read " << path << ": " << strerror(errno) << "\n";
    std::exit(1);
  }
  std::string s((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
  return s;
}

void WriteFile(const std::string& path, const std::string& content) {
  std::ofstream file(path, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Failed to open " << path
              << " for writing: " << strerror(errno) << "\n";
    std::exit(1);
  }
  file.write((char*) content.data(), content.size());
  file.close();
  if (file.fail()) {
    std::cerr << "Failed to write to " << path << ": " << strerror(errno)
              << "\n";
    std::exit(1);
  }
}

int main(int argc, const char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: merge-p5 output.js p5-loader.js p5-library.js\n";
    std::exit(1);
  }

  std::string loader = ReadFile(argv[2]);
  std::string library = ReadFile(argv[3]);

  auto k = loader.find("WINDOWJS_P5_IMPORT_MARKER");
  if (k == std::string::npos) {
    std::cerr << "WINDOWJS_P5_IMPORT_MARKER not found.\n";
    std::exit(1);
  }

  std::string merged = loader.substr(0, k);

  merged += "\n\nfunction loadP5() {\n";
  merged += library;
  merged += "\n}\n";

  WriteFile(argv[1], merged);

  return 0;
}
