#include "ast/scopes/registers.hpp"
#include "nodes.h"

namespace ast {

inline void BinaryOperation::loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::GeneralPurposeRegister targetRegister) const {
  auto gRhsRegister = generator.regSet().acquireGuard();
  if (!gRhsRegister) TODO("Not enough registers, memory fallback not implemented");

  auto tmpRegister = gRhsRegister->reg;

  lhs->loadValueInRegister(generator, targetRegister);
  rhs->loadValueInRegister(generator, tmpRegister);

  constexpr auto size = 8; // TODO get size from decoration step

  auto properTargetReg = scopes::getProperRegisterFromID64(targetRegister, size);
  auto properTmpReg = scopes::getProperRegisterFromID64(tmpRegister, size);

  switch (op) {
    case Operation::ADD:
      generator.emitAdd(properTargetReg, properTmpReg);
      break;
    case Operation::SUBSTRACT:
      generator.emitSub(targetRegister, tmpRegister);
      break;
    default:
      THROW("Unrecognised operation " << static_cast<char>(op));
  }
}

} /* namespace ast */
