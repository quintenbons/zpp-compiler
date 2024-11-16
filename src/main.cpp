#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "core/errors.hpp"
#include "lexing_parsing/parser.ipp"
#include "ast/DebugEvaluator.hpp"
#include "ast/MockEvaluator.hpp"

struct translationUnitHandle
{
  parser::Parser parser;
  ast::TranslationUnit translationUnit;
};

translationUnitHandle parseTranslationUnit(const char *filename)
{
  std::ifstream inputFile(filename);
  DEBUG_ASSERT(inputFile.is_open(), "Could not open file");
  parser::Parser parser(std::move(inputFile));
  ast::TranslationUnit translationUnit = parser.parseProgram();
  return { std::move(parser), std::move(translationUnit) };
}

int main(int argc, char** argv)
{
  std::vector<const char*> args(argv, argv+argc);

  if (args.size() != 2) {
    std::cerr << "Usage: " << args[0] << " [inputfile]" << std::endl;
    return 0;
  }

  auto tuHandle = parseTranslationUnit(args[1]);
  // ast::evaluate<ast::MockEvaluator>([]() { return ast::MockEvaluator(); }, translationUnit);
  ast::DebugEvaluator()(tuHandle.translationUnit);
}
