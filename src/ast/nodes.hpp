#pragma once

#include <cstddef>
#include <format>
#include <ostream>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>
#include <ranges>

#include "interface/AstNode.hpp"
#include "codegen/generate.hpp"
#include "ast/scopes/scopeStack.hpp"
#include "ast/scopes/registers.hpp"
#include "dbg/utils.hpp"
#include "dbg/errors.hpp"
#include "scopes/scopeStack.hpp"
#include "scopes/types.hpp"

namespace ast {

namespace {
  static constexpr auto ENDL = codegen::NasmGenerator_x86_64::ENDL;
  static constexpr auto INDENT = codegen::NasmGenerator_x86_64::INDENT;
}

enum class Visibility { Public, Protected, Private };
constexpr Visibility allVisibilities[] = { Visibility::Public, Visibility::Protected, Visibility::Private };

class Type : public interface::AstNode<Type>
{
public:
  static constexpr const char *node_name = "Node_Type";

public:
  Type(std::string_view name, int pointerDepth): name(name), pointerDepth(pointerDepth) {}

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    (void) scopeStack;
    description = &scope.findType(name);
  }

  inline std::string fullName() const {
    return std::string(name) + std::string(pointerDepth, '*');
  }

  inline void debug(size_t depth) const {
    logNode(depth, fullName());
  }

private:
  std::string_view name;
  int pointerDepth;
  const scopes::TypeDescription *description{};
};

class NumberLiteral : public interface::AstNode<NumberLiteral>
{
public:
  static constexpr const char *node_name = "Node_NumberLiteral";
  using UnderlyingT = uint64_t;

public:
  NumberLiteral(UnderlyingT number): number(number) {}

  inline void debug(size_t depth) const {
    logNode(depth, number);
  }

  void loadValueInRegister(codegen::NasmGenerator_x86_64 &_evaluator, scopes::Register targetRegister) const {
    _evaluator << INDENT << "mov " << scopes::regToStr(targetRegister) << ", " << number << ENDL;
  }

private:
  UnderlyingT number;
};

class StringLiteral : public interface::AstNode<StringLiteral> {
public:
  static constexpr const char *node_name = "Node_StringLiteral";

public:
  StringLiteral(std::string_view content): content(content) {}

  inline void debug(size_t depth) const {
    logNode(depth, content);
  };

  std::string_view getContent() const {
    return content;
  }

private:
  std::string content;
};

class Expression : public interface::AstNode<Expression> {
public:
  static constexpr const char *node_name = "Node_Expression";
  using ExpressionVariant = std::variant<
    NumberLiteral
  >;

public:
  template<typename T>
  Expression(T &&expr): expr(std::forward<T>(expr)) {}

  inline void debug(size_t depth) const {
    std::visit([depth](const auto &node) { node.debug(depth); }, expr);
  }

  void loadValueInRegister(codegen::NasmGenerator_x86_64 &_evaluator, scopes::Register targetRegister) const {
    std::visit( [&_evaluator, targetRegister](auto &&expr) { expr.loadValueInRegister(_evaluator, targetRegister); }, expr);
  }

private:
  ExpressionVariant expr;
};

class ReturnStatement : public interface::AstNode<ReturnStatement> {
public:
  static constexpr const char *node_name = "Node_ReturnStatement";

public:
  ReturnStatement(Expression &&expression): expression(std::move(expression)) {}

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &_evaluator) const {
    expression.loadValueInRegister(_evaluator, scopes::returnRegister);
    _evaluator.emitReturnInstruction();
  }

  inline void debug(size_t depth) const {
    logNode(depth);
    expression.debug(depth);
  }

private:
  Expression expression;
};

class InlineAsmStatement : public interface::AstNode<InlineAsmStatement> {
public:
  static constexpr const char *node_name = "Node_InlineAsmStatement";
  using Register = scopes::Register;

