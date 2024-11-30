#pragma once

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <iostream>
#include <utility>

#include "dbg/errors.hpp"

namespace assemble {

namespace bp = boost::process;
namespace fs = boost::filesystem;

static inline std::pair<int, std::string> runNasmImpl(const fs::path& asmFile, const fs::path& outputFile) {
  if (!fs::exists(asmFile)) {
    LOG_ERROR("Assembly file does not exist: " << asmFile);
    return std::make_pair(EXIT_IO_ERROR, "");
  }

  bp::ipstream errStream;
  std::string command = std::format("nasm -f elf64 -o {} {}", outputFile.string(), asmFile.string());

  bp::child c(command, bp::std_err > errStream);

  std::string line;
  while (c.running() && std::getline(errStream, line) && !line.empty()) {
    std::cerr << line << std::endl;
  }

  c.wait();

  return std::make_pair(c.exit_code(), command);
}

static inline int runNasm(const fs::path& asmFile, const fs::path& outputFile) {
  return runNasmImpl(asmFile, outputFile).first;
}

static inline void runNasmSafe(const fs::path& asmFile, const fs::path& outputFile) {
  auto [res, cmd] = runNasmImpl(asmFile, outputFile);
  CUSTOM_ASSERT(res == EXIT_OK, "Nasm run failed command=[" << cmd << "]", res);
}

} /* namespace assemble */
