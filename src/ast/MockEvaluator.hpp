#pragma once

#include <cstddef>

#include "InterfaceEvaluator.hpp"
#include "nodes.hpp"
#include "core/errors.hpp"

namespace ast
{

class MockEvaluator: public InterfaceEvaluator<MockEvaluator>
{
public:
#define METHOD_NOT_DEFINED(T) void evaluate(const T &node) { THROW("[" << node << "] Evalutaion not implemented in MockEvaluator"); }
#define X(node, str) METHOD_NOT_DEFINED(node)
PURE_NODE_LIST
#undef X
#undef METHOD_NOT_DEFINED

private:
  size_t _depth = 0;
};

} /* namespace ast */
