#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "core/TranslationUnitHandle.hpp"
#include "dbg/errors.hpp"
#include "lexing_parsing/parser.ipp"
#include "ast/DebugEvaluator.hpp"

int main(int argc, char** argv)
{
  std::vector<const char*> args(argv, argv+argc);

  if (args.size() != 2) {
    std::cerr << "Usage: " << args[0] << " [inputfile]" << std::endl;
    return 0;
  }

  auto translationUnitHandle = core::TranslationUnitHandle(args[1]);
  ast::DebugEvaluator debugEvaluator;
  translationUnitHandle.getOrCreateTranslationUnit()->debugEvaluate(debugEvaluator);
}
