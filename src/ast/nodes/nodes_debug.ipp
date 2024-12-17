#include <cstddef>
#include <ostream>
#include <ranges>
#include <variant>
#include <vector>

#include "ast/scopes/registers.hpp"
#include "ast/scopes/types.hpp"
#include "dbg/utils.hpp"

#include "nodes.h"

namespace ast {

inline void Type::debug(size_t depth) const {
  logNode(depth, fullName());
  if (description)
    logDecoration(depth + 1, "TypeDescription: ", description->name,
                  " ; Size: ", description->byteSize,
                  " ; Id: ", description->id);
}

inline void Variable::debug(size_t depth) const {
  logNode(depth, name);
  if (description)
    logDecoration(depth + 1,
                  "VariableDescription: Id: ", description->variableId);
}

inline void NumberLiteral::debug(size_t depth) const { logNode(depth, number); }

inline void StringLiteral::debug(size_t depth) const { logNode(depth, content); }

inline void Expression::debug(size_t depth) const {
  std::visit([depth](const auto &node) { node.debug(depth); }, expr);
}

inline void FunctionCall::debug(size_t depth) const {
  logNode(depth, name, " ; ArgumentCount: ", arguments.size());
  for (const auto &arg : arguments) {
    arg.debug(depth + 1);
  }
}

inline void BinaryOperation::debug(size_t depth) const {
  logNode(depth, static_cast<char>(op));
  lhs->debug(depth+1);
  rhs->debug(depth+1);
}

inline void Declaration::debug(size_t depth) const {
  logNode(depth, "Type: ", type.fullName(),
          " ; Assignment: ", assignment.has_value());
  variable.debug(depth + 1);
  if (assignment.has_value()) {
    assignment->debug(depth + 1);
  }
}

inline void ReturnStatement::debug(size_t depth) const {
  logNode(depth);
  expression.debug(depth);
}

inline void InlineAsmStatement::debug(size_t depth) const {
  logNode(depth, "Register binding request count: ", requests.size());
  for (auto &request : requests) {
    LOG_DEBUG(INDENT_D(depth) << "[Request] " << request.registerTo << "("
                              << request.varIdentifier << ")");
  }
  asmBlock.debug(depth + 1);
}

inline void Instruction::debug(size_t depth) const {
  std::visit([depth](const auto &node) { node.debug(depth); }, instr);
}

inline void InstructionList::debug(size_t depth) const {
  logNode(depth, "InstructionCount: ", instructions.size());
  for (const auto &instr : instructions) {
    instr.debug(depth + 1);
  }
}

inline void FunctionParameter::debug(size_t depth) const {
  logNode(depth, "Type: ", type.fullName());
  variable.debug(depth + 1);
}

inline void Attribute::debug(size_t depth) const {
  logNode(depth, "Type: ", type.fullName());
  variable.debug(depth + 1);
}

inline void FunctionParameterList::debug(size_t depth) const {
  logNode(depth, "ParameterCount: ", parameters.size());
  for (const auto &param : parameters) {
    param.debug(depth + 1);
  }
}

inline void Function::debug(size_t depth) const {
  logNode(depth, "ReturnType: ", returnType.fullName(), " ; Name: ", name,
          " ; ParamCount: ", params.size());
  returnType.debug(depth + 1);
  params.debug(depth + 1);
  body.debug(depth + 1);
  if (description)
    logDecoration(depth + 1, "FunctionDescription: ", description->name,
                  " ; Id: ", description->functionId);
}

inline void Method::debug(size_t depth) const {
  logNode(depth, "ReturnType: ", returnType.fullName(), " ; Name: ", name,
          " ; ParamCount: ", params.size());
  returnType.debug(depth + 1);
  params.debug(depth + 1);
  body.debug(depth + 1);
}

inline void AccessSpecifier::debug(size_t depth) const {
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

inline void Class::debug(size_t depth) const {
  logNode(depth, "Name: ", name,
          " ; Members: ", attributes.size() + methods.size());
  for (const auto &visibility : allVisibilities) {
    AccessSpecifier(visibility).debug(depth);
    auto filteredAttrs =
        attributes | std::views::filter([visibility](const auto &pair) {
          return pair.second == visibility;
        });
    auto filteredMethods =
        methods | std::views::filter([visibility](const auto &pair) {
          return pair.second == visibility;
        });
    for (const auto &[attribute, accessSpecifier] : filteredAttrs) {
      attribute.debug(depth + 1);
    }
    for (const auto &[method, accessSpecifier] : filteredMethods) {
      method.debug(depth + 1);
    }
  }
}

inline void TranslationUnit::debug(size_t depth) const {
  logNode(depth, "Function count: ", functions.size());
  for (const auto &funcNode : functions) {
    funcNode.debug(depth + 1);
  }

  logNode(depth, "Class count: ", classes.size());
  for (const auto &classNode : classes) {
    classNode.debug(depth + 1);
  }
}
} /* namespace ast */
