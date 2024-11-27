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

template <typename NodeType> struct AstNode {
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
  static constexpr const char *node_name = "Node_Type";
  std::string_view name;
  int pointerDepth;

  inline std::string fullName() const {
    return std::string(name) + std::string(pointerDepth, '*');
  }

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, fullName());
  }
};

struct NumberLiteral : public AstNode<NumberLiteral> {
  static constexpr const char *node_name = "Node_NumberLiteral";
  using UnderlyingT = uint64_t;

  UnderlyingT number;

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, number);
  }
};

struct Expression : public AstNode<Expression> {
  static constexpr const char *node_name = "Node_Expression";
  std::variant<NumberLiteral> instruction;

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    std::visit(
        [&evaluator](const auto &node) { node.debugEvaluate(evaluator); },
        instruction);
  }
};

struct ReturnStatement : public AstNode<ReturnStatement> {
  static constexpr const char *node_name = "Node_ReturnStatement";
  std::unique_ptr<Expression> expression;

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this);
    evaluator.increaseDepth();
    expression->debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};

struct Instruction : public AstNode<Instruction> {
  static constexpr const char *node_name = "Node_Instruction";
  using InstructionVariant = std::variant<ReturnStatement
                                          // Declaration,
                                          // Definition,
                                          >;

  InstructionVariant instruction;

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    std::visit(
        [&evaluator](const auto &node) { node.debugEvaluate(evaluator); },
        instruction);
  }
};

struct InstructionList : public AstNode<InstructionList> {
  static constexpr const char *node_name = "Node_InstructionList";
  std::vector<Instruction> instructions;

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
  static constexpr const char *node_name = "Node_FunctionParameter";
  Type type;
  std::string_view name;

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "Type: ", type.fullName(), " ; Name: ", name);
  }
};

struct Attribute : public AstNode<Attribute> {
  static constexpr const char *node_name = "Node_Class_Attribute";
  Type type;
  std::string_view name;

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "Type: ", type.fullName(), " ; Name: ", name);
  }
};

struct FunctionParameterList : public AstNode<FunctionParameterList> {
  static constexpr const char *node_name = "Node_FunctionParameterList";
  std::vector<FunctionParameter> parameters;

  inline void debugEvaluate(DebugEvaluator &evaluator) const {
    evaluator.logNode(*this, "ParameterCount: ", parameters.size());
    evaluator.increaseDepth();
    for (const auto &param : parameters)
      param.debugEvaluate(evaluator);
    evaluator.decreaseDepth();
  }
};

struct Function : public AstNode<Function> {
  static constexpr const char *node_name = "Node_Function";
  Type returnType;
  std::string_view name;
  FunctionParameterList parametersNode;
  CodeBlock body;

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
  static constexpr const char *node_name = "Node_Class_Method";
  Type returnType;
  std::string_view name;
  FunctionParameterList parametersNode;
  CodeBlock body;

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
  static constexpr const char *node_name = "Node_AccessSpecifier";
  LevelSpecifier level;

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
  static constexpr const char *node_name = "Node_Class";
  std::string_view name;
  std::vector<std::pair<Attribute, AccessSpecifier>> attributes;
  std::vector<std::pair<Method, AccessSpecifier>> methods;

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
  static constexpr const char *node_name = "Node_TranslationUnit";
  std::vector<Function> functions;
  std::vector<Class> classes;

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
