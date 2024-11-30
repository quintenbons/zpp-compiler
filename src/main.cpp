#include <cstdlib>
#include <iostream>
#include <cassert>

#include "codegen/assemble.hpp"
#include "codegen/linking.hpp"
#include "core/TranslationUnitHandle.hpp"
#include "dbg/argparse.hpp"
#include "dbg/errors.hpp"
#include "dbg/iohelper.hpp"

static inline int fullDebugExec(argparse::CompilerOptions &options) {
  utils::DryRunSafeIFStream inputFile(options.inputFiles.at(0));
  utils::SafeOFStream asmFile("./a.asm");
  utils::SafeOFStream objFile("./a.o");
  utils::SafeOFStream outputFile("./a.out");

  LOG("== Parsing");
  auto translationUnitHandle = core::TranslationUnitHandle(inputFile.path());
  LOG("");
  LOG("== Done parsing");
  translationUnitHandle.debug();
  LOG("");
  LOG("== Decorating");
  translationUnitHandle.decorate();
  translationUnitHandle.debugScopeStack();
  LOG("");
  LOG("== Done decorating");
  translationUnitHandle.debug();
  LOG("");
  LOG("== Generating code");
  std::string generatedAsm = translationUnitHandle.genAsm_x86_64();
  LOG("== Generated asm to a.asm:");
  std::cout << generatedAsm;
  asmFile.writeAndClose(generatedAsm);

  LOG("== Generated .o as a.o:");
  assemble::runNasmSafe(asmFile.path(), objFile.path());

  LOG("== Generating exe as a.out:");
  linking::runLdSafe(objFile.path(), outputFile.path());

  return 0;
}

int main(int argc, char** argv)
{
  auto options = argparse::ArgParser(argc, argv).parse();

  if (options.fullDebugExec) {
    fullDebugExec(options);
  }

  else if (options.preprocessOnly) {
    TODO("Preprocessing not yet implemented");
  }

  // redundancy here is acceptable to make sure io errors happen sooner rather than later
  else if (options.compileOnly) {
    utils::DryRunSafeIFStream inputFile(options.inputFiles.at(0));
    utils::SafeOFStream outputFile(options.outputFile);

    auto tu = core::TranslationUnitHandle(inputFile.path());
    tu.decorate();
    outputFile << tu.genAsm_x86_64();
  }

  else if (options.compileAndAssemble) {
    utils::DryRunSafeIFStream inputFile(options.inputFiles.at(0));
    utils::TempOFStream asmFile("asm");
    utils::SafeOFStream outputFile(options.outputFile);

    auto tu = core::TranslationUnitHandle(inputFile.path());
    tu.decorate();
    asmFile.writeAndClose(tu.genAsm_x86_64());

    assemble::runNasmSafe(asmFile.path(), outputFile.path());
  }

  else {
    utils::DryRunSafeIFStream inputFile(options.inputFiles.at(0));
    utils::TempOFStream asmFile("asm");
    utils::TempOFStream objectFile("o");
    utils::DryRunSafeIFStream outputFile(options.outputFile);

    auto tu = core::TranslationUnitHandle(options.inputFiles.at(0));
    tu.decorate();
    asmFile.writeAndClose(tu.genAsm_x86_64());

    assemble::runNasmSafe(asmFile.path(), objectFile.path());
    linking::runLdSafe(objectFile.path(), outputFile.path());
  }
}
