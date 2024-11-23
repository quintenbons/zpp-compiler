#pragma once

#include <cstddef>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>
#include <type_traits>
#include "ast/DebugEvaluator.hpp"

namespace ast {

enum class LevelSpecifier { Public, Protected, Private };

template <typename T>
concept HasDebugEvaluate = requires(const T &obj, DebugEvaluator &evaluator) {
  { obj.debugEvaluate(evaluator) } -> std::same_as<void>;
};

template <typename NodeType> class AstNode {
public:
  inline void debugEvaluate(DebugEvaluator &evaluator) const
    requires HasDebugEvaluate<NodeType>
  {
    static_cast<const NodeType *>(this)->debugEvaluate(evaluator);
  }

  inline static const char *getNodeName() { return NodeType::node_name; }

  friend std::ostream &operator<<(std::ostream &os, const AstNode &) {
    return os << getNodeName();
  }
};

struct Type : public AstNode<Type> {
  std::string_view name;
  int pointerDepth;
  static constexpr const char *node_name = "Node_Type";

  Type(std::string_view &&name, int &&pointerDepth)
      : name{name}, pointerDepth{pointerDepth} {}

  inline std::string fullName() const {
    return std::string(name) + std::string(pointerDepth, '*');
  }

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, fullName());
  }
};

struct NumberLiteral : public AstNode<NumberLiteral> {
  using UnderlyingT = uint64_t;

  UnderlyingT number;
  static constexpr const char *node_name = "Node_NumberLiteral";

  NumberLiteral(UnderlyingT &&number) : number{number} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, number);
  }
};

struct Expression : public AstNode<Expression> {
  std::variant<NumberLiteral> instruction;
  static constexpr const char *node_name = "Node_Expression";

  Expression(NumberLiteral &&numberLiteral) : instruction{numberLiteral} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    std::visit(
        [&evaluator](const auto &node) { node.debugEvaluate(evaluator); },
        instruction);
  }
};

struct ReturnStatement : public AstNode<ReturnStatement> {
  std::unique_ptr<Expression> expression;
  static constexpr const char *node_name = "Node_ReturnStatement";

  ReturnStatement(std::unique_ptr<Expression> expression)
      : expression{std::move(expression)} {}

  ReturnStatement(const ReturnStatement &rs)
      : expression{std::make_unique<Expression>(*rs.expression)} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this);
    evaluator.increaseDepth();
    expression->debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};

struct Instruction : public AstNode<Instruction> {
  using InstructionVariant = std::variant<ReturnStatement
                                          // Declaration,
                                          // Definition,
                                          >;

  InstructionVariant instruction;
  static constexpr const char *node_name = "Node_Instruction";

  template <typename T>
    requires std::constructible_from<InstructionVariant, T &&>
  Instruction(T &&instruction) : instruction{std::forward<T>(instruction)} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    std::visit(
        [&evaluator](const auto &node) { node.debugEvaluate(evaluator); },
        instruction);
  }
};

struct InstructionList : public AstNode<InstructionList> {
  std::vector<Instruction> instructions;
  static constexpr const char *node_name = "Node_InstructionList";

  InstructionList(std::vector<Instruction> &&instructions)
      : instructions(instructions) {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "InstructionCount: ", instructions.size());
    evaluator.increaseDepth();
    for (const auto &instr : instructions)
      instr.debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};

using CodeBlock = InstructionList;

struct FunctionParameter : public AstNode<FunctionParameter> {
  Type type;
  std::string_view name;
  static constexpr const char *node_name = "Node_FunctionParameter";

  FunctionParameter(std::string_view &&name, Type &&type)
      : type{type}, name{name} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "Type: ", type.fullName(), " ; Name: ", name);
  }
};

struct Attribute : public AstNode<Attribute> {
  Type type;
  std::string_view name;
  static constexpr const char *node_name = "Node_Class_Attribute";

  Attribute(std::string_view &&name, Type &&type) : type{type}, name{name} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "Type: ", type.fullName(), " ; Name: ", name);
  }
};

