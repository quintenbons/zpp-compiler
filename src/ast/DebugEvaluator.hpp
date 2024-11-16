#pragma once

#include <cstddef>

#include "InterfaceEvaluator.hpp"
#include "nodes.hpp"
#include "core/errors.hpp"

namespace ast
{

class DebugEvaluator: public InterfaceEvaluator
{
public:
  // uint64_t operator()(const T&) const { THROW("[" << __PRETTY_FUNCTION__ << "] Not implemented"); }

private:
  size_t _depth = 0;
};

} /* namespace ast */
