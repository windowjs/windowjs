#include "file.h"

#include <fstream>

#include <errno.h>
#include <string.h>
#include <uv.h>

#include "fail.h"

bool WriteFile(const std::filesystem::path& path, const std::string& content,
               std::string* error) {
  return WriteFile(path, content.data(), content.size(), error);
}

bool WriteFile(const std::filesystem::path& path, const void* data, size_t size,
               std::string* error) {
  std::ofstream file(path, std::ios::binary);
  if (!file.is_open()) {
    *error = "Failed to open " + path.u8string() +
             " for writing: " + strerror(errno);
    return false;
  }
  file.write((char*) data, size);
  file.close();
  if (file.fail()) {
    *error = "Failed to write to " + path.u8string() + ": " + strerror(errno);
    return false;
  }
  return true;
}

bool ReadFile(const std::filesystem::path& path, std::string* content,
              std::string* error) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    *error = "Failed to read " + path.u8string() + ": " + strerror(errno);
    return false;
  }
  std::string s((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
  *content = std::move(s);
  return true;
}

sk_sp<SkData> ReadFile(const std::filesystem::path& path, std::string* error) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file) {
    *error = "Failed to read " + path.u8string() + ": " + strerror(errno);
    return {};
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  sk_sp<SkData> data = SkData::MakeUninitialized(size);
  if (!file.read(static_cast<char*>(data->writable_data()), size)) {
    *error = "Failed to read " + path.u8string() + ": read failed";
    return {};
  }
  return data;
}

bool IsDir(const std::filesystem::path& path, std::string* error) {
  std::error_code error_code;
  bool result = std::filesystem::exists(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  if (!result) {
    return false;
  }
  result = std::filesystem::is_directory(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return result;
}

bool IsFile(const std::filesystem::path& path, std::string* error) {
  std::error_code error_code;
  bool result = std::filesystem::exists(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  if (!result) {
    return false;
  }
  result = std::filesystem::is_regular_file(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return result;
}

size_t GetFileSize(const std::filesystem::path& path, std::string* error) {
  std::error_code error_code;
  size_t result = std::filesystem::file_size(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return 0;
  }
  return result;
}

bool MkDirs(const std::filesystem::path& path, std::string* error) {
  std::error_code error_code;
  bool result = std::filesystem::create_directories(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return result;
}

bool Remove(const std::filesystem::path& path, std::string* error) {
  std::error_code error_code;
  bool result = std::filesystem::remove(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return result;
}

bool RemoveTree(const std::filesystem::path& path, std::string* error) {
  std::error_code error_code;
  std::filesystem::remove_all(path, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return true;
}

bool Rename(const std::filesystem::path& from, const std::filesystem::path& to,
            std::string* error) {
  std::error_code error_code;
  std::filesystem::rename(from, to, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return true;
}

bool Copy(const std::filesystem::path& from, const std::filesystem::path& to,
          std::string* error) {
  std::error_code error_code;
  bool result = std::filesystem::copy_file(from, to, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return result;
}

bool CopyTree(const std::filesystem::path& from,
              const std::filesystem::path& to, std::string* error) {
  std::filesystem::copy_options options = std::filesystem::copy_options::none;
  options |= std::filesystem::copy_options::recursive;
  options |= std::filesystem::copy_options::copy_symlinks;
  std::error_code error_code;
  std::filesystem::copy(from, to, options, error_code);
  if (error_code) {
    *error = error_code.message();
    return false;
  }
  return true;
}

std::filesystem::path GetCwd() {
  std::error_code error_code;
  std::filesystem::path result = std::filesystem::current_path(error_code);
  ASSERT(!error_code);
  return result;
}

std::filesystem::path Dirname(const std::filesystem::path& path) {
  return path.parent_path();
}

std::filesystem::path Basename(const std::filesystem::path& path) {
  return path.filename();
}

std::vector<std::filesystem::path> ListDir(const std::filesystem::path& path,
                                           std::string* error) {
  std::vector<std::filesystem::path> list;
  std::error_code error_code;
  for (const std::filesystem::directory_entry& entry :
       std::filesystem::directory_iterator{path, error_code}) {
    std::error_code err;
    std::filesystem::path p =
        std::filesystem::relative(entry.path(), path, err);
    if (err) {
      continue;
    }
    list.emplace_back(std::move(p));
  }
  if (error_code) {
    *error = error_code.message();
    return {};
  }
  return list;
}

std::vector<std::filesystem::path> ListTree(const std::filesystem::path& path,
                                            std::string* error) {
  std::vector<std::filesystem::path> list;
  std::error_code error_code;
  for (const std::filesystem::directory_entry& entry :
       std::filesystem::recursive_directory_iterator{path, error_code}) {
    std::error_code err;
    std::filesystem::path p =
        std::filesystem::relative(entry.path(), path, err);
    if (err) {
      continue;
    }
    list.emplace_back(std::move(p));
  }
  if (error_code) {
    *error = error_code.message();
    return {};
  }
  return list;
}

std::string GetExePath(std::string* error) {
  size_t size = 4096;
  char buffer[4096];
  int result = uv_exepath(buffer, &size);
  if (result != 0 || size > 4096) {
    *error = uv_strerror(result);
    return {};
  }
  return buffer;
}

std::string GetUserHomePath(std::string* error) {
  size_t size = 4096;
  char buffer[4096];
  int result = uv_os_homedir(buffer, &size);
  if (result != 0 || size > 4096) {
    *error = uv_strerror(result);
    return {};
  }
  return buffer;
}

std::string GetTmpDir(std::string* error) {
  size_t size = 4096;
  char buffer[4096];
  int result = uv_os_tmpdir(buffer, &size);
  if (result != 0 || size > 4096) {
    *error = uv_strerror(result);
    return {};
  }
  return buffer;
}
