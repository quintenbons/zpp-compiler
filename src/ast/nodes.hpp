#pragma once

#include <cstddef>
#include <format>
#include <memory>
#include <ostream>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>

#include "ast/DebugEvaluator.hpp"
#include "ast/DecoratorEvaluator.ipp"
#include "ast/scopes/registers.hpp"
#include "dbg/errors.hpp"
#include "scopes/scopeStack.hpp"
#include "scopes/types.hpp"
#include <type_traits>

namespace ast {

static constexpr char ENDL = '\n';
static constexpr char INDENT = '\t';

enum class LevelSpecifier { Public, Protected, Private };

template <typename T>
concept HasDebugEvaluate = requires(const T &obj, DebugEvaluator &_evaluator) {
  { obj.debug(_evaluator) } -> ::std::same_as<void>;
};

template <typename NodeType> struct AstNode {
  inline void debug(DebugEvaluator &_evaluator) const
    requires HasDebugEvaluate<NodeType>
  {
    static_cast<const NodeType *>(this)->debugEvaluate(_evaluator);
  }

  inline void decorate(DecoratorEvaluator &_evaluator) {
    static_cast<NodeType *>(this)->decorate(_evaluator);
  }

  inline static const char *getNodeName() { return NodeType::node_name; }

  friend ::std::ostream &operator<<(::std::ostream &os, const AstNode &) {
    return os << getNodeName();
  }
};

struct Type : public AstNode<Type> {
  static constexpr const char *node_name = "Node_Type";
  ::std::string_view name;
  int pointerDepth;
  const scopes::TypeDescription *description{};

  inline void decorate(DecoratorEvaluator &_evaluator) {
    description = &(_evaluator.getCurrentScope()).findType(name);
  }

  inline ::std::string fullName() const {
    return ::std::string(name) + ::std::string(pointerDepth, '*');
  }

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, fullName());
  }
};

struct NumberLiteral : public AstNode<NumberLiteral> {
  static constexpr const char *node_name = "Node_NumberLiteral";
  using UnderlyingT = uint64_t;

  UnderlyingT number;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, number);
  }

  void loadValueInRegister(::std::ostream &ss,
                           scopes::Register targetRegister) {
    ss << "  mov " << scopes::regToStr(targetRegister) << ", " << number
       << ENDL;
  }
};

struct StringLiteral : public AstNode<StringLiteral> {
  static constexpr const char *node_name = "Node_StringLiteral";
  ::std::string content;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, content);
    _evaluator.increaseDepth();
    LOG_DEBUG(::std::string(_evaluator.getDepth() * 2, ' ') << content);
    _evaluator.decreaseDepth();
  };
};

struct Expression : public AstNode<Expression> {
  static constexpr const char *node_name = "Node_Expression";
  ::std::variant<NumberLiteral> instruction;

  inline void debug(DebugEvaluator &_evaluator) const {
    ::std::visit([&_evaluator](const auto &node) { node.debug(_evaluator); },
                 instruction);
  }
};

struct ReturnStatement : public AstNode<ReturnStatement> {
  static constexpr const char *node_name = "Node_ReturnStatement";
  ::std::unique_ptr<Expression> expression;

  void genCode(::std::ostream &ss) const {
    ::std::visit(
        [&ss](auto &&expr) {
          expr.loadValueInRegister(ss, scopes::returnRegister);
        },
        (*expression).instruction);
    ss << "  ret" << ENDL;
  }

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this);
    _evaluator.increaseDepth();
    expression->debug(_evaluator);
    _evaluator.decreaseDepth();
  }
};

struct InlineAsmStatement : public AstNode<InlineAsmStatement> {
  static constexpr const char *node_name = "Node_InlineAsmStatement";
  using Register = scopes::Register;

  struct BindingRequest {
    scopes::Register registerTo;
    ::std::string varIdentifier;
  };

  StringLiteral asmBlock;
  ::std::vector<BindingRequest> requests;

