cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_COMPILER g++-13)

# set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer")

set(SRC_DIR "src")
set(PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}")

project(MyCppCompiler VERSION 0.1 LANGUAGES CXX)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_DIR}/main.cpp ${SRC_DIR}/lexing_parsing/lexer.ipp ${SRC_DIR}/lexing_parsing/parser.ipp ${SRC_DIR}/errors.hpp)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SRC_DIR})

find_library(BACKTRACE_LIBRARY NAMES backtrace PATHS /usr/local/lib NO_DEFAULT_PATH)
if(BACKTRACE_LIBRARY)
  add_definitions(-DBOOST_STACKTRACE_USE_BACKTRACE)
  target_link_libraries(${CMAKE_PROJECT_NAME} ${BACKTRACE_LIBRARY})
endif()