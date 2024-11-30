#pragma once

#include <cstddef>
#include <sstream>

#include "dbg/logger.hpp"
#include "dbg/utils.hpp"

namespace interface
{

template <typename T>
concept IsAstNodeT = requires(
  const T &constObj,
  T &obj,
  size_t sizeT
  // scopes::ScopeStack &scopeStackT,
  // scopes::Scope &scopeT
) {
  { T::node_name } -> std::same_as<const char*>;
  { constObj.debug(sizeT) } -> std::same_as<void>;
  // { obj.decorate(scopeStackT, scopeT) } -> std::same_as<void>;
};

template <typename Derived>
struct AstNode {
  // Maybe later
  // AstNode() = delete;
  // AstNode(const AstNode&) = delete;
  // AstNode& operator=(const AstNode&) = delete;

protected:
  template<typename... Ts>
  static inline void logNode(size_t depth, Ts &&... args)
  {
    std::stringstream ss;
    ss << std::string(depth * 2, ' ') << "[" << Derived::node_name << "] ";
    if constexpr (sizeof...(args) > 0)
    {
      (ss << ... << args);
    }
    LOG_DEBUG(ss.str());
  }

private:
  void enforceInterface() { static_assert(IsAstNodeT<Derived>, "Class " STRINGIFY(Derived) " does not implement AstNode interface"); }
};

} /* namespace interface */