  void genCode(::std::ostream &ss) const {
    ss << ";-- START -- asm binding requests" << ENDL;
    // TODO: not assume that every variable is always in rdx

    for (auto &request : requests) {
      LOG("generating code for request " << request.registerTo);
      ss << INDENT
         << ::std::format("mov {}, {}", regToStr(request.registerTo),
                          regToStr(Register::REG_RAX))
         << ENDL;
    }

    ss << ";-- START -- user defined" << ENDL;
    ss << asmBlock.content << ENDL;
    ss << ";-- END -- user defined" << ENDL;
  }

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this,
                       "Register binding request count: ", requests.size());
    _evaluator.increaseDepth();
    for (auto &request : requests) {
      LOG_DEBUG(::std::string(_evaluator.getDepth() * 2, ' ') << "[Request] " <<
      request.registerTo << "(" << request.varIdentifier << ")");
      LOG("Register binding request: " << request.registerTo << " ("
                                       << request.varIdentifier << ")");
    }
    asmBlock.debug(_evaluator);
    _evaluator.decreaseDepth();
  }
};

struct Instruction : public AstNode<Instruction> {
  static constexpr const char *node_name = "Node_Instruction";
  using InstructionVariant = ::std::variant<ReturnStatement,
                                            InlineAsmStatement
                                            // Declaration,
                                            // Definition,
                                            >;

  InstructionVariant instruction;

  inline void debug(DebugEvaluator &_evaluator) const {
    ::std::visit([&_evaluator](const auto &node) { node.debug(_evaluator); },
                 instruction);
  }
};

struct InstructionList : public AstNode<InstructionList> {
  static constexpr const char *node_name = "Node_InstructionList";
  ::std::vector<Instruction> instructions;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "InstructionCount: ", instructions.size());
    _evaluator.increaseDepth();
    for (const auto &instr : instructions)
      instr.debug(_evaluator);
    _evaluator.decreaseDepth();
  }
};

using CodeBlock = InstructionList;

struct FunctionParameter : public AstNode<FunctionParameter> {
  static constexpr const char *node_name = "Node_FunctionParameter";
  Type type;
  ::std::string_view name;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "Type: ", type.fullName(), " ; Name: ", name);
  }
};

struct Attribute : public AstNode<Attribute> {
  static constexpr const char *node_name = "Node_ClassAttribute";
  Type type;
  ::std::string_view name;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "Type: ", type.fullName(), " ; Name: ", name);
  }
};

struct FunctionParameterList : public AstNode<FunctionParameterList> {
  static constexpr const char *node_name = "Node_FunctionParameterList";
  ::std::vector<FunctionParameter> parameters;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "ParameterCount: ", parameters.size());
    _evaluator.increaseDepth();
    for (const auto &param : parameters)
      param.debug(_evaluator);
    _evaluator.decreaseDepth();
  }
};

struct Function : public AstNode<Function> {
  static constexpr const char *node_name = "Node_Function";
  Type returnType;
  ::std::string_view name;
  FunctionParameterList parametersNode;
  CodeBlock body;

  void genCode(::std::ostream &ss) const {
    ::std::stringstream asmCode;
    ss << ENDL;
    ss << name << ":" << ENDL;
    for (auto &instructionNode : body.instructions) {
      ::std::visit([&](auto &&instr) { instr.genCode(ss); }, instructionNode.instruction);
    }
    ss << INDENT << "ret" << ENDL;
  };

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "ReturnType: ", returnType.fullName(),
                       " ; Name: ", name,
                       " ; ParamCount: ", parametersNode.parameters.size());
    _evaluator.increaseDepth();
    returnType.debug(_evaluator);
    parametersNode.debug(_evaluator);
    body.debug(_evaluator);
    _evaluator.decreaseDepth();
  }
};

struct Method : public AstNode<Method> {
  static constexpr const char *node_name = "Node_ClassMethod";
  Type returnType;
  ::std::string_view name;
  FunctionParameterList parametersNode;
  CodeBlock body;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "ReturnType: ", returnType.fullName(),
                       " ; Name: ", name,
                       " ; ParamCount: ", parametersNode.parameters.size());
    _evaluator.increaseDepth();
    returnType.debug(_evaluator);
    parametersNode.debug(_evaluator);
    body.debug(_evaluator);
    _evaluator.decreaseDepth();
  }
};

struct AccessSpecifier : public AstNode<AccessSpecifier> {
  static constexpr const char *node_name = "Node_AccessSpecifier";
  LevelSpecifier level;

  inline void debug(DebugEvaluator &_evaluator) const {
    switch (level) {
    case LevelSpecifier::Private:
      _evaluator.logNode(*this, "LevelSpecifier: Private");
      break;
    case LevelSpecifier::Protected:
      _evaluator.logNode(*this, "LevelSpecifier: Protected");
      break;
    case LevelSpecifier::Public:
      _evaluator.logNode(*this, "LevelSpecifier: Public");
      break;
    }
  }
};

