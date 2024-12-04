#pragma once

#include <iostream>

#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#ifdef ZPP_DEBUG_MODE
static constexpr bool _zppDebugMode = true;
#else
static constexpr bool _zppDebugMode = false;
#endif

static std::ostream &defaultOStream = std::cout;
static std::ostream &defaultEStream = std::cerr;

#define _LOG_INLINE(stream, msg) stream << msg;
#define LOG_INLINE(msg) _LOG_INLINE(defaultOStream, msg)

#define _LOG(stream, msg)                                                      \
  do {                                                                         \
    LOG_INLINE(msg);                                                           \
    stream << ::std::endl;                                                       \
  } while (0)

#define LOG(msg) _LOG(defaultOStream, msg)

#define _LOG_COLOR(stream, msg, color)                                         \
  do {                                                                         \
    stream << color;                                                           \
    _LOG(stream, msg);                                                         \
    stream << RESET;                                                           \
  } while (0)

#define LOG_COLOR(msg, color) _LOG_COLOR(defaultOStream, msg, color)

#define LOG_ERROR(msg) _LOG_COLOR(defaultEStream, msg, RED)
#define LOG_DEBUG_COLOR(msg, color)                                            \
  do {                                                                         \
    if constexpr (_zppDebugMode) {                                             \
      LOG_COLOR(msg, color);                                                   \
    }                                                                          \
  } while (0)
#define LOG_DEBUG(msg) LOG_DEBUG_COLOR(msg, MAGENTA)