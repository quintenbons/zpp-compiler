cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_COMPILER g++-13)

# set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fno-omit-frame-pointer")

set(SRC_DIR "src")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(MAIN_PROGRAM_SOURCES
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/lexing_parsing/lexer.ipp
  ${SRC_DIR}/lexing_parsing/parser.ipp
  ${SRC_DIR}/ast/nodes.hpp
  ${SRC_DIR}/ast/scopes/scopeStack.hpp
  ${SRC_DIR}/ast/scopes/types.hpp
  ${SRC_DIR}/ast/scopes/memory_x86_64.hpp
  ${SRC_DIR}/ast/scopes/registers.hpp

  ${SRC_DIR}/codegen/generate.hpp

  ${SRC_DIR}/dbg/errors.hpp
  ${SRC_DIR}/dbg/logger.hpp
  ${SRC_DIR}/dbg/utils.hpp

  ${SRC_DIR}/core/TranslationUnitHandle.hpp

  ${SRC_DIR}/ast/DebugEvaluator.hpp
  ${SRC_DIR}/ast/DecoratorEvaluator.ipp
)

set(COMPILER_NAME z++)

project(zpp-compiler VERSION 0.1 LANGUAGES CXX)

add_executable(${COMPILER_NAME} ${MAIN_PROGRAM_SOURCES})
# We removed the warning missing-field-initializers because a bug is present for empty structs before gcc 14 (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=110064)
target_compile_options(${COMPILER_NAME} PRIVATE 
    -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)
target_include_directories(${COMPILER_NAME} PRIVATE ${SRC_DIR})
target_compile_definitions(${COMPILER_NAME} PRIVATE -DZPP_DEBUG_MODE)

find_library(BACKTRACE_LIBRARY NAMES backtrace PATHS /usr/local/lib NO_DEFAULT_PATH)
if(BACKTRACE_LIBRARY)
  add_definitions(-DBOOST_STACKTRACE_USE_BACKTRACE)
  target_link_libraries(${COMPILER_NAME} ${BACKTRACE_LIBRARY})
endif()