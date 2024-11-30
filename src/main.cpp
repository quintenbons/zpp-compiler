#include <iostream>
#include <vector>
#include <cassert>

#include "codegen/generate.hpp"
#include "core/TranslationUnitHandle.hpp"

int main(int argc, char** argv)
{
  std::vector<const char*> args(argv, argv+argc);

  if (args.size() != 2) {
    std::cerr << "Usage: " << args[0] << " [inputfile]" << std::endl;
    return 1;
  }

  auto translationUnitHandle = core::TranslationUnitHandle(args[1]);

  translationUnitHandle.debug();
  LOG("");
  LOG("== Decorating");
  translationUnitHandle.decorate();
  LOG("");
  LOG("== Done decorating");
  translationUnitHandle.debug();
  LOG("");
  LOG("== Generating code");
  std::string generatedAsm = codegen::generateAsm(*translationUnitHandle.getOrCreateTranslationUnit());
  LOG("== Generated asm to a.asm:");
  std::cout << generatedAsm;
  std::ofstream("./a.asm", std::ios_base::out) << generatedAsm;

  LOG("== Generated .o as a.o:");
  int nasmResult = std::system("nasm -f elf64 ./a.asm -o ./a.o");
  if (nasmResult != 0) {
    std::cerr << "Error: nasm command failed with exit code " << nasmResult
              << std::endl;
    return nasmResult;
  }

  LOG("== Generating exe as a.out:");
  int ldResult = std::system("ld ./a.o -o ./a.out");
  if (ldResult != 0) {
    std::cerr << "Error: ld command failed with exit code " << ldResult
              << std::endl;
    return ldResult;
  }
}
