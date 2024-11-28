#pragma once

#include <cstddef>

#include "nodes.hpp"
#include "scopes/scopeStack.hpp"

namespace ast
{

static constexpr char ENDL = '\n';
static constexpr char INDENT = '\t';

class CodeGeneratorEvaluator
{
  ::std::stringstream dataSection;
  ::std::stringstream RODataSection;
  ::std::stringstream bssSection;
  ::std::stringstream textSection;

  void startDataSection() {
    dataSection << "section .data" << ENDL << ENDL;
  }

  void startRODataSection() {
    RODataSection << "section .rodata" << ENDL << ENDL;
  }

  void startBSSSection() {
    bssSection << "section .bss" << ENDL << ENDL;
  }

  void startTextSection() {
    textSection << "section .text" << ENDL;

    textSection << INDENT << "global _start" << ENDL;
    // asmCode << INDENT << "global print:function" << ENDL; // when generating stdlibc

    textSection << ENDL;
    textSection << "_start:" << ENDL;
    textSection << INDENT << "call main" << ENDL;
    textSection << INDENT
            << "mov rax, 60                  ; Syscall number for exit (60)"
            << ENDL;
    textSection << INDENT
            << "mov rdi, rbx                 ; Exit code (0) expects return of "
               "main to be put in rbx for now"
            << ENDL;
    textSection << INDENT << "syscall                      ; Make the syscall"
            << ENDL;
  }

public:
  CodeGeneratorEvaluator() {
    startDataSection();
    startRODataSection();
    startBSSSection();
    startTextSection();
  };

  template <typename T>
  CodeGeneratorEvaluator& operator<<(const T& value) {
    textSection << value;
    return *this;
  }

  void generateAsmCode(::std::stringstream &asmCode) {
    asmCode << dataSection.str();
    asmCode << RODataSection.str();
    asmCode << bssSection.str();
    asmCode << textSection.str();
  }
};


} /* namespace ast */
