#pragma once

#include <boost/filesystem.hpp>
#include <fstream>
#include <format>

#include "errors.hpp"

namespace utils {

class OFStream {
public:
  void operator<<(const auto &&data) { fileStream << std::move(data); }
  void operator<<(const auto &data) { fileStream << data; }
  inline const auto &path() { return filePath; }
  inline auto is_open() { return fileStream.is_open(); }
  inline auto close() { return fileStream.close(); }
  inline auto writeAndClose(const auto &&data) { *this << std::move(data); return close(); }
  inline auto writeAndClose(const auto &data) { *this << data; return close(); }

protected:
  boost::filesystem::path filePath;
  std::fstream fileStream;
};

class TempOFStream: public OFStream {
public:
  TempOFStream(std::string_view ext) {
    std::string name = std::format("artifact-%%%%-%%%%-%%%%-%%%%.{}", ext);
    filePath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path(name);
    fileStream.open(filePath.string(), std::fstream::in | std::fstream::out | std::fstream::trunc);
    CUSTOM_ASSERT(fileStream.is_open(), "Failed to open temporary file " << filePath, EXIT_IO_ERROR);
  }

  ~TempOFStream() {
    fileStream.close();
    boost::filesystem::remove(filePath);
  }
};

class SafeOFStream: public OFStream {
public:
  SafeOFStream(boost::filesystem::path _filePath) {
    filePath = std::move(_filePath);
    fileStream.open(filePath.string(), std::fstream::in | std::fstream::out | std::fstream::trunc);
    CUSTOM_ASSERT(fileStream.is_open(), "Failed to open output file " << filePath, EXIT_IO_ERROR);
  }
};

class DryRunSafeIFStream {
public:
  DryRunSafeIFStream(boost::filesystem::path _filePath): filePath(_filePath) {
    std::ifstream fileStream(filePath.string(), std::fstream::in | std::fstream::trunc);
    (void) fileStream;
  }

  inline const auto &path() { return filePath; }

private:
  boost::filesystem::path filePath;
};

} // namespace utils
