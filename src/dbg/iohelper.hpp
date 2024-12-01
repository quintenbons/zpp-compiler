#pragma once

#include <boost/filesystem.hpp>
#include <boost/filesystem/file_status.hpp>
#include <fstream>
#include <format>

#include "errors.hpp"

namespace utils::fs {

static inline void safeExists(boost::filesystem::path filePath) {
  CUSTOM_ASSERT(boost::filesystem::exists(filePath), "File " << filePath << " does not exist", EXIT_IO_ERROR);
}

static inline void safeCanRead(const boost::filesystem::path& filePath) {
  safeExists(filePath);

  struct stat fileStat;
  bool statRes = stat(filePath.string().c_str(), &fileStat) == 0;
  CUSTOM_ASSERT(statRes, "Failed to retrieve file permissions for " << filePath, EXIT_IO_ERROR);

  bool canRead = false;
  if (fileStat.st_mode & S_IRUSR && fileStat.st_uid == getuid()) {
      canRead = true;
  } else if (fileStat.st_mode & S_IRGRP && fileStat.st_gid == getgid()) {
      canRead = true;
  } else if (fileStat.st_mode & S_IROTH) {
      canRead = true;
  }

  CUSTOM_ASSERT(canRead, "File " << filePath << " is not readable", EXIT_IO_ERROR);
}

static inline std::ifstream safeIfStream(const boost::filesystem::path &filePath) {
  std::ifstream fileStream(filePath);
  CUSTOM_ASSERT(fileStream.is_open(), "Failed to open output file " << filePath, EXIT_IO_ERROR);
  return fileStream;
}

static inline std::ofstream safeOfStream(const boost::filesystem::path &filePath) {
  std::ofstream fileStream(filePath, std::fstream::out | std::fstream::trunc);
  CUSTOM_ASSERT(fileStream.is_open(), "Failed to open output file " << filePath, EXIT_IO_ERROR);
  return fileStream;
}

static inline boost::filesystem::path getTempFilePath(std::string_view ext) {
  std::string name = std::format("artifact-%%%%-%%%%-%%%%-%%%%.{}", ext);
  return boost::filesystem::temp_directory_path() / boost::filesystem::unique_path(name);
}

} // namespace utils
