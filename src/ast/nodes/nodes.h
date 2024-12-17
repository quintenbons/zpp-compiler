#pragma once

#include <cstddef>
#include <string_view>
#include <variant>
#include <vector>

#include "ast/literalTypes.hpp"
#include "ast/scopes/registers.hpp"
#include "ast/scopes/scopeStack.hpp"
#include "ast/scopes/types.hpp"
#include "codegen/generate.hpp"
#include "dbg/errors.hpp"
#include "interface/AstNode.hpp"

namespace ast {

enum class Visibility { Public, Protected, Private };
constexpr Visibility allVisibilities[] = {
    Visibility::Public, Visibility::Protected, Visibility::Private};

class Expression;
class FunctionCall;

class Type : public interface::AstNode<Type> {
public:
  static constexpr const char *node_name = "Node_Type";

public:
  Type(std::string_view name, int pointerDepth)
      : name(name), pointerDepth(pointerDepth) {}

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);
  inline void debug(size_t depth) const;

  inline const scopes::TypeDescription *getTypeDescription() const {
    if (description)
      return description;
    THROW("TypeDescription not set");
  }
  inline std::string fullName() const {
    return std::string(name) + std::string(pointerDepth, '*');
  }

private:
  std::string_view name;
  int pointerDepth;
  const scopes::TypeDescription *description = nullptr;
};

class Variable : public interface::AstNode<Variable> {
public:
  static constexpr const char *node_name = "Node_Variable";

public:
  Variable(std::string_view &&name) : name(name) {}

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void debug(size_t depth) const;

  inline void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator,
                                  scopes::GeneralPurposeRegister targetRegister) const {
    const scopes::byteSize_t size = (*getVariableDescription()->typeDescription)->byteSize;
    generator.emitLoadFromMemory(scopes::getProperRegisterFromID64(targetRegister, size), getVariableDescription()->location);
  }

  inline std::string_view getName() const { return name; }

  inline const scopes::VariableDescription *getVariableDescription() const {
    if (description)
      return description;
    THROW("VariableDescription not set");
  }

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    (void)generator;
    THROW("Variable genAsm_x86_64 should not be called");
  }

private:
  std::string_view name;
  const scopes::VariableDescription *description = nullptr;
};

class NumberLiteral : public interface::AstNode<NumberLiteral> {
public:
  static constexpr const char *node_name = "Node_NumberLiteral";

public:
  NumberLiteral(NumberLiteralUnderlyingType number) : number(number) {}

  inline void debug(size_t depth) const;
  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator,
                           scopes::GeneralPurposeRegister targetRegister) const {
    generator.emitLoadNumberLiteral(scopes::getProperRegisterFromID64(targetRegister, NumberLiteralUnderlyingTypeSize), number);
  }

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    (void)generator;
    THROW("NumberLiteral genAsm_x86_64 should not be called");
  }

private:
  NumberLiteralUnderlyingType number;
};

class StringLiteral : public interface::AstNode<StringLiteral> {
public:
  static constexpr const char *node_name = "Node_StringLiteral";

public:
  StringLiteral(std::string_view content) : content(content) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  std::string_view getContent() const { return content; }

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    (void)generator;
    THROW("StringLiteral genAsm_x86_64 should not be called");
  }

private:
  std::string content;
};

class FunctionCall : public interface::AstNode<FunctionCall> {
public:
  static constexpr const char *node_name = "Node_FunctionCall";

public:
  FunctionCall(std::string_view name, std::vector<Expression> &&arguments)
      : name(name), arguments(std::move(arguments)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const;

  inline void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator,
                                  scopes::GeneralPurposeRegister targetRegister) const {
    (void)generator;
    (void)targetRegister;
    THROW("FunctionCall loadValueInRegister Not implemented");
  }

private:
  std::string_view name;
  std::vector<Expression> arguments;
};

class BinaryOperation: interface::AstNode<BinaryOperation> {
public:
  enum class Operation: char { NOT_AN_OPERATION='N', ADD='+', SUBSTRACT='-', MULTIPLY='*', DIVIDE='/' };