struct Class : public AstNode<Class> {
  static constexpr const char *node_name = "Node_Class";
  ::std::string_view name;
  ::std::vector<::std::pair<Attribute, AccessSpecifier>> attributes;
  ::std::vector<::std::pair<Method, AccessSpecifier>> methods;

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "Name: ", name,
                       " ; Members: ", attributes.size() + methods.size());
    _evaluator.increaseDepth();
    for (const auto &[attribute, accessSpecifier] : attributes) {
      attribute.debug(_evaluator);
    }
    for (const auto &[method, accessSpecifier] : methods) {
      method.debug(_evaluator);
    }
    _evaluator.decreaseDepth();
  }
};

struct TranslationUnit : public AstNode<TranslationUnit> {
  static constexpr const char *node_name = "Node_TranslationUnit";
  ::std::vector<Function> functions;
  ::std::vector<Class> classes;

  inline bool isDecorated() const { return true; }

  inline ::std::string genAsm_x86_64() const {
    ::std::stringstream asmCode;

    asmCode << "section .data" << ENDL;

    asmCode << ENDL;
    asmCode << "section .rodata" << ENDL;

    asmCode << ENDL;
    asmCode << "section .bss" << ENDL;

    asmCode << ENDL;
    asmCode << "section .text" << ENDL;
    asmCode << INDENT << "global _start" << ENDL;
    // asmCode << INDENT << "global print:function" << ENDL; // when generating stdlibc

    asmCode << ENDL;
    asmCode << "_start:" << ENDL;
    asmCode << INDENT << "call main" << ENDL;
    asmCode << INDENT
            << "mov rax, 60                  ; Syscall number for exit (60)"
            << ENDL;
    asmCode << INDENT
            << "mov rdi, rbx                 ; Exit code (0) expects return of "
               "main to be put in rbx for now"
            << ENDL;
    asmCode << INDENT << "syscall                      ; Make the syscall"
            << ENDL;

    for (auto &func : functions) {
      func.genCode(asmCode);
    }

    for (auto &classNode : classes) {
      (void)classNode;
      TODO("Implement classNodes");
    }

    return asmCode.str();
  }

  inline void debug(DebugEvaluator &_evaluator) const {
    _evaluator.logNode(*this, "Function count: ", functions.size());
    _evaluator.increaseDepth();
    for (const auto &funcNode : functions)
      funcNode.debug(_evaluator);
    _evaluator.decreaseDepth();
    _evaluator.logNode(*this, "Class count: ", classes.size());
    _evaluator.increaseDepth();
    for (const auto &classNode : classes)
      classNode.debug(_evaluator);
    _evaluator.decreaseDepth();
  }

  inline void decorate(DecoratorEvaluator &)
  {

  }
};

#define PURE_NODE_LIST                                                         \
  X(Type, "Node_Type")                                                         \
  X(NumberLiteral, "Node_NumberLiteral")                                       \
  X(StringLiteral, "Node_StringLiteral")                                       \
  X(ReturnStatement, "Node_ReturnStatement")                                   \
  X(InlineAsmStatement, "Node_InlineAsmStatement")                             \
  X(InstructionList, "Node_InstructionList")                                   \
  X(FunctionParameter, "Node_FunctionParameter")                               \
  X(FunctionParameterList, "Node_FunctionParameter")                           \
  X(Function, "Node_Function")                                                 \
  X(Method, "Node_ClassMethod")                                                \
  X(AccessSpecifier, "Node_AccessSpecifier")                                   \
  X(Attribute, "Node_ClassAttribute")                                          \
  X(Class, "Node_Class")                                                       \
  X(TranslationUnit, "Node_TranslationUnit")

#define VARIANT_NODE_LIST                                                      \
  X(Expression, "Node_Expression")                                             \
  X(Instruction, "Node_Instruction")

#define NODE_LIST                                                              \
  PURE_NODE_LIST                                                               \
  VARIANT_NODE_LIST

#define ALL_NODE_LIST                                                          \
  NODE_LIST                                                                    \
  X(CodeBlock, "Node_CodeBlock")

#define X(node, str)                                                           \
  inline const char *nodeToStr(const node &) { return str; }
  NODE_LIST
#undef X

} /* namespace ast */
