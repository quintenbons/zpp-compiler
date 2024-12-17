#include <string_view>
#include <variant>
#include <vector>

#include "ast/scopes/scopeStack.hpp"
#include "ast/scopes/types.hpp"

#include "nodes.h"

namespace ast {
inline void Type::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  (void)scopeStack;
  description = scope.findType(name);
}

inline void Variable::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  (void)scopeStack;
  description = scope.findVariable(name);
}

inline void NumberLiteral::decorate(scopes::ScopeStack &scopeStack,
                             scopes::Scope &scope) {
  (void)scopeStack;
  (void)scope;
}

inline void StringLiteral::decorate(scopes::ScopeStack &scopeStack,
                             scopes::Scope &scope) {
  (void)scopeStack;
  (void)scope;
}

inline void Expression::decorate(scopes::ScopeStack &scopeStack,
                          scopes::Scope &scope) {
  std::visit(
      [&scopeStack, &scope](auto &node) { node.decorate(scopeStack, scope); },
      expr);
}

inline void FunctionCall::decorate(scopes::ScopeStack &scopeStack,
                            scopes::Scope &scope) {
  scope.findFunction(name);
  for (auto &arg : arguments) {
    arg.decorate(scopeStack, scope);
  }
}

inline void BinaryOperation::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  lhs->decorate(scopeStack, scope);
  rhs->decorate(scopeStack, scope);
  // TODO add binary operation return type to decoration based on lhs & rhs
}


inline void Declaration::decorate(scopes::ScopeStack &scopeStack,
                           scopes::Scope &scope) {
  type.decorate(scopeStack, scope);
  scopeStack.addLocalVariable(variable.getName(), type.getTypeDescription(),
                              scope);
  variable.decorate(scopeStack, scope);
}

inline void ReturnStatement::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  expression.decorate(scopeStack, scope);
}

inline void InlineAsmStatement::decorate(scopes::ScopeStack &scopeStack,
                                  scopes::Scope &scope) {
  asmBlock.decorate(scopeStack, scope);
}

inline void Instruction::decorate(scopes::ScopeStack &scopeStack,
                           scopes::Scope &scope) {
  std::visit(
      [&scopeStack, &scope](auto &node) { node.decorate(scopeStack, scope); },
      instr);
}

inline void FunctionParameter::decorate(scopes::ScopeStack &scopeStack,
                                 scopes::Scope &scope) {
  type.decorate(scopeStack, scope);
  // variable.decorate(scopeStack, scope);
}

inline void Attribute::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  type.decorate(scopeStack, scope);
  // variable.decorate(scopeStack, scope);
}

inline void FunctionParameterList::decorate(scopes::ScopeStack &scopeStack,
                                     scopes::Scope &scope) {
  for (auto &param : parameters) {
    param.decorate(scopeStack, scope);
  }
}

inline void CodeBlock::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  scopes::Scope &newScope = getOrCreateScope(scopeStack, scope);
  for (auto &instr : statements) {
    instr.decorate(scopeStack, newScope);
  }
}

inline void Statement::decorate(scopes::ScopeStack &scopeStack,
                          scopes::Scope &scope) {
  std::visit(
      [&scopeStack, &scope](auto &node) { node.decorate(scopeStack, scope); },
      statement);
}

inline void ConditionalStatement::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  condition.decorate(scopeStack, scope);
  ifBody.decorate(scopeStack, scope);
  if (elseBody.has_value()) {
    elseBody->decorate(scopeStack, scope);
  }
}

inline void Function::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  scopes::Scope &newScope = body.getOrCreateScope(scopeStack, scope);
  returnType.decorate(scopeStack, newScope);
  params.decorate(scopeStack, newScope);
  body.decorate(scopeStack, newScope);

  std::vector<const scopes::TypeDescription *> paramTypes;
  for (auto &param : params) {
    paramTypes.push_back(param.getTypeDescription());
  }
  scopeStack.addFunction(name, paramTypes, returnType.getTypeDescription(),
                         scope);
  description = scope.findFunction(name);
}

inline void Method::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  returnType.decorate(scopeStack, scope);
  params.decorate(scopeStack, scope);
  body.decorate(scopeStack, scope);
}

inline void AccessSpecifier::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  (void)scopeStack;
  (void)scope;
}

inline void Class::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  for (auto &attr : attributes) {
    attr.first.decorate(scopeStack, scope);
  }
  for (auto &method : methods) {
    method.first.decorate(scopeStack, scope);
  }
}

inline void TranslationUnit::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  for (auto &func : functions) {
    func.decorate(scopeStack, scope);
  }
  for (auto &classNode : classes) {
    classNode.decorate(scopeStack, scope);
  }
}

} /* namespace ast */
