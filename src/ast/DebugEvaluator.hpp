#pragma once

#include <cstddef>

#include "InterfaceEvaluator.hpp"
#include "nodes.hpp"
#include "core/errors.hpp"

namespace ast
{

class DebugEvaluator: public InterfaceEvaluator<DebugEvaluator>
{
public:
#define METHOD_NOT_DEFINED(T) uint64_t evaluate(const T &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
#define X(node, str) METHOD_NOT_DEFINED(node)
PURE_NODE_LIST
#undef X
#undef METHOD_NOT_DEFINED

private:
  size_t _depth = 0;
};

} /* namespace ast */
