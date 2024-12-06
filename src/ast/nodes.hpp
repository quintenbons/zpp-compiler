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
#include "ast/litteralTypes.hpp"

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
    description = scope.findType(name);
  }

  inline std::string fullName() const {
    return std::string(name) + std::string(pointerDepth, '*');
  }

  inline void debug(size_t depth) const {
    logNode(depth, fullName());
    if (description)
      logDecoration(depth+1, "TypeDescription: ", description->name, " ; Size: ", description->byteSize, " ; Id: ", description->id);
  }

  inline const scopes::TypeDescription *getTypeDescription() const {
    if (description) return description;
    THROW("TypeDescription not set");
  }

private:
  std::string_view name;
  int pointerDepth;
  const scopes::TypeDescription *description = nullptr;
};

// TODO Variable as parameter
class Variable : public interface::AstNode<Variable>
{
public:
  static constexpr const char *node_name = "Node_Variable";

public:
  Variable(std::string_view &&name): name(name) {}

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    (void) scopeStack;
    description = scope.findVariable(name);
  }

  inline void debug(size_t depth) const {
    logNode(depth, name);
    if (description)
      logDecoration(depth+1, "VariableDescription: Id: ", description->variableId);
  }

  inline std::string_view getName() const {
    return name;
  }

  inline const scopes::VariableDescription *getVariableDescription() const {
    if (description) return description;
    THROW("VariableDescription not set");
  }

  inline void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::Register targetRegister) const {
    (void) generator;
    (void) targetRegister;
    THROW("variable loadValueInRegister Not implemented");
  }

private:
  std::string_view name;
  const scopes::VariableDescription *description = nullptr;
};

class NumberLiteral : public interface::AstNode<NumberLiteral>
{
public:
  static constexpr const char *node_name = "Node_NumberLiteral";

public:
  NumberLiteral(NumberLitteralUnderlyingType number): number(number) {}

  inline void debug(size_t depth) const {
    logNode(depth, number);
  }

  void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::Register targetRegister) const {
    generator.emitLoadNumberLitteral(targetRegister, number);
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    (void) scopeStack;
    (void) scope;
  }

private:
  NumberLitteralUnderlyingType number;
};

class StringLiteral : public interface::AstNode<StringLiteral> {
public:
  static constexpr const char *node_name = "Node_StringLiteral";

public:
  StringLiteral(std::string_view content): content(content) {}

  inline void debug(size_t depth) const {
    logNode(depth, content);
  };

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    (void) scopeStack;
    (void) scope;
  }

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
    NumberLiteral,
    Variable
    // FunctionCall
  >;

public:
  template<typename T>
  Expression(T &&expr): expr(std::forward<T>(expr)) {}

  inline void debug(size_t depth) const {
    std::visit([depth](const auto &node) { node.debug(depth); }, expr);
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    std::visit([&scopeStack, &scope](auto &node) { node.decorate(scopeStack, scope); }, expr);
  }

  void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::Register targetRegister) const {
    std::visit( [&generator, targetRegister](auto &&expr) { expr.loadValueInRegister(generator, targetRegister); }, expr);
  }

private:
  ExpressionVariant expr;
};

class FunctionCall : public interface::AstNode<FunctionCall> {
public:
  static constexpr const char *node_name = "Node_FunctionCall";

public:
  FunctionCall(std::string_view name, std::vector<Expression> &&arguments): name(name), arguments(std::move(arguments)) {}

  inline void debug(size_t depth) const {
    logNode(depth, name, " ; ArgumentCount: ", arguments.size());
    for (const auto &arg : arguments) {
      arg.debug(depth + 1);
    }
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    (void) scopeStack;
    (void) scope;
  }

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    (void) generator;
  }

  inline void loadValueInRegister(codegen::NasmGenerator_x86_64 &generator, scopes::Register targetRegister) const {
    (void) generator;
    (void) targetRegister;
    THROW("FunctionCall loadValueInRegister Not implemented");
  }

private:
  std::string_view name;
  std::vector<Expression> arguments;
};

class Declaration : public interface::AstNode<Declaration> {
public:
  static constexpr const char *node_name = "Node_Declaration";

public:
  Declaration(Type &&type, Variable &&variable): type(std::move(type)), variable(variable) {}
  Declaration(Type &&type, Variable &&variable, Expression &&assignment): type(std::move(type)), variable(variable), assignment(std::move(assignment)) {}

  inline void debug(size_t depth) const {
    logNode(depth, "Type: ", type.fullName(), " ; Assignment: ", assignment.has_value());
    variable.debug(depth + 1);
    if (assignment.has_value()) {
      assignment->debug(depth + 1);
    }
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    type.decorate(scopeStack, scope);
    scopeStack.addLocalVariable(variable.getName(), type.getTypeDescription(), scope);
    variable.decorate(scopeStack, scope);
  }

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    generator.emitDeclaration(variable.getVariableDescription()->location);
    if (assignment.has_value()) {
      assignment->loadValueInRegister(generator, scopes::Register::REG_RAX);
      generator.emitStoreInMemory(variable.getVariableDescription()->location, scopes::Register::REG_RAX);
    }
  }

