#pragma once

#include "ast/nodes.hpp"
#include "lexing_parsing/parser.ipp"

namespace core
{

class TranslationUnitHandle
{
public:
  TranslationUnitHandle(const char *filename)
  {
    std::ifstream inputFile(filename);
    DEBUG_ASSERT(inputFile.is_open(), "Could not open file");
    _parser = std::make_unique<parser::Parser>(std::move(inputFile));
  }

  ast::TranslationUnit *getOrCreateTranslationUnit()
  {
    parseIfNeeded();
    return _translationUnit.get();
  }

  void debug()
  {
    getOrCreateTranslationUnit()->debug(0);
  }

  void decorate()
  {
    if (_scopeStack) return;

    parseIfNeeded();
    _scopeStack = std::make_unique<scopes::ScopeStack>();
    getOrCreateTranslationUnit()->decorate(*_scopeStack, _scopeStack->rootScope());
    _scopeStack->logDebug();
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
