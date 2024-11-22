#pragma once

#include "InterfaceEvaluator.hpp"
#include "nodes.hpp"
#include "dbg/errors.hpp"

namespace ast
{

class MockEvaluator: public InterfaceEvaluator<MockEvaluator>
{
  friend class InterfaceEvaluator<MockEvaluator>;

protected:
#define METHOD_NOT_DEFINED(T) void evaluate(const T &node) { THROW("[" << node << "] Evalutaion not implemented in MockEvaluator"); }
#define X(node, str) METHOD_NOT_DEFINED(node)
NODE_LIST
#undef X
#undef METHOD_NOT_DEFINED

private:
};

class MockEvaluatorConst: public InterfaceEvaluatorConst<MockEvaluatorConst>
{
  friend class InterfaceEvaluatorConst<MockEvaluatorConst>;

protected:
#define METHOD_NOT_DEFINED(T) void evaluate(const T &node) { THROW("[" << node << "] Evalutaion not implemented in MockEvaluator"); }
#define X(node, str) METHOD_NOT_DEFINED(node)
NODE_LIST
#undef X
#undef METHOD_NOT_DEFINED

private:
};

} /* namespace ast */