  struct BindingRequest {
    scopes::Register registerTo;
    std::string varIdentifier;
  };

public:
  InlineAsmStatement(StringLiteral &&asmBlock, std::vector<BindingRequest> &&requests)
  : asmBlock(std::move(asmBlock)), requests(std::move(requests))
  {}

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &_evaluator) const {
    _evaluator << ";-- START -- asm binding requests" << ENDL;
    // TODO: not assume that every variable is always in rdx

    for (auto &request : requests) {
      LOG("generating code for request " << request.registerTo);
      _evaluator << INDENT
         << std::format("mov {}, {}", regToStr(request.registerTo),
                          regToStr(Register::REG_RAX))
         << ENDL;
    }

    _evaluator << ";-- START -- user defined" << ENDL;
    _evaluator << asmBlock.getContent() << ENDL;
    _evaluator << ";-- END -- user defined" << ENDL;
  }

  inline void debug(size_t depth) const {
    logNode(depth, "Register binding request count: ", requests.size());
    for (auto &request : requests) {
      LOG_DEBUG(INDENT_D(depth) << "[Request] " << request.registerTo << "(" << request.varIdentifier << ")");
    }
    asmBlock.debug(depth + 1);
  }

private:
  StringLiteral asmBlock;
  std::vector<BindingRequest> requests;
};

class Instruction : public interface::AstNode<Instruction> {
public:
  static constexpr const char *node_name = "Node_Instruction";
  using InstructionVariant = std::variant<
    ReturnStatement,
    InlineAsmStatement
    // Declaration,
    // Definition,
  >;

public:
  template<typename T>
  Instruction(T &&instr): instr(std::forward<T>(instr)) {}

  inline void debug(size_t depth) const {
    std::visit([depth](const auto &node) { node.debug(depth); }, instr);
  }

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const {
    std::visit([&evaluator](const auto &node) { node.genAsm_x86_64(evaluator); }, instr);
  }

private:
  InstructionVariant instr;
};

class InstructionList : public interface::AstNode<InstructionList> {
public:
  static constexpr const char *node_name = "Node_InstructionList";

public:
  InstructionList(std::vector<Instruction> &&instructions): instructions(std::move(instructions)) {}

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const {
    for (const auto &instr : instructions) {
      instr.genAsm_x86_64(evaluator);
    }
  }

  inline void debug(size_t depth) const {
    logNode(depth, "InstructionCount: ", instructions.size());
    for (const auto &instr : instructions) {
      instr.debug(depth + 1);
    }
  }

private:
  std::vector<Instruction> instructions;
};

class FunctionParameter : public interface::AstNode<FunctionParameter> {
public:
  static constexpr const char *node_name = "Node_FunctionParameter";

public:
  FunctionParameter(Type &&type, std::string_view name): type(type), name(name) {}

  inline void debug(size_t depth) const {
    logNode(depth, "Type: ", type.fullName(), " ; Name: ", name);
  }

private:
  Type type;
  std::string_view name;
};

class Attribute : public interface::AstNode<Attribute> {
public:
  static constexpr const char *node_name = "Node_ClassAttribute";

public:
  Attribute(Type &&type, std::string_view name): type(type), name(name) {}

  inline void debug(size_t depth) const {
    logNode(depth, "Type: ", type.fullName(), " ; Name: ", name);
  }

private:
  Type type;
  std::string_view name;
};

class FunctionParameterList : public interface::AstNode<FunctionParameterList> {
public:
  static constexpr const char *node_name = "Node_FunctionParameterList";

public:
  FunctionParameterList(std::vector<FunctionParameter> &&parameters): parameters(parameters) {}

  inline void debug(size_t depth) const {
    logNode(depth, "ParameterCount: ", parameters.size());
    for (const auto &param : parameters) {
      param.debug(depth + 1);
    }
  }

  inline auto size() const { return parameters.size(); }

private:
  std::vector<FunctionParameter> parameters;
};