private:
  Type type;
  Variable variable;
  std::optional<Expression> assignment;
};

class ReturnStatement : public interface::AstNode<ReturnStatement> {
public:
  static constexpr const char *node_name = "Node_ReturnStatement";

public:
  ReturnStatement(Expression &&expression): expression(std::move(expression)) {}

  inline void debug(size_t depth) const {
    logNode(depth);
    expression.debug(depth);
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    expression.decorate(scopeStack, scope);
  }

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    expression.loadValueInRegister(generator, scopes::returnRegister);
    generator.emitReturnInstruction();
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

  inline void debug(size_t depth) const {
    logNode(depth, "Register binding request count: ", requests.size());
    for (auto &request : requests) {
      LOG_DEBUG(INDENT_D(depth) << "[Request] " << request.registerTo << "(" << request.varIdentifier << ")");
    }
    asmBlock.debug(depth + 1);
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    asmBlock.decorate(scopeStack, scope);
  }

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    generator << ";-- START -- asm binding requests" << ENDL;
    // TODO: not assume that every variable is always in rdx

    for (auto &request : requests) {
      generator << INDENT
         << std::format("mov {}, {}", regToStr(request.registerTo),
                          regToStr(Register::REG_RAX))
         << ENDL;
    }

    generator << ";-- START -- user defined" << ENDL;
    generator << asmBlock.getContent() << ENDL;
    generator << ";-- END -- user defined" << ENDL;
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
    InlineAsmStatement,
    Declaration
    // Definition,
  >;

public:
  template<typename T>
  Instruction(T &&instr): instr(std::forward<T>(instr)) {}

  inline void debug(size_t depth) const {
    std::visit([depth](const auto &node) { node.debug(depth); }, instr);
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    std::visit([&scopeStack, &scope](auto &node) { node.decorate(scopeStack, scope); }, instr);
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

  inline void debug(size_t depth) const {
    logNode(depth, "InstructionCount: ", instructions.size());
    for (const auto &instr : instructions) {
      instr.debug(depth + 1);
    }
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    for (auto &instr : instructions) {
      instr.decorate(scopeStack, scope);
    }
  }

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &evaluator) const {
    for (const auto &instr : instructions) {
      instr.genAsm_x86_64(evaluator);
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

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    type.decorate(scopeStack, scope);
  }

  inline const scopes::TypeDescription *getTypeDescription() const {
    return type.getTypeDescription();
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

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    type.decorate(scopeStack, scope);
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

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    for (auto &param : parameters) {
      param.decorate(scopeStack, scope);
    }
  }

  inline auto size() const { return parameters.size(); }

  inline std::vector<FunctionParameter>::iterator begin() { return parameters.begin(); }
  inline std::vector<FunctionParameter>::iterator end() { return parameters.end(); }

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

  inline void debug(size_t depth) const {
    logNode(depth, "ReturnType: ", returnType.fullName(), " ; Name: ", name, " ; ParamCount: ", params.size());
    returnType.debug(depth + 1);
    params.debug(depth + 1);
    body.debug(depth + 1);
    if (description)
      logDecoration(depth + 1, "FunctionDescription: ", description->name, " ; Id: ", description->functionId);
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    scopes::Scope &newScope = scopeStack.createChildScope(&scope);
    returnType.decorate(scopeStack, newScope);
    params.decorate(scopeStack, newScope);
    body.decorate(scopeStack, newScope);

    std::vector<const scopes::TypeDescription*> paramTypes;
    for (auto &param : params) {
      paramTypes.push_back(param.getTypeDescription());
    }
    scopeStack.addFunction(name, paramTypes, returnType.getTypeDescription(), scope);
    description = scope.findFunction(name);
  }

  void genAsm_x86_64(codegen::NasmGenerator_x86_64 &generator) const {
    generator.emitGlobalDirective(name);
    generator.emitFunctionLabel(name);

    generator.emitSaveBasePointer();
    generator.emitSetBasePointerToCurrentStackPointer();
    body.genAsm_x86_64(generator);
    generator.emitRestoreBasePointer();

    generator.emitReturnInstruction();
  };

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
  Method(Type &&returnType, std::string_view name, FunctionParameterList &&params, InstructionList &&body)
  : returnType(returnType), name(name), params(params), body(body)
  {}

  inline void debug(size_t depth) const {
    logNode(depth, "ReturnType: ", returnType.fullName(), " ; Name: ", name, " ; ParamCount: ", params.size());
    returnType.debug(depth + 1);
    params.debug(depth + 1);
    body.debug(depth + 1);
  }

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    returnType.decorate(scopeStack, scope);
    params.decorate(scopeStack, scope);
    body.decorate(scopeStack, scope);
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

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    (void) scopeStack;
    (void) scope;
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

  inline void decorate(scopes::ScopeStack &scopeStack, scopes::Scope &scope) {
    for (auto &attr : attributes) {
      attr.first.decorate(scopeStack, scope);
    }
    for (auto &method : methods) {
      method.first.decorate(scopeStack, scope);
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
    for (auto &func : functions) {
      func.decorate(scopeStack, scope);
    }
    for (auto &classNode : classes) {
      classNode.decorate(scopeStack, scope);
    }
  }

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