  static constexpr const char *node_name = "Node_BinaryOperation";

public:
  BinaryOperation(Operation op, std::unique_ptr<Expression> &&lhs, std::unique_ptr<Expression> &&rhs)
    : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

  BinaryOperation(const BinaryOperation &other)
    : op(other.op), lhs(std::make_unique<Expression>(*other.lhs)), rhs(std::make_unique<Expression>(*other.rhs)) {}

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    (void)generator;
    THROW("BinaryOperation genAsm_x86_64 should not be called");
  }

  inline void debug(size_t depth) const;

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::GeneralPurposeRegister targetRegister) const;

private:
  Operation op;
  std::unique_ptr<Expression> lhs;
  std::unique_ptr<Expression> rhs;
};

class Expression : public interface::AstNode<Expression> {
public:
  static constexpr const char *node_name = "Node_Expression";
  using ExpressionVariant = std::variant<NumberLiteral, Variable, FunctionCall, BinaryOperation>;

public:
  Expression(const Expression &other) : expr(other.expr) {}
  Expression(const Expression &&other) : expr(std::move(other.expr)) {}
  Expression(NumberLiteral &&expr) : expr(std::move(expr)) {}
  Expression(Variable &&expr) : expr(std::move(expr)) {}
  Expression(FunctionCall &&expr) : expr(std::move(expr)) {}
  Expression(BinaryOperation &&expr) : expr(std::move(expr)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator,
                           scopes::GeneralPurposeRegister targetRegister) const {
    std::visit(
        [&generator, targetRegister](auto &&expr) {
          expr.loadValueInRegister(generator, targetRegister);
        },
        expr);
  }

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const;

private:
  ExpressionVariant expr;
};

class Declaration : public interface::AstNode<Declaration> {
public:
  static constexpr const char *node_name = "Node_Declaration";

public:
  Declaration(Type &&type, Variable &&variable)
      : type(std::move(type)), variable(variable) {}
  Declaration(Type &&type, Variable &&variable, Expression &&assignment)
      : type(std::move(type)), variable(variable),
        assignment(std::move(assignment)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const;

private:
  Type type;
  Variable variable;
  std::optional<Expression> assignment;
};

class ReturnStatement : public interface::AstNode<ReturnStatement> {
public:
  static constexpr const char *node_name = "Node_ReturnStatement";

public:
  ReturnStatement(Expression &&expression)
      : expression(std::move(expression)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const;

private:
  Expression expression;
};

class InlineAsmStatement : public interface::AstNode<InlineAsmStatement> {
public:
  static constexpr const char *node_name = "Node_InlineAsmStatement";

