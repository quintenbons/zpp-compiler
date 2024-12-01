#pragma once

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <iostream>
#include <utility>

#include "dbg/errors.hpp"

namespace linking {

namespace bp = boost::process;
namespace fs = boost::filesystem;

static inline std::pair<int, std::string> runLdImpl(const fs::path &objFile,
                                                    const fs::path &outputFile,
                                                    bool isShared = false) {
  if (!fs::exists(objFile)) {
    LOG_ERROR("Object file does not exist: " << objFile);
    return std::make_pair(EXIT_IO_ERROR, "");
  }

  bp::ipstream errStream;
  std::string command = std::format("ld {}-o {} {}", (isShared ? "-shared " : ""), outputFile.string(), objFile.string());

  bp::child c(command, bp::std_err > errStream);

  std::string line;
  while (c.running() && std::getline(errStream, line) && !line.empty()) {
    std::cerr << line << std::endl;
  }

  c.wait();

  return std::make_pair(c.exit_code(), command);
}

static inline int runLd(const fs::path& objFile, const fs::path& outputFile, bool isShared=false) {
  return runLdImpl(objFile, outputFile, isShared).first;
}

static inline void runLdSafe(const fs::path& objFile, const fs::path& outputFile, bool isShared=false) {
  auto [res, cmd] = runLdImpl(objFile, outputFile, isShared);
  CUSTOM_ASSERT(res == EXIT_OK, "ld run failed command=[" << cmd << "]", res);
}

} /* namespace linking */
