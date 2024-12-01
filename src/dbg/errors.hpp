#pragma once

#include <boost/stacktrace.hpp>
#include <sstream>

#include "logger.hpp"

enum ExitCode: int {
// Public codes
  EXIT_OK=0,
  EXIT_HELP=0,
  EXIT_USER_THROW=1,
  EXIT_INVALID_ARGUMENTS=2,
  EXIT_UNSUPPORTED=3,
  EXIT_IO_ERROR=4,
  EXIT_NASM_FAILURE=5,

// Unexpected codes
  EXIT_THROW=128,
  EXIT_DEBUG_THROW=129,
};

#define SILENT_THROW_CODE(msg, code)                                           \
  do {                                                                         \
    LOG_ERROR(msg);                                                            \
    std::exit(code);                                                           \
  } while (0)

#define THROW_CODE(msg, code)                                                  \
  do {                                                                         \
    LOG_ERROR(msg);                                                            \
    LOG_ERROR(boost::stacktrace::to_string(boost::stacktrace::stacktrace()));  \
    std::exit(code);                                                           \
  } while (0)

#define THROW(msg)                                                             \
  do {                                                                         \
    LOG_ERROR(msg);                                                            \
    LOG_ERROR(boost::stacktrace::to_string(boost::stacktrace::stacktrace()));  \
    std::exit(EXIT_THROW);                                                     \
  } while (0)

#define DEBUG_ASSERT(cond, ...)                                      \
  do                                                                 \
  {                                                                  \
    if (!(cond))                                                     \
    {                                                                \
      std::stringstream ss;                                          \
      ss << "Assertion failed: (" << #cond << "), file " << __FILE__ \
         << ", line " << __LINE__;                                   \
      if constexpr (sizeof(#__VA_ARGS__) > 1)                        \
      {                                                              \
        ss << ": " << __VA_ARGS__;                                   \
      }                                                              \
      LOG_ERROR(ss.str());                                           \
      std::exit(EXIT_DEBUG_THROW);                                   \
    }                                                                \
  } while (0)

#define USER_THROW(msg, ...)                                                   \
  do {                                                                         \
    std::stringstream ss;                                                      \
    __VA_OPT__(const FilePosition &pos = __VA_ARGS__;                          \
               ss << "[Line: " << pos.lineCount                                \
                  << ", Offset: " << pos.lineOffset << "] ";)                  \
    ss << "Build failed: " << msg;                                             \
    __VA_OPT__(ss << '\n'                                                      \
                  << pos.lineView << '\n';                                     \
               if (pos.lineOffset > 0) ss                                      \
               << std::string(pos.lineOffset - 1, ' ');                        \
               ss << "^";)                                                     \
    LOG_ERROR(ss.str());                                                       \
    std::exit(EXIT_USER_THROW);                                                \
  } while (0)

#define USER_ASSERT(cond, msg, ...) \
  do                                \
  {                                 \
    if (!(cond))                    \
    {                               \
      USER_THROW(msg, __VA_ARGS__); \
    }                               \
  } while (0)

#define CUSTOM_ASSERT(cond, msg, code)                                         \
  do {                                                                         \
    if (!(cond)) {                                                             \
      SILENT_THROW_CODE(msg, code);                                            \
    }                                                                          \
  } while (0)

#define TODO(msg) SILENT_THROW_CODE("TODO " << msg, EXIT_UNSUPPORTED)
