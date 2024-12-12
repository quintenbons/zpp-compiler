#include <format>
#include <sstream>
#include <variant>

#include "ast/scopes/registers.hpp"
#include "codegen/generate.hpp"
#include "dbg/errors.hpp"

#include "nodes.h"

namespace ast {
namespace {
static constexpr auto ENDL = codegen::NasmGenerator_x86_64::ENDL;
static constexpr auto INDENT = codegen::NasmGenerator_x86_64::INDENT;
} // namespace

inline void Instruction::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &evaluator) const {
  std::visit([&evaluator](const auto &node) { node.genAsm_x86_64(evaluator); },
             instr);
}

inline void InstructionList::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &evaluator) const {
  for (const auto &instr : instructions) {
    instr.genAsm_x86_64(evaluator);
  }
}

inline void FunctionCall::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &generator) const {
  size_t maxArgumentsStoredInRegisters = std::min(arguments.size(), scopes::FUNCTION_ARGUMENT_REGISTERS.size());
  for (size_t i = 0; i < maxArgumentsStoredInRegisters; i++) {
    arguments[i].loadValueInRegister(generator, scopes::FUNCTION_ARGUMENT_REGISTERS[i]);
  }
  for (size_t i = maxArgumentsStoredInRegisters; i < arguments.size(); i++) {
    TODO("Implement pushing arguments on the stack");
  }
  generator.emitCall(name);
}

inline void ReturnStatement::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &generator) const {
  expression.loadValueInRegister(generator, scopes::returnRegister);
}

inline void InlineAsmStatement::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &generator) const {
  generator << ";-- START -- asm binding requests" << ENDL;
  // TODO: not assume that every variable is always in rdx

  for (auto &request : requests) {
    generator << INDENT
              << std::format("mov {}, {}", regToStr(request.registerTo),
                             regToStr(scopes::Register::REG_RAX))
              << ENDL;
  }

  generator << ";-- START -- user defined" << ENDL;
  generator << asmBlock.getContent() << ENDL;
  generator << ";-- END -- user defined" << ENDL;
}

inline void Declaration::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &generator) const {
  generator.emitDeclaration(variable.getVariableDescription()->location);
  if (assignment.has_value()) {
    // TODO: Take an available register
    assignment->loadValueInRegister(generator, scopes::GeneralPurposeRegister::REG_RAX);
    scopes::byteSize_t size = (*variable.getVariableDescription()->typeDescription)->byteSize;
    generator.emitStoreInMemory(variable.getVariableDescription()->location,
                                 scopes::getProperRegisterFromID64(scopes::GeneralPurposeRegister::REG_RAX, size));
  }
}

inline void Function::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  generator.emitGlobalDirective(name);
  generator.emitFunctionLabel(name);

  generator.emitSaveBasePointer();
  generator.emitSetBasePointerToCurrentStackPointer();
  body.genAsm_x86_64(generator);
  generator.emitRestoreStackPointer();
  generator.emitRestoreBasePointer();

  generator.emitReturnInstruction();
}

inline void Expression::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  std::visit([&generator](const auto &node) { node.genAsm_x86_64(generator); }, expr);
}

inline std::string
TranslationUnit::genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const {
  for (auto &func : functions) {
    func.genAsm_x86_64(evaluator);
  }

  for (auto &classNode : classes) {
    (void)classNode;
    TODO("Implement classNodes");
  }

  std::stringstream asmCode;
  evaluator.generateAsmCode(asmCode);

  return asmCode.str();
}

} /* namespace ast */
