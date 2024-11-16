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

} /* namespace ast */
