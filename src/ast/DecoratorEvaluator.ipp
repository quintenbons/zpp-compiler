#pragma once

#include <cstddef>

#include "InterfaceEvaluator.hpp"
#include "nodes.hpp"
#include "scopes/scopeStack.hpp"

namespace ast
{

class DecoratorEvaluator: public InterfaceEvaluator<DecoratorEvaluator>
{
  friend class InterfaceEvaluator<DecoratorEvaluator>;

public:
  DecoratorEvaluator(scopes::ScopeStack &scopeStack)
  : _scopeStack{scopeStack}
  , _currentScope{_scopeStack.rootScope()}
  {
  }

protected:
void evaluate(Type &node)
{
  logNode(node, "Type: ", node.name);
  node.decorate(_currentScope);
}

void evaluate(NumberLiteral &node)
{
  logNode(node, node.number);
}

void evaluate(StringLiteral &node)
{
  logNode(node);
  _depth++;
  LOG_DEBUG(std::string(_depth * 2, ' ') << node.content);
  _depth--;
}

void evaluate(ReturnStatement &node)
{
  logNode(node);
  _depth++;
  (*this)(*node.expression);
  _depth--;
}

void evaluate(InlineAsmStatement &node)
{
  logNode(node);
  _depth++;
  (*this)(node.asmBlock);
  _depth--;
}

void evaluate(InstructionList &node)
{
  logNode(node, "InstructionCount: ", node.size());
  _depth++;
  for (auto &instr: node) (*this)(instr);
  _depth--;
}

void evaluate(FunctionParameter &node)
{
  logNode(node, "Type: ", node.type.fullName(), " ; Name: ", node.name);
}

void evaluate(FunctionParameterList &node)
{
  logNode(node, "ParameterCount: ", node.size());
  _depth++;
  for (auto &paramNode: node) (*this)(paramNode);
  _depth--;
}

void evaluate(Function &node)
{
  logNode(node, "ReturnType: ", node.returnType.fullName(), " ; Name: ", node.name, " ; ParamCount: ", node.parameters.size());
  _depth++;
  (*this)(node.returnType);
  (*this)(node.parameters);
  (*this)(node.body);
  _depth--;
}

void evaluate(TranslationUnit &node)
{
  logNode(node, "Function count: ", node.functions.size());
  _depth++;
  for (auto &funcNode: node.functions) (*this)(funcNode);
  // for (auto &classNode: node.classes) (*this)(classNode);
  _depth--;
}

private:
  template<typename NodeT, typename... Ts>
  void logNode(const NodeT &node, Ts &&... args) {
    std::stringstream ss;
    ss << std::string(_depth * 2, ' ') << "[" << node << "] ";
    if constexpr (sizeof...(args) > 0)
    {
      (ss << ... << args);
    }
    LOG_DEBUG(ss.str());
  }

  scopes::ScopeStack &_scopeStack;
  scopes::Scope &_currentScope;
  size_t _depth = 0;
};


} /* namespace ast */