struct FunctionParameterList : public AstNode<FunctionParameterList> {
  std::vector<FunctionParameter> parameters;
  static constexpr const char *node_name = "Node_FunctionParameterList";

  FunctionParameterList(std::vector<FunctionParameter> &&parameters)
      : parameters{parameters} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "ParameterCount: ", parameters.size());
    evaluator.increaseDepth();
    for (const auto &param : parameters)
      param.debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};

struct Function : public AstNode<Function> {
  Type returnType;
  std::string_view name;
  FunctionParameterList parametersNode;
  CodeBlock body;
  static constexpr const char *node_name = "Node_Function";

  Function(CodeBlock &&body, FunctionParameterList &&parametersNode,
           std::string_view &&name, Type &&returnType)
      : returnType{returnType}, name{name}, parametersNode{parametersNode},
        body{body} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "ReturnType: ", returnType.fullName(),
                      " ; Name: ", name,
                      " ; ParamCount: ", parametersNode.parameters.size());
    evaluator.increaseDepth();
    returnType.debugEvaluate(evaluator);
    parametersNode.debugEvaluate(evaluator);
    body.debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};

struct Method : public AstNode<Method> {
  Type returnType;
  std::string_view name;
  FunctionParameterList parametersNode;
  CodeBlock body;
  static constexpr const char *node_name = "Node_Class_Method";

  Method(CodeBlock &&body, FunctionParameterList &&parametersNode,
         std::string_view &&name, Type &&returnType)
      : returnType{returnType}, name{name}, parametersNode{parametersNode},
        body{body} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "ReturnType: ", returnType.fullName(),
                      " ; Name: ", name,
                      " ; ParamCount: ", parametersNode.parameters.size());
    evaluator.increaseDepth();
    returnType.debugEvaluate(evaluator);
    parametersNode.debugEvaluate(evaluator);
    body.debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};

struct AccessSpecifier : public AstNode<AccessSpecifier> {
  LevelSpecifier level;
  static constexpr const char *node_name = "Node_AccessSpecifier";

  AccessSpecifier(LevelSpecifier &&level) : level{level} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    switch (level) {
    case LevelSpecifier::Private:
      evaluator.logNode(*this, "LevelSpecifier: Private");
      break;
    case LevelSpecifier::Protected:
      evaluator.logNode(*this, "LevelSpecifier: Protected");
      break;
    case LevelSpecifier::Public:
      evaluator.logNode(*this, "LevelSpecifier: Public");
      break;
    }
  }
};

struct Class : public AstNode<Class> {
  std::string_view name;
  std::vector<std::pair<Attribute, AccessSpecifier>> attributes;
  std::vector<std::pair<Method, AccessSpecifier>> methods;
  static constexpr const char *node_name = "Node_Class";

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "Name: ", name,
                      " ; Members: ", attributes.size() + methods.size());
    evaluator.increaseDepth();
    for (const auto &[attribute, accessSpecifier] : attributes) {
      attribute.debugEvaluate(evaluator);
    }
    for (const auto &[method, accessSpecifier] : methods) {
      method.debugEvaluate(evaluator);
    }
    evaluator.decreaseDepth();
  }
};

struct TranslationUnit : public AstNode<TranslationUnit> {
  std::vector<Function> functions;
  std::vector<Class> classes;
  static constexpr const char *node_name = "Node_TranslationUnit";

  TranslationUnit(std::vector<Function> &&functions,
                  std::vector<Class> &&classes)
      : functions{std::move(functions)}, classes{std::move(classes)} {}

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "Function count: ", functions.size());
    evaluator.increaseDepth();
    for (const auto &funcNode : functions)
      funcNode.debugEvaluate(evaluator);
    evaluator.decreaseDepth();
    evaluator.logNode(*this, "Class count: ", classes.size());
    evaluator.increaseDepth();
    for (const auto &classNode : classes)
      classNode.debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};
} /* namespace ast */
