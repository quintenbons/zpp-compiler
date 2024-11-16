#pragma once

#include <cstddef>
#include <variant>
#include <type_traits>

#include "nodes.hpp"
#include "core/utils.hpp"
#include "core/errors.hpp"

namespace ast
{

class InterfaceEvaluator
{
public:
#define METHOD_NOT_DEFINED(T) \
uint64_t operator()(const T &) { THROW("[" << __PRETTY_FUNCTION__ << "] Not implemented"); }
  METHOD_NOT_DEFINED(NumberLiteral);
  METHOD_NOT_DEFINED(Type);
  METHOD_NOT_DEFINED(Expression);
  METHOD_NOT_DEFINED(ReturnStatement);
  METHOD_NOT_DEFINED(Instruction);
  METHOD_NOT_DEFINED(InstructionList);
  // METHOD_NOT_DEFINED(CodeBlock); // same as InstructionList
  METHOD_NOT_DEFINED(FunctionParameter);
  METHOD_NOT_DEFINED(Function);
  METHOD_NOT_DEFINED(TranslationUnit);
#undef METHOD_NOT_DEFINED
};

template <typename EvaluatorT, typename AstNodeT>
void evaluate(std::function<EvaluatorT()> createEvaluator, AstNodeT &astNode)
{
  EvaluatorT evaluator = createEvaluator();
  if constexpr (utils::is_variant_v<AstNodeT>)
  {
    std::visit(evaluator, astNode);
  }
  else
  {
    evaluator(astNode);
  }
}

} /* namespace ast */
