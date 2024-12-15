#include "nodes.h"

namespace ast {

inline void BinaryOperation::loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::GeneralPurposeRegister targetRegister) const {
  auto gRhsRegister = generator.regSet().acquireGuard();
  if (!gRhsRegister) TODO("Not enough registers, memory fallback not implemented");

  lhs->loadValueInRegister(generator, targetRegister);
  rhs->loadValueInRegister(generator, gRhsRegister->reg);
}

} /* namespace ast */
