#pragma once

#include <boost/filesystem.hpp>

#include "ast/nodes.hpp"
#include "codegen/generate.hpp"
#include "dbg/errors.hpp"
#include "dbg/iohelper.hpp"
#include "lexing_parsing/parser.ipp"

namespace core
{

class TranslationUnitHandle
{
public:
  TranslationUnitHandle(std::ifstream &&inputFile)
  {
    _parser = std::make_unique<parser::Parser>(std::move(inputFile));
    parseIfNeeded();
  }

  TranslationUnitHandle(const boost::filesystem::path &filename)
  : TranslationUnitHandle(utils::fs::safeIfStream(filename))
  {
  }

  ast::TranslationUnit &getOrCreateTranslationUnit()
  {
    parseIfNeeded();
    DEBUG_ASSERT(_translationUnit, "Translation unit creation failed");
    return *_translationUnit.get();
  }

  void debug()
  {
    getOrCreateTranslationUnit().debug(0);
  }

  void debugScopeStack()
  {
    _scopeStack->logDebug();
  }

  void decorate()
  {
    if (_scopeStack) return;

    parseIfNeeded();
    _scopeStack = std::make_unique<scopes::ScopeStack>();
    getOrCreateTranslationUnit().decorate(*_scopeStack, _scopeStack->rootScope());
  }

  std::string genAsm_x86_64()
  {
    const auto &translationUnit = getOrCreateTranslationUnit();
    DEBUG_ASSERT(translationUnit.isDecorated(), "Translation unit is not decorated!");
    codegen::NasmGenerator_x86_64 codeGenerator;
    return translationUnit.genAsm_x86_64(codeGenerator);
  }

private:
  inline void parseIfNeeded()
  {
    if (!_translationUnit)
    {
      _translationUnit = std::make_unique<ast::TranslationUnit>(_parser->parseTranslationUnit());
    }
  }

private:
  std::unique_ptr<parser::Parser> _parser;
  std::unique_ptr<ast::TranslationUnit> _translationUnit;
  std::unique_ptr<scopes::ScopeStack> _scopeStack;
};

}
