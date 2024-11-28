#pragma once

#include <cstddef>
#include <stdexcept>

#include "nodes.hpp"
#include "dbg/logger.hpp"

namespace ast
{

template <typename NodeType>
class AstNode;

class DebugEvaluator
{
public:
template<typename NodeT, typename... Ts>
void logNode(const AstNode<NodeT> &node, Ts &&... args) {
  std::stringstream ss;
  ss << std::string(_depth * 2, ' ') << "[" << node << "] ";
  if constexpr (sizeof...(args) > 0)
  {
    (ss << ... << args);
  }
  LOG(ss.str());
}

inline void increaseDepth() {
  _depth++;
}

inline void decreaseDepth() {
  if (_depth == 0)
    throw ::std::invalid_argument("Depth cannot be decreased");
  _depth--;
}

inline size_t getDepth() const {
  return _depth;
}

private:
  size_t _depth = 0;
};


} /* namespace ast */
