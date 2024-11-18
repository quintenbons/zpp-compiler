#pragma once

#include <ast/nodes.hpp>
#include <lexing_parsing/parser.ipp>

namespace core
{

class TranslationUnitHandle
{
public:
  TranslationUnitHandle(const char *filename)
  : _filename{filename}
  {
    std::ifstream inputFile(filename);
    DEBUG_ASSERT(inputFile.is_open(), "Could not open file");
    _parser = std::make_unique<parser::Parser>(std::move(inputFile));
  }

  const ast::TranslationUnit *getOrCreateTranslationUnit()
  {
    if (!_translationUnit)
    {
      _translationUnit = std::make_unique<ast::TranslationUnit>(std::move(_parser->parseTranslationUnit()));
    }

    return _translationUnit.get();
  }

private:
  const char *_filename;
  std::unique_ptr<parser::Parser> _parser;
  std::unique_ptr<ast::TranslationUnit> _translationUnit;
};

}
