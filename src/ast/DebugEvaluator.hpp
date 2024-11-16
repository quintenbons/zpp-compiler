#pragma once

#include <cstddef>

#include "InterfaceEvaluator.hpp"
#include "nodes.hpp"
#include "core/errors.hpp"

namespace ast
{

class DebugEvaluator: public InterfaceEvaluator<DebugEvaluator>
{
  friend class InterfaceEvaluator<DebugEvaluator>;

protected:
void evaluate(const Type &node)
{
  logNode(node, node.fullName());
}

void evaluate(const NumberLiteral &node)
{
  logNode(node, node.number);
}

void evaluate(const ReturnStatement &node)
{
  logNode(node);
  _depth++;
  (*this)(*node.expression);
  _depth--;
}

void evaluate(const InstructionList &node)
{
  logNode(node, "InstructionCount: ", node.size());
  _depth++;
  for (const auto &instr: node) (*this)(instr);
  _depth--;
}

void evaluate(const FunctionParameter &node)
{
  logNode(node, "Type: ", node.type.fullName(), " ; Name: ", node.name);
}

void evaluate(const FunctionParameterList &node)
{
  logNode(node, "ParameterCount: ", node.size());
  _depth++;
  for (const auto &paramNode: node) (*this)(paramNode);
  _depth--;
}

void evaluate(const Function &node)
{
  logNode(node, "ReturnType: ", node.returnType.fullName(), " ; Name: ", node.name, " ; ParamCount: ", node.parameters.size());
  _depth++;
  (*this)(node.returnType);
  (*this)(node.parameters);
  (*this)(node.body);
  _depth--;
}

void evaluate(const TranslationUnit &node)
{
  logNode(node, "Function count: ", node.size());
  _depth++;
  for (const auto &funcNode: node) (*this)(funcNode);
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
    LOG(ss.str());
  }

  size_t _depth = 0;
};


} /* namespace ast */
