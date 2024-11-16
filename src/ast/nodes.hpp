#pragma once

#include <variant>
#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

namespace ast
{

struct Type
{
  std::string_view name;
};

struct NumberLiteral
{
  using UnderlyingT = uint64_t;

  UnderlyingT number;
};

using Expression = std::variant<
  NumberLiteral
>;

struct ReturnStatement
{
  std::unique_ptr<Expression> expression;
};

using Instruction = std::variant<
  ReturnStatement
  // Declaration,
  // Definition,
>;

using InstructionList = std::vector<Instruction>;

using CodeBlock = InstructionList;

struct FunctionParameter
{
  Type type;
  std::string_view name;
};

using FunctionParameterList = std::vector<FunctionParameter>;

struct Function
{
  Type returnType;
  std::string_view name;
  FunctionParameterList parameters;
  CodeBlock body;
};

using TranslationUnit = std::vector<Function>;

#define PURE_NODE_LIST \
  X(Type, "Node_Type") \
  X(NumberLiteral, "Node_NumberLiteral") \
  X(Expression, "Node_Expression") \
  X(ReturnStatement, "Node_ReturnStatement") \
  X(Instruction, "Node_Instruction") \
  X(InstructionList, "Node_InstructionList") \
  X(FunctionParameter, "Node_FunctionParameter") \
  X(Function, "Node_Function") \
  X(TranslationUnit, "Node_TranslationUnit")

#define NODE_LIST \
  PURE_NODE_LIST \
  X(CodeBlock, "Node_CodeBlock")

#define X(node, str) inline const char *nodeToStr(const node&) { return str; }
PURE_NODE_LIST
#undef X

#define X(node, str) inline std::ostream& operator<<(std::ostream& os, const node &token) { return os << nodeToStr(token); }
PURE_NODE_LIST
#undef X

} /* namespace ast */
