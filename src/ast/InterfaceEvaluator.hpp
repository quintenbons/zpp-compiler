#pragma once

#include <cstddef>
#include <variant>
#include <type_traits>

#include "nodes.hpp"
#include "dbg/utils.hpp"
#include "dbg/errors.hpp"

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

#define CRTP(T) inline void operator()(T &node) { return static_cast<Derived*>(this)->evaluate(node); }
#define X(node, str) CRTP(node)
PURE_NODE_LIST;
#undef X
#undef CRTP

inline void operator()(Expression &node) { return std::visit(*this, node); }
inline void operator()(Instruction &node) { return std::visit(*this, node); }

};

template<typename Derived>
class InterfaceEvaluatorConst
{
public:
  InterfaceEvaluatorConst()
  {
    static_assert(std::derived_from<Derived, InterfaceEvaluatorConst>, "CRTP pattern not working: " TOSTRING(Derived) " should derive from " TOSTRING(InterfaceEvaluator<Derived>) );
  }

#define CRTP(T) inline void operator()(const T &node) { return static_cast<Derived*>(this)->evaluate(node); }
#define X(node, str) CRTP(node)
PURE_NODE_LIST;
#undef X
#undef CRTP

inline void operator()(const Expression &node) { return std::visit(*this, node); }
inline void operator()(const Instruction &node) { return std::visit(*this, node); }

};

} /* namespace ast */
