#pragma once

#include <sstream>

namespace codegen
{

class NasmGenerator_x86_64
{
public:
  static constexpr char ENDL = '\n';
  static constexpr char INDENT = '\t';

  struct TextSection {
    std::stringstream sectionTitle;
    std::stringstream globalDeclarations;
    std::stringstream preBody;
    std::stringstream body;
  };

public:
  NasmGenerator_x86_64() {
    emitDataSectionDirective();
    emitRODataSectionDirective();
    emitBSSSectionDirective();
    emitTextSectionDirective();
  };

  template <typename T>
  NasmGenerator_x86_64& operator<<(const T& value) {
    textSection.body << value;
    return *this;
  }

  void emitDataSectionDirective() {
    dataSection << "section .data";
  }

  void emitRODataSectionDirective() {
    RODataSection << "section .rodata";
  }

  void emitBSSSectionDirective() {
    bssSection << "section .bss";
  }

  void emitTextSectionDirective() {
    textSection.sectionTitle << "section .text";
  }

  void emitStartProcedure() {
    emitGlobalDirective("_start");
    textSection.preBody << "_start:" << ENDL;
    textSection.preBody << INDENT << "call main" << ENDL;
    textSection.preBody << INDENT << "mov rdi, rax                 ; Exit code (0) expects return of main to be put in rax for now" << ENDL;
    textSection.preBody << INDENT << "mov rax, 60                  ; Syscall number for exit (60)" << ENDL;
    textSection.preBody << INDENT << "syscall                      ; Make the syscall" << ENDL;
  }

  void emitGlobalDirective(const std::string_view &name) {
    textSection.globalDeclarations << INDENT << "global " << name << ENDL;
  }

  void emitFunctionLabel(const std::string_view &name) {
    containsMain = containsMain || name == "main";
    textSection.body << name << ":" << ENDL;
  }

  void emitReturnInstruction() {
    textSection.body << INDENT << "ret" << ENDL;
  }

  void generateAsmCode(std::ostream &asmCode) {
    if (containsMain) {
      emitStartProcedure();
    }

    asmCode << dataSection.str() << ENDL;

    asmCode << ENDL;
    asmCode << RODataSection.str() << ENDL;

    asmCode << ENDL;
    asmCode << bssSection.str() << ENDL;

    asmCode << ENDL;
    asmCode << textSection.sectionTitle.str() << ENDL;
    asmCode << textSection.globalDeclarations.str() << ENDL;
    asmCode << textSection.preBody.str() << ENDL;
    asmCode << textSection.body.str();
  }

private:
  bool containsMain = false;
  std::stringstream dataSection;
  std::stringstream RODataSection;
  std::stringstream bssSection;
  TextSection textSection;
};

}