  struct BindingRequest {
    scopes::Register registerTo;
    std::string varIdentifier;
  };

public:
  InlineAsmStatement(StringLiteral &&asmBlock,
                     std::vector<BindingRequest> &&requests)
      : asmBlock(std::move(asmBlock)), requests(std::move(requests)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const;

private:
  StringLiteral asmBlock;
  std::vector<BindingRequest> requests;
};

class Instruction : public interface::AstNode<Instruction> {
public:
  static constexpr const char *node_name = "Node_Instruction";
  using InstructionVariant =
      std::variant<ReturnStatement, InlineAsmStatement, Declaration, Expression
                   // Definition,
                   >;

public:
  template <typename T>
  Instruction(T &&instr) : instr(std::forward<T>(instr)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const;

private:
  InstructionVariant instr;
};

class InstructionList : public interface::AstNode<InstructionList> {
public:
  static constexpr const char *node_name = "Node_InstructionList";

public:
  InstructionList(std::vector<Instruction> &&instructions)
      : instructions(std::move(instructions)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const;

private:
  std::vector<Instruction> instructions;
};

class FunctionParameter : public interface::AstNode<FunctionParameter> {
public:
  static constexpr const char *node_name = "Node_FunctionParameter";

public:
  FunctionParameter(Type &&type, Variable &&variable)
      : type(type), variable(variable) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline const scopes::TypeDescription *getTypeDescription() const {
    return type.getTypeDescription();
  }

private:
  Type type;
  Variable variable;
};

class Attribute : public interface::AstNode<Attribute> {
public:
  static constexpr const char *node_name = "Node_ClassAttribute";

public:
  Attribute(Type &&type, Variable &&variable) : type(type), variable(variable) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

private:
  Type type;
  Variable variable;
};

class FunctionParameterList : public interface::AstNode<FunctionParameterList> {
public:
  static constexpr const char *node_name = "Node_FunctionParameterList";

public:
  FunctionParameterList(std::vector<FunctionParameter> &&parameters)
      : parameters(parameters) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline auto size() const { return parameters.size(); }
  inline std::vector<FunctionParameter>::iterator begin() {
    return parameters.begin();
  }
  inline std::vector<FunctionParameter>::iterator end() {
    return parameters.end();
  }

private:
  std::vector<FunctionParameter> parameters;
};

class Function : public interface::AstNode<Function> {
public:
  static constexpr const char *node_name = "Node_Function";

public:
  Function(Type &&returnType, std::string_view name,
           FunctionParameterList &&params, InstructionList &&body)
      : returnType(returnType), name(name), params(params), body(std::move(body)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const;

private:
  Type returnType;
  std::string_view name;
  FunctionParameterList params;
  InstructionList body;
  const scopes::FunctionDescription *description = nullptr;
};

class Method : public interface::AstNode<Method> {
public:
  static constexpr const char *node_name = "Node_ClassMethod";

public:
  Method(Type &&returnType, std::string_view name,
         FunctionParameterList &&params, InstructionList &&body)
      : returnType(returnType), name(name), params(params), body(std::move(body)) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

private:
  Type returnType;
  std::string_view name;
  FunctionParameterList params;
  InstructionList body;
};

class AccessSpecifier : public interface::AstNode<AccessSpecifier> {
public:
  static constexpr const char *node_name = "Node_AccessSpecifier";

public:
  AccessSpecifier(Visibility level) : level(level) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline bool operator==(const Visibility vis) const { return level == vis; }

private:
  Visibility level;
};

class Class : public interface::AstNode<Class> {
public:
  static constexpr const char *node_name = "Node_Class";
  using AttributeList = std::vector<std::pair<Attribute, AccessSpecifier>>;
  using MethodList = std::vector<std::pair<Method, AccessSpecifier>>;

public:
  Class(std::string_view name, AttributeList &&attributes, MethodList &&methods)
      : name(name), attributes(attributes), methods(methods) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

private:
  std::string_view name;
  AttributeList attributes;
  MethodList methods;
};

class TranslationUnit : public interface::AstNode<TranslationUnit> {
public:
  static constexpr const char *node_name = "Node_TranslationUnit";

public:
  TranslationUnit(std::vector<Function> &&functions,
                  std::vector<Class> &&classes)
      : functions(functions), classes(classes) {}

  inline void debug(size_t depth) const;

  inline void decorate (scopes::ScopeStack &scopeStack, scopes::Scope &scope);

  inline std::string genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const;

  inline bool isDecorated() const { return true; }

private:
  std::vector<Function> functions;
  std::vector<Class> classes;
};

#define Y(T) X(T, T::node_name)
#define PURE_NODE_LIST                                                         \
  Y(Type)                                                                      \
  Y(Declaration)                                                               \
  Y(FunctionCall)                                                              \
  Y(BinaryOperation)                                                           \
  Y(NumberLiteral)                                                             \
  Y(StringLiteral)                                                             \
  Y(ReturnStatement)                                                           \
  Y(InlineAsmStatement)                                                        \
  Y(InstructionList)                                                           \
  Y(FunctionParameter)                                                         \
  Y(FunctionParameterList)                                                     \
  Y(Function)                                                                  \
  Y(Method)                                                                    \
  Y(AccessSpecifier)                                                           \
  Y(Attribute)                                                                 \
  Y(Class)                                                                     \
  Y(TranslationUnit)

#define VARIANT_NODE_LIST                                                      \
  Y(Expression)                                                                \
  Y(Instruction)

#define NODE_LIST                                                              \
  PURE_NODE_LIST                                                               \
  VARIANT_NODE_LIST

#define X(node, str)                                                           \
  inline const char *nodeToStr(const node &) { return str; }

NODE_LIST
#undef X

} /* namespace ast */
