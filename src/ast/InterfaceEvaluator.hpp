#pragma once

#include <cstddef>
#include <variant>
#include <type_traits>

#include "nodes.hpp"
#include "core/utils.hpp"
#include "core/errors.hpp"

namespace ast
{


template<typename Derived>
class InterfaceEvaluator
{
public:
InterfaceEvaluator()
{
  static_assert(std::derived_from<Derived, InterfaceEvaluator>, "CRTP pattern not working: " TOSTRING(Derived) " should derive from " TOSTRING(InterfaceEvaluator<Derived>) );
}

#define CRTP(T) uint64_t operator()(const T &node) { return static_cast<Derived*>(this)->evaluate(node); }
#define X(node, str) CRTP(node)
PURE_NODE_LIST;
#undef X
#undef CRTP
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
