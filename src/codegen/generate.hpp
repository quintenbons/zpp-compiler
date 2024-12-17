#pragma once

#include <sstream>
#include "ast/scopes/types.hpp"
#include "ast/scopes/registers.hpp"
#include "ast/scopes/memory_x86_64.hpp"
#include "ast/literalTypes.hpp"
#include "codegen/GPRegisterSet.hpp"

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
    textSection.preBody << INDENT << "mov rdi, " << scopes::regToStr(scopes::getProperRegisterFromID64(scopes::returnRegister)) << "            ; Exit code (0) expects return of main to be put in rax for now" << ENDL;
    textSection.preBody << INDENT << "mov " << scopes::regToStr(scopes::getProperRegisterFromID64(scopes::returnRegister)) << ", 60                  ; Syscall number for exit (60)" << ENDL;
    textSection.preBody << INDENT << "syscall                      ; Make the syscall" << ENDL;
  }

  void emitSaveBasePointer() {
    textSection.body << INDENT << "push " << scopes::regToStr(scopes::Register::REG_RBP) << "                 ; Save the base pointer" << ENDL;
  }

  void emitSetBasePointerToCurrentStackPointer() {
    textSection.body << INDENT << "mov " << scopes::regToStr(scopes::Register::REG_RBP) << ", " << scopes::regToStr(scopes::Register::REG_RSP) << "              ; Set base pointer to current stack pointer" << ENDL;
  }

  void emitGlobalDirective(const std::string_view &name) {
    textSection.globalDeclarations << INDENT << "global " << name << ":function" << ENDL;
  }

  void emitRestoreStackPointer() {
    textSection.body << INDENT << "mov " << scopes::regToStr(scopes::Register::REG_RSP) << ", " << scopes::regToStr(scopes::Register::REG_RBP) << "              ; Restoring stack pointer" << ENDL;
  }

  void emitRestoreBasePointer() {
    textSection.body << INDENT << "pop " << scopes::regToStr(scopes::Register::REG_RBP) << "                   ; Restore the base pointer" << ENDL;
  }

  void emitFunctionLabel(const std::string_view &name) {
    containsMain = containsMain || name == "main";
    textSection.body << name << ":" << ENDL;
  }

  void emitReturnInstruction() {
    textSection.body << INDENT << "ret" << ENDL;
  }

  void emitCall(const std::string_view &name) {
    textSection.body << INDENT << "call " << name << ENDL;
  }

  void emitDeclaration(const scopes::LocationDescription &location) {
    std::visit([this](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, scopes::LocalStackOffset>) {
        textSection.body << INDENT << "sub " << scopes::regToStr(scopes::Register::REG_RSP) << ", " << arg._byteSize << " ; Creating space on the stack" << ENDL;
      }
      else if constexpr (std::is_same_v<T, scopes::GlobalStackOffset>) {
        THROW("Global stack offset not yet implemented");
      }
      else if constexpr (std::is_same_v<T, scopes::Register>) {
        THROW("Register declaration not yet implemented");
      }
      else {
        THROW("Unknown location description type");
      }
    }, location);
  }

  void emitBinaryOp(const char *op, const scopes::Register &tgt, const scopes::Register &src) {
    textSection.body << INDENT << op << " " << scopes::regToStr(tgt) << ", " << scopes::regToStr(src) << ENDL;
  }

  void emitAdd(const scopes::Register &tgt, const scopes::Register &src) { emitBinaryOp("add", tgt, src); }
  void emitSub(const scopes::Register &tgt, const scopes::Register &src) { emitBinaryOp("sub", tgt, src); }

  void emitStoreInMemory(const scopes::LocationDescription &location, const scopes::Register &reg) {
    std::visit([this, &reg](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, scopes::LocalStackOffset>) {
        textSection.body << INDENT << "mov [rbp-" << arg._byteOffset << "], " << scopes::regToStr(reg) << " ; Storing value in memory" << ENDL;
      }
      else if constexpr (std::is_same_v<T, scopes::GlobalStackOffset>) {
        THROW("Global stack offset not yet implemented");
      }
      else if constexpr (std::is_same_v<T, scopes::Register>) {
        THROW("Register declaration not yet implemented");
      }
      else {
        THROW("Unknown location description type");
      }
    }, location);
  }

  void emitLoadFromMemory(const scopes::Register &reg, const scopes::LocationDescription &location) {
    std::visit([this, &reg](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, scopes::LocalStackOffset>) {
        textSection.body << INDENT << "mov " << scopes::regToStr(reg) << ", [rbp-" << arg._byteOffset << "] ; Loading value from memory" << ENDL;
      }
      else if constexpr (std::is_same_v<T, scopes::GlobalStackOffset>) {
        THROW("Global stack offset not yet implemented");
      }
      else if constexpr (std::is_same_v<T, scopes::Register>) {
        THROW("Register declaration not yet implemented");
      }
      else {
        THROW("Unknown location description type");
      }
    }, location);
  }

  void emitLoadNumberLiteral(const scopes::Register &reg, const ast::NumberLiteralUnderlyingType &value) {
    textSection.body << INDENT << "mov " << scopes::regToStr(reg) << ", " << value << " ; Loading number literal" << ENDL;
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

  scopes::GPRegisterSet &regSet() { return registerSet; }

private:
  bool containsMain = false;
  std::stringstream dataSection;
  std::stringstream RODataSection;
  std::stringstream bssSection;
  TextSection textSection;
  scopes::GPRegisterSet registerSet;
};

}
