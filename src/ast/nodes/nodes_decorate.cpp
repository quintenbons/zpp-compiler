#include <string_view>
#include <variant>
#include <vector>

#include "ast/scopes/scopeStack.hpp"
#include "ast/scopes/types.hpp"

#include "nodes.h"

namespace ast {
void Type::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  (void)scopeStack;
  description = scope.findType(name);
}

void Variable::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  (void)scopeStack;
  description = scope.findVariable(name);
}

void NumberLiteral::decorate(scopes::ScopeStack &scopeStack,
                             scopes::Scope &scope) {
  (void)scopeStack;
  (void)scope;
}

void StringLiteral::decorate(scopes::ScopeStack &scopeStack,
                             scopes::Scope &scope) {
  (void)scopeStack;
  (void)scope;
}

void Expression::decorate(scopes::ScopeStack &scopeStack,
                          scopes::Scope &scope) {
  std::visit(
      [&scopeStack, &scope](auto &node) { node.decorate(scopeStack, scope); },
      expr);
}

void FunctionCall::decorate(scopes::ScopeStack &scopeStack,
                            scopes::Scope &scope) {
  (void)scopeStack;
  (void)scope;
}

void Declaration::decorate(scopes::ScopeStack &scopeStack,
                           scopes::Scope &scope) {
  type.decorate(scopeStack, scope);
  scopeStack.addLocalVariable(variable.getName(), type.getTypeDescription(),
                              scope);
  variable.decorate(scopeStack, scope);
}

void ReturnStatement::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  expression.decorate(scopeStack, scope);
}

void InlineAsmStatement::decorate(scopes::ScopeStack &scopeStack,
                                  scopes::Scope &scope) {
  asmBlock.decorate(scopeStack, scope);
}

void Instruction::decorate(scopes::ScopeStack &scopeStack,
                           scopes::Scope &scope) {
  std::visit(
      [&scopeStack, &scope](auto &node) { node.decorate(scopeStack, scope); },
      instr);
}

void InstructionList::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  for (auto &instr : instructions) {
    instr.decorate(scopeStack, scope);
  }
}

void FunctionParameter::decorate(scopes::ScopeStack &scopeStack,
                                 scopes::Scope &scope) {
  type.decorate(scopeStack, scope);
}

void Attribute::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  type.decorate(scopeStack, scope);
}

void FunctionParameterList::decorate(scopes::ScopeStack &scopeStack,
                                     scopes::Scope &scope) {
  for (auto &param : parameters) {
    param.decorate(scopeStack, scope);
  }
}

void Function::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  scopes::Scope &newScope = scopeStack.createChildScope(&scope);
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

void Method::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  returnType.decorate(scopeStack, scope);
  params.decorate(scopeStack, scope);
  body.decorate(scopeStack, scope);
}

void AccessSpecifier::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  (void)scopeStack;
  (void)scope;
}

void Class::decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
  for (auto &attr : attributes) {
    attr.first.decorate(scopeStack, scope);
  }
  for (auto &method : methods) {
    method.first.decorate(scopeStack, scope);
  }
}

void TranslationUnit::decorate(scopes::ScopeStack &scopeStack,
                               scopes::Scope &scope) {
  for (auto &func : functions) {
    func.decorate(scopeStack, scope);
  }
  for (auto &classNode : classes) {
    classNode.decorate(scopeStack, scope);
  }
}

} /* namespace ast */
