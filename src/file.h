#ifndef WINDOWJS_FILE_H
#define WINDOWJS_FILE_H

#include <filesystem>
#include <string>
#include <vector>

#include <skia/include/core/SkData.h>

bool WriteFile(const std::filesystem::path& path, const std::string& content,
               std::string* error);

bool WriteFile(const std::filesystem::path& path, const void* data, size_t size,
               std::string* error);

bool ReadFile(const std::filesystem::path& path, std::string* content,
              std::string* error);

sk_sp<SkData> ReadFile(const std::filesystem::path& path, std::string* error);

bool IsDir(const std::filesystem::path& path, std::string* error);
bool IsFile(const std::filesystem::path& path, std::string* error);

size_t GetFileSize(const std::filesystem::path& path, std::string* error);

bool MkDirs(const std::filesystem::path& path, std::string* error);
bool Remove(const std::filesystem::path& path, std::string* error);
bool RemoveTree(const std::filesystem::path& path, std::string* error);
bool Rename(const std::filesystem::path& from, const std::filesystem::path& to,
            std::string* error);
bool Copy(const std::filesystem::path& from, const std::filesystem::path& to,
          std::string* error);
bool CopyTree(const std::filesystem::path& from,
              const std::filesystem::path& to, std::string* error);

std::filesystem::path Dirname(const std::filesystem::path& path);
std::filesystem::path Basename(const std::filesystem::path& path);

// The paths returned are relative to the parent "path" argument.
std::vector<std::filesystem::path> ListDir(const std::filesystem::path& path,
                                           std::string* error);
std::vector<std::filesystem::path> ListTree(const std::filesystem::path& path,
                                            std::string* error);

std::filesystem::path GetCwd();
std::string GetExePath(std::string* error);
std::string GetUserHomePath(std::string* error);
std::string GetTmpDir(std::string* error);

#endif  // WINDOWJS_FILE_H
