cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_COMPILER g++-13)

# set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer")

set(SRC_DIR "src")
set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(MAIN_PROGRAM_SOURCES
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/lexing_parsing/lexer.ipp
  ${SRC_DIR}/lexing_parsing/parser.ipp
  ${SRC_DIR}/lexing_parsing/ast.hpp
  ${SRC_DIR}/core/errors.hpp
  ${SRC_DIR}/core/logger.hpp
  ${SRC_DIR}/core/utils.hpp
)

project(MyCppCompiler VERSION 0.1 LANGUAGES CXX)

add_executable(${CMAKE_PROJECT_NAME} ${MAIN_PROGRAM_SOURCES})
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SRC_DIR})

find_library(BACKTRACE_LIBRARY NAMES backtrace PATHS /usr/local/lib NO_DEFAULT_PATH)
if(BACKTRACE_LIBRARY)
  add_definitions(-DBOOST_STACKTRACE_USE_BACKTRACE)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${BACKTRACE_LIBRARY})
endif()