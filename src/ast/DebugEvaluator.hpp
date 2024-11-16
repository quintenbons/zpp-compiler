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
void evaluate(const Type &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const NumberLiteral &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const Expression &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const ReturnStatement &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const Instruction &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const InstructionList &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const FunctionParameter &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const Function &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }
void evaluate(const TranslationUnit &node) { THROW("[" << node << "] Evalutaion not implemented in DebugEvaluator"); }

private:
  size_t _depth = 0;
};

} /* namespace ast */
