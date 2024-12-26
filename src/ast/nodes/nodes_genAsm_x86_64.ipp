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
    codegen::NasmGenerator_x86_64 &generator) const {
  std::visit([&generator](const auto &node) { node.genAsm_x86_64(generator); },
             instr);
}

inline void CodeBlock::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &generator) const {
  for (const auto &instr : statements) {
    instr.genAsm_x86_64(generator);
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

inline void ConditionalStatement::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  auto elseLabel = generator.generateUniqueLabel("else");
  auto endIfLabel = generator.generateUniqueLabel("end_if");
  std::string_view conditionFailLabel = elseBody.has_value() ? elseLabel : endIfLabel;

  {
    auto condRegGuard = generator.regSet().acquireGuard();
    condition.loadValueInRegister(generator, condRegGuard->reg);
    generator.emitConditionalJump(conditionFailLabel, scopes::getProperRegisterFromID64(condRegGuard->reg));
  }

  ifBody.genAsm_x86_64(generator);

  if (elseBody) {
    generator.emitJump(endIfLabel);
    generator.emitLabel(elseLabel);
    elseBody->genAsm_x86_64(generator);
  }

  generator.emitLabel(endIfLabel);
}

inline void WhileStatement::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  auto condLabel = generator.generateUniqueLabel("while");
  auto endLabel = generator.generateUniqueLabel("while.end");

  generator.emitLabel(condLabel);

  {
    auto condRegGuard = generator.regSet().acquireGuard();
    condition.loadValueInRegister(generator, condRegGuard->reg);
    generator.emitConditionalJump(endLabel, scopes::getProperRegisterFromID64(condRegGuard->reg));
  }

  body.genAsm_x86_64(generator);

  generator.emitJump(condLabel);
  generator.emitLabel(endLabel);
}

inline void DoStatement::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  (void) generator;
  TODO("Implement do-while statement genasm");
}

inline void ForStatement::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  auto condLabel = generator.generateUniqueLabel("for");
  auto endLabel = generator.generateUniqueLabel("for.end");

  init.genAsm_x86_64(generator);

  generator.emitLabel(condLabel);

  if (condition) {
    auto condRegGuard = generator.regSet().acquireGuard();
    condition->loadValueInRegister(generator, condRegGuard->reg);
    generator.emitConditionalJump(endLabel, scopes::getProperRegisterFromID64(condRegGuard->reg));
  }

  body.genAsm_x86_64(generator);

  if (expr) {
    expr->genAsm_x86_64(generator);
  }

  generator.emitJump(condLabel);
  generator.emitLabel(endLabel);
}

inline void Statement::genAsm_x86_64(
    codegen::NasmGenerator_x86_64 &generator) const {
  std::visit([&generator](const auto &node) { node.genAsm_x86_64(generator); },
             statement);
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

inline void FunctionDeclaration::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  if (isExtern) generator.emitExternDirective(name);
  else TODO("Implement non-extern declarations");
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
TranslationUnit::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  for (auto &funcDecl : functionDeclarations) {
    funcDecl.genAsm_x86_64(generator);
  }

  for (auto &func : functions) {
    func.genAsm_x86_64(generator);
  }

  for (auto &classNode : classes) {
    (void)classNode;
    TODO("Implement classNodes");
  }

  std::stringstream asmCode;
  generator.generateAsmCode(asmCode);

  return asmCode.str();
}

} /* namespace ast */
