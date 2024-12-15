#include "nodes.h"

namespace ast {

inline void BinaryOperation::loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::GeneralPurposeRegister targetRegister) const {
  auto gRhsRegister = generator.regSet().acquireGuard();
  if (!gRhsRegister) TODO("Not enough registers, memory fallback not implemented");

  auto tmpRegister = gRhsRegister->reg;

  lhs->loadValueInRegister(generator, targetRegister);
  rhs->loadValueInRegister(generator, tmpRegister);

  switch (op) {
    case Operation::ADD:
      generator.emitAdd(targetRegister, tmpRegister);
      break;
    case Operation::SUBSTRACT:
      generator.emitAdd(targetRegister, tmpRegister);
      break;
    default:
      THROW("Unrecognised operation " << static_cast<char>(op));
  }
}

} /* namespace ast */
