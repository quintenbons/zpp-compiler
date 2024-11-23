#pragma once

#include <sstream>
#include <variant>
#include <memory>
#include <string_view>
#include <vector>

#include "dbg/errors.hpp"
#include "scopes/scopeStack.hpp"
#include "scopes/types.hpp"

namespace ast
{

static constexpr char ENDL = '\n';

enum class LevelSpecifier { Public, Protected, Private };

struct Type
{
  std::string_view name;
  int pointerDepth;
  const scopes::TypeDescription* description{};

  void decorate(scopes::Scope &_currentScope)
  {
    description = &_currentScope.findType(name);
  }

  inline std::string fullName() const
  {
    return std::string(name) + std::string(pointerDepth, '*');
  }
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

  void genCode(std::ostream &ss) const
  {
    // charger dans le registre eax la val de expression
    ss << "  ret" << ENDL;
  }
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

struct Attribute
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

  void genCode(std::ostream &ss) const
  {
    std::stringstream asmCode;
    ss << ENDL;
    ss << name << ":" << ENDL;
    for (auto &instrVariant: body)
    {
      std::visit([&](auto &&instr) { instr.genCode(ss); }, instrVariant);
    }
  };
};

using Method = Function;

struct AccessSpecifier
{
  LevelSpecifier level;
};

struct Class
{
  std::string_view name;
  std::vector<std::pair<Attribute, AccessSpecifier>> attributes;
  std::vector<std::pair<Method, AccessSpecifier>> methods;
};

struct TranslationUnit {
  std::vector<Function> functions;
  std::vector<Class> classes;

  inline bool isDecorated() const
  {
    return true;
  }

  inline std::string genAsm_x86_64() const
  {
    std::stringstream asmCode;

    asmCode << ENDL;
    asmCode << "section .data" << ENDL;

    asmCode << ENDL;
    asmCode << "section .rodata" << ENDL;

    asmCode << ENDL;
    asmCode << "section .bss" << ENDL;

    asmCode << ENDL;
    asmCode << "section .text" << ENDL;

    asmCode << ENDL;
    asmCode << "  global _start" << ENDL;

    asmCode << ENDL;
    asmCode << "_start:" << ENDL;
    asmCode << "  call main" << ENDL;
    asmCode << "  mov rax, 60                  ; Syscall number for exit (60)" << ENDL;
    asmCode << "  xor rdi, rdi                 ; Exit code (0)" << ENDL;
    asmCode << "  syscall                      ; Make the syscall" << ENDL;

    for (auto &func: functions)
    {
      func.genCode(asmCode);
    }

    for (auto &classNode: classes)
    {
      (void) classNode;
      TODO("Implement classNodes");
    }

    return asmCode.str();
  }
};

#define PURE_NODE_LIST \
  X(Type, "Node_Type") \
  X(NumberLiteral, "Node_NumberLiteral") \
  X(ReturnStatement, "Node_ReturnStatement") \
  X(InstructionList, "Node_InstructionList") \
  X(FunctionParameter, "Node_FunctionParameter") \
  X(FunctionParameterList, "Node_FunctionParameter") \
  X(Function, "Node_Function") \
  X(LevelSpecifier, "Node_ClassLevelSpecifier") \
  X(Attribute, "Node_ClassAttribute") \
  X(Class, "Node_Class") \
  X(TranslationUnit, "Node_TranslationUnit")

#define VARIANT_NODE_LIST \
  X(Expression, "Node_Expression") \
  X(Instruction, "Node_Instruction")

#define NODE_LIST \
  PURE_NODE_LIST \
  VARIANT_NODE_LIST

#define ALL_NODE_LIST \
  NODE_LIST \
  X(CodeBlock, "Node_CodeBlock")

#define X(node, str) inline const char *nodeToStr(const node&) { return str; }
NODE_LIST
#undef X

#define X(node, str) inline std::ostream& operator<<(std::ostream& os, const node &token) { return os << nodeToStr(token); }
NODE_LIST
#undef X

} /* namespace ast */
