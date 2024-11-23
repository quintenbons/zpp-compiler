#pragma once

#include <boost/stacktrace.hpp>

#include "logger.hpp"

#define THROW(msg)                              \
  do                                            \
  {                                             \
    LOG_ERROR(msg);                             \
    LOG_ERROR(boost::stacktrace::stacktrace()); \
    std::exit(1);                               \
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
      std::exit(1);                                                  \
    }                                                                \
  } while (0)

#define USER_THROW(msg, ...)                                \
  do                                                        \
  {                                                         \
    std::stringstream ss;                                   \
    __VA_OPT__(                                             \
        const FilePosition &pos = __VA_ARGS__;              \
        ss << "[Line: " << pos.lineCount                    \
           << ", Offset: " << pos.lineOffset << "] ";)      \
    ss << "Build failed: " << msg;                          \
    __VA_OPT__(                                             \
        ss << '\n'                                          \
           << pos.lineView;                                 \
        ss << '\n'                                          \
           << std::string(pos.lineOffset - 1, ' ') << "^";) \
    LOG_ERROR(ss.str());                                    \
    std::exit(1);                                           \
  } while (0)

#define USER_ASSERT(cond, msg, ...) \
  do                                \
  {                                 \
    if (!(cond))                    \
    {                               \
      USER_THROW(msg, __VA_ARGS__); \
    }                               \
  } while (0)

#define TODO(msg) THROW("TODO " << msg)
