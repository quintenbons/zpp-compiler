#include "ast/scopes/registers.hpp"
#include "codegen/generate.hpp"
#include "dbg/logger.hpp"
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
  auto tgtRegByte = scopes::getProperRegisterFromID64(targetRegister, 1);

  switch (op) {
    case Operation::ADD:
      generator.emitAdd(properTargetReg, properTmpReg);
      break;
    case Operation::SUBSTRACT:
      generator.emitSub(properTargetReg, properTmpReg);
      break;
    case Operation::CMP_EQ:
      generator.emitCmp(properTargetReg, properTmpReg);
      generator.emitSetCC(tgtRegByte, codegen::CMP_OPERATION::EQ);
      break;
    case Operation::CMP_NEQ:
      generator.emitCmp(properTargetReg, properTmpReg);
      generator.emitSetCC(tgtRegByte, codegen::CMP_OPERATION::NEQ);
      break;
    case Operation::CMP_LEQ:
      generator.emitCmp(properTargetReg, properTmpReg);
      generator.emitSetCC(tgtRegByte, codegen::CMP_OPERATION::LEQ);
      break;
    case Operation::CMP_GEQ:
      generator.emitCmp(properTargetReg, properTmpReg);
      generator.emitSetCC(tgtRegByte, codegen::CMP_OPERATION::GEQ);
      break;
    case Operation::CMP_LT:
      generator.emitCmp(properTargetReg, properTmpReg);
      generator.emitSetCC(tgtRegByte, codegen::CMP_OPERATION::LT);
      break;
    case Operation::CMP_GT:
      generator.emitCmp(properTargetReg, properTmpReg);
      generator.emitSetCC(tgtRegByte, codegen::CMP_OPERATION::GT);
      break;
    default:
      THROW("Unrecognised operation " << static_cast<char>(op));
  }
}

} /* namespace ast */
