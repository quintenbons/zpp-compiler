#include "ast/nodes/nodes.h"
#include "ast/scopes/registers.hpp"

namespace ast {

Assign::Assign(Variable lhs, Expression rhs)
      : lhs(std::make_unique<Variable>(std::move(lhs)))
      , rhs(std::make_unique<Expression>(std::move(rhs))) {}

Assign::Assign(const Assign &other)
      : lhs(std::make_unique<Variable>(*other.lhs))
      , rhs(std::make_unique<Expression>(*other.rhs)) {}

inline void Assign::debug(size_t depth) const {
  logNode(depth);
  lhs->debug(depth+1);
  rhs->debug(depth+1);
}

inline void Assign::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  lhs->decorate(scopeStack, scope);
  rhs->decorate(scopeStack, scope);
  // TODO check rhs can be implicitly casted to lhs's type
  // TODO evaluate type as lhs's type
}

inline void Assign::genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
  auto regGuard = generator.regSet().acquireGuard();
  loadValueInRegister(generator, regGuard->reg);
}

inline void Assign::loadValueInRegister(codegen::NasmGenerator_x86_64 &generator,
                                scopes::GeneralPurposeRegister targetRegister) const {
  rhs->loadValueInRegister(generator, targetRegister);
  auto &varDesc = *lhs->getVariableDescription();
  generator.emitStoreInMemory(varDesc.location, scopes::getProperRegisterFromID64(targetRegister));
}

} /* namespace ast */
