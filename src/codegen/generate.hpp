#pragma once

#include "ast/nodes.hpp"
#include "dbg/errors.hpp"

namespace codegen
{

inline std::string generateAsm(const ast::TranslationUnit &decoratedTranslationUnit)
{
  DEBUG_ASSERT(decoratedTranslationUnit.isDecorated(), "Node is not decorated!");
  return decoratedTranslationUnit.genAsm_x86_64();
}

}
