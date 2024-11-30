#pragma once

#include "dbg/errors.hpp"
#include <format>
#include <iostream>
#include <string_view>
#include <vector>

namespace argparse {

struct CompilerOptions {
  // mandatory
  std::vector<std::string> inputFiles;

  // optional
  std::vector<std::string> includeDirs;
  std::string outputFile;
  bool preprocessOnly = false;
  bool compileOnly = false;
  bool compileAndAssemble = false;
  bool createSharedLib = false;
  bool fullDebugExec = false;
};

class ArgParser {
private:
  using BoolPtrT = bool CompilerOptions::*;
  using StringPtrT = std::string CompilerOptions::*;
  using StringVectorPtrT = std::vector<std::string> CompilerOptions::*;
  using PlaceHolderPtrT = void *;

  template<typename PtrT>
  struct OptionDescription {
    const char *shortFlag;
    const char *longFlag;
    const char *following;
    PtrT ptr;
    const char *description;
  };

/** FLAG DEFINITIONS **/
private:
  static constexpr OptionDescription<BoolPtrT> boolFlags[] = {
    { "-E", nullptr, nullptr, &CompilerOptions::preprocessOnly, "Preprocess only; do not compile, assemble or link." },
    { "-S", nullptr, nullptr, &CompilerOptions::compileOnly, "Compile only; do not assemble or link." },
    { "-c", nullptr, nullptr, &CompilerOptions::compileAndAssemble, "Compile and assemble, but do not link." },
    { "-shared", "--shared", nullptr, &CompilerOptions::createSharedLib, "Link as shared library" },
    { "-d", "--debug", nullptr, &CompilerOptions::fullDebugExec, "Full generation with debug logs" },
  };

  static constexpr OptionDescription<StringPtrT> stringFlags[] = {
    { "-o", nullptr, "<output file>", &CompilerOptions::outputFile, "Place the output into <file>." },
  };

  static constexpr OptionDescription<StringVectorPtrT> fileListFlags[] = {
    { "-I", "--include-path", "<include dir>", &CompilerOptions::includeDirs, "Add directory to include search path" },
  };

  static constexpr OptionDescription<StringVectorPtrT> sourceFiles =
    { nullptr, nullptr, nullptr, &CompilerOptions::inputFiles, "Input source files" };

  static constexpr OptionDescription<PlaceHolderPtrT> helpFlag =
    { "-h", "--help", nullptr, nullptr, "Print usage/help" };

public:
  ArgParser(const int argc, const char * const*argv) : args(argv, argv + argc) {}

  static inline void printUsage() {
    std::cerr << "Usage: z++ [options] file...\n";
    std::cerr << "Options:\n";

    printFlagUsage(helpFlag);
    printFlagListUsage(boolFlags);
    printFlagListUsage(stringFlags);
    printFlagListUsage(fileListFlags);
  }

  inline CompilerOptions parse() {
    CompilerOptions opts = parseRaw();
    verifyAndAdjustOpts(opts);
    return opts;
  }

protected:
  CompilerOptions parseRaw() {
    CompilerOptions opts;

    size_t idx = 1;
    while (idx < args.size())
    {
      auto arg = args.at(idx);

      // help
      if (arg == helpFlag.shortFlag || arg == helpFlag.longFlag) {
        printUsage();
        exit(EXIT_HELP);
      }

      // positional
      else if (arg[0] != '-') {
        opts.inputFiles.emplace_back(arg);
      }

      // optional
      else if (auto *flag = tryMatch(boolFlags, arg)) {
        opts.*(flag->ptr) = true;
      }

      else if (auto *flag = tryMatch(stringFlags, arg)) {
        CUSTOM_ASSERT((idx+1) < args.size(), "Expected " << flag->following << " after " << arg, EXIT_INVALID_ARGUMENTS);
        opts.*(flag->ptr) = args[++idx];
      }

      else if (auto *flag = tryMatch(fileListFlags, arg)) {
        CUSTOM_ASSERT((idx+1) < args.size(), "Expected " << flag->following << " after " << arg, EXIT_INVALID_ARGUMENTS);
        (opts.*(flag->ptr)).emplace_back(args[++idx]);
      }

      else {
        printUsage();
        THROW_CODE("Unknown flag: " << arg, EXIT_INVALID_ARGUMENTS);
      }

      idx++;
    }

    return opts;
  }

  static inline void verifyAndAdjustOpts(CompilerOptions &opts) {
    if (opts.inputFiles.empty()) {
      printUsage();
      SILENT_THROW_CODE("No source file was provided\n", EXIT_INVALID_ARGUMENTS);
    }

    if (opts.inputFiles.size() > 1) {
      printUsage();
      SILENT_THROW_CODE("Multiple source files not yet supported", EXIT_UNSUPPORTED);
    }

    if (opts.outputFile.empty()) {
      opts.outputFile = opts.createSharedLib ? "./a.out" : "./a.so";
    }
  }

  template<typename PtrT>
  static inline void printFlagUsage(const OptionDescription<PtrT> &opt) {
    DEBUG_ASSERT(opt.shortFlag, "optFlag did not provide a shortflag");
    DEBUG_ASSERT(opt.description, "optFlag did not provide a description");

    std::stringstream flagSS;
    flagSS << opt.shortFlag << " ";
    if (opt.longFlag) flagSS << opt.longFlag << " ";
    if (opt.following) flagSS << opt.following << " ";

    std::cerr << std::format("\t{:<40}{}\n", flagSS.str(), opt.description);
  }

  template<typename PtrT, size_t size>
  static inline void printFlagListUsage(const OptionDescription<PtrT> (&flags)[size]) {
    for (size_t i = 0; i < size; ++i) {
      printFlagUsage(flags[i]);
    }
  }

  template<typename PtrT, size_t size>
  static inline const OptionDescription<PtrT> *tryMatch(const OptionDescription<PtrT> (&flags)[size], std::string_view arg) {
    for (size_t i = 0; i < size; ++i) {
      auto &opt = flags[i];

      if (arg == opt.shortFlag || (opt.longFlag && arg == opt.longFlag)) return &opt;
    }

    return nullptr;
  }

private:
  const std::vector<std::string_view> args;
};

} // namespace argparse
