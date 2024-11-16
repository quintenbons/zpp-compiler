#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "core/errors.hpp"
#include "lexing_parsing/parser.ipp"
#include "ast/DebugEvaluator.hpp"

ast::TranslationUnit parseTranslationUnit(const char *filename)
{
  std::ifstream inputFile(filename);
  DEBUG_ASSERT(inputFile.is_open(), "Could not open file");
  auto parser = parser::Parser(std::move(inputFile));
  return parser.parseProgram();
}

int main(int argc, char** argv)
{
  std::vector<const char*> args(argv, argv+argc);

  if (args.size() != 2) {
    std::cerr << "Usage: " << args[0] << " [inputfile]" << std::endl;
    return 0;
  }

  ast::TranslationUnit translationUnit = parseTranslationUnit(args[1]);
  ast::evaluate<ast::DebugEvaluator>([]() { return ast::DebugEvaluator(); }, translationUnit);
}