class Function : public interface::AstNode<Function> {
public:
  static constexpr const char *node_name = "Node_Function";

public:
  Function(Type &&returnType, std::string_view name, FunctionParameterList &&params, InstructionList &&body)
  : returnType(returnType), name(name), params(params), body(body)
  {}

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &_evaluator) const {
    _evaluator.emitGlobalDirective(name);
    _evaluator.emitFunctionLabel(name);
    body.genAsm_x86_64(_evaluator);
    _evaluator.emitReturnInstruction();
  };

  inline void debug(size_t depth) const {
    logNode(depth, "ReturnType: ", returnType.fullName(), " ; Name: ", name, " ; ParamCount: ", params.size());
    returnType.debug(depth + 1);
    params.debug(depth + 1);
    body.debug(depth + 1);
  }

private:
  Type returnType;
  std::string_view name;
  FunctionParameterList params;
  InstructionList body;
};

class Method : public interface::AstNode<Method> {
public:
  static constexpr const char *node_name = "Node_ClassMethod";

public:
  Method(Type &&returnType, std::string_view name, FunctionParameterList &&params, InstructionList &&body)
  : returnType(returnType), name(name), params(params), body(body)
  {}

  inline void debug(size_t depth) const {
    logNode(depth, "ReturnType: ", returnType.fullName(), " ; Name: ", name, " ; ParamCount: ", params.size());
    returnType.debug(depth + 1);
    params.debug(depth + 1);
    body.debug(depth + 1);
  }

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

  inline void debug(size_t depth) const {
    switch (level) {
    case Visibility::Private:
      logNode(depth, "Visibility: Private");
      break;
    case Visibility::Protected:
      logNode(depth, "Visibility: Protected");
      break;
    case Visibility::Public:
      logNode(depth, "Visibility: Public");
      break;
    }
  }

  inline bool operator==(const Visibility vis) const {
    return level == vis;
  }

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
  : name(name) , attributes(attributes) , methods(methods)
  {}

  inline void debug(size_t depth) const {
    logNode(depth, "Name: ", name, " ; Members: ", attributes.size() + methods.size());
    for (const auto &visibility : allVisibilities)
    {
      AccessSpecifier(visibility).debug(depth);
      auto filteredAttrs = attributes | std::views::filter([visibility](const auto &pair){ return pair.second == visibility; });
      auto filteredMethods = methods | std::views::filter([visibility](const auto &pair){ return pair.second == visibility; });
      for (const auto &[attribute, accessSpecifier] : filteredAttrs) {
        attribute.debug(depth + 1);
      }
      for (const auto &[method, accessSpecifier] : filteredMethods) {
        method.debug(depth + 1);
      }
    }
  }

private:
  std::string_view name;
  AttributeList attributes;
  MethodList methods;
};

class TranslationUnit : public interface::AstNode<TranslationUnit> {
public:
  static constexpr const char *node_name = "Node_TranslationUnit";

public:
  TranslationUnit(std::vector<Function> &&functions, std::vector<Class> &&classes)
  : functions(functions), classes(classes)
  {}

  inline bool isDecorated() const { return true; }

  inline std::string genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const {
    for (auto &func : functions) {
      func.genAsm_x86_64(evaluator);
    }

    for (auto &classNode : classes) {
      (void)classNode;
      TODO("Implement classNodes");
    }

    std::stringstream asmCode;
    evaluator.generateAsmCode(asmCode);

    return asmCode.str();
  }

  inline void debug(size_t depth) const {
    logNode(depth, "Function count: ", functions.size());
    for (const auto &funcNode : functions) {
      funcNode.debug(depth + 1);
    }

    logNode(depth, "Class count: ", classes.size());
    for (const auto &classNode : classes) {
      classNode.debug(depth + 1);
    }
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    (void) scopeStack;
    (void) scope;
  }

private:
  std::vector<Function> functions;
  std::vector<Class> classes;
};

#define Y(T) X(T, T::node_name)
#define PURE_NODE_LIST                                                         \
  Y(Type)                                                                      \
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
