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
  bool containsMain = false;

  ::std::stringstream dataSection;
  ::std::stringstream RODataSection;
  ::std::stringstream bssSection;
  struct TextSection {
    ::std::stringstream sectionTitle;
    ::std::stringstream globalDeclarations;
    ::std::stringstream body;
  };

  TextSection textSection;

  void startDataSection() {
    dataSection << "section .data" << ENDL;
  }

  void startRODataSection() {
    RODataSection << "section .rodata" << ENDL;
  }

  void startBSSSection() {
    bssSection << "section .bss" << ENDL;
  }

  void startTextSection() {
    textSection.sectionTitle << "section .text" << ENDL;
  }

  void generateStartFunction() {
    functionSetGlobal("_start");
    textSection.body << "_start:" << ENDL;
    textSection.body << INDENT << "call main" << ENDL;
    textSection.body << INDENT
            << "mov rax, 60                  ; Syscall number for exit (60)"
            << ENDL;
    textSection.body << INDENT
            << "mov rdi, rbx                 ; Exit code (0) expects return of "
               "main to be put in rbx for now"
            << ENDL;
    textSection.body << INDENT << "syscall                      ; Make the syscall"
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
    textSection.body << value;
    return *this;
  }

  void functionSetGlobal(const ::std::string_view &name) {
    textSection.globalDeclarations << INDENT << "global " << name << ENDL;
  }

  void functionDeclare(const ::std::string_view &name) {
    if (!containsMain) {
      containsMain = name == "main";
    }

    textSection.body << name << ":" << ENDL;
  }

  void functionReturnEmpty() {
    textSection.body << INDENT << "ret" << ENDL << ENDL;
  }

  void generateAsmCode(::std::stringstream &asmCode) {
    if (containsMain)
      generateStartFunction();

    asmCode << dataSection.str();
    asmCode << ENDL;
    asmCode << RODataSection.str();
    asmCode << ENDL;
    asmCode << bssSection.str();
    asmCode << ENDL;
    asmCode << textSection.sectionTitle.str();
    asmCode << textSection.globalDeclarations.str();
    asmCode << ENDL;
    asmCode << textSection.body.str();
  }
};


} /* namespace ast */
