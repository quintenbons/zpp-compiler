#include "ast/nodes/nodes.h"
#include <iterator>

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
  (void) generator;
  TODO("Implement " << __PRETTY_FUNCTION__);
}

inline void Assign::loadValueInRegister(codegen::NasmGenerator_x86_64 &generator,
                                scopes::GeneralPurposeRegister targetRegister) const {
  (void) generator;
  (void) targetRegister;
  TODO("Implement " << __PRETTY_FUNCTION__);
}

} /* namespace ast */
