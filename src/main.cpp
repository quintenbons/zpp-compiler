#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "core/errors.hpp"
#include "lexing_parsing/parser.ipp"

int main(int argc, char** argv)
{
  std::vector<const char*> args(argv, argv+argc);

  if (args.size() != 2) {
    std::cerr << "Usage: " << args[0] << " [inputfile]" << std::endl;
    return 0;
  }

  {
    std::ifstream inputFile(args[1]);
    DEBUG_ASSERT(inputFile.is_open(), "Could not open file");
    auto parser = parser::Parser(std::move(inputFile));
    parser.parseProgram();
  }
}
