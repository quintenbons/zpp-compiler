#include <cstdlib>
#include <iostream>
#include <cassert>

#include "codegen/assemble.hpp"
#include "codegen/linking.hpp"
#include "core/TranslationUnitHandle.hpp"
#include "dbg/argparse.hpp"
#include "dbg/errors.hpp"
#include "dbg/iohelper.hpp"
#include "ast/nodes/nodes.ipp"

static inline int fullDebugExec(argparse::CompilerOptions &options) {
  static constexpr const char *asmFilePath = "./a.asm";
  static constexpr const char *objFilePath = "./a.o";

  LOG(options.inputFiles[0]);
  LOG("== Parsing");
  auto translationUnitHandle = core::TranslationUnitHandle(options.inputFiles.at(0));
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
  utils::fs::safeOfStream(asmFilePath) << generatedAsm;

  LOG("== Generated .o as a.o:");
  assemble::runNasmSafe(asmFilePath, objFilePath);

  LOG("== Generating exe as a.out:");
  linking::runLdSafe(objFilePath, options.outputFile);

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

  auto tu = core::TranslationUnitHandle(options.inputFiles.at(0));
  tu.decorate();
  auto generatedAsm = tu.genAsm_x86_64();

  if (options.compileOnly) {
    utils::fs::safeOfStream(options.outputFile) << generatedAsm;
    return 0;
  }

  auto asmFilePath = utils::fs::getTempFilePath("asm");
  utils::fs::safeOfStream(asmFilePath) << generatedAsm;

  if (options.compileAndAssemble) {
    assemble::runNasmSafe(asmFilePath, options.outputFile);
    return 0;
  }

  auto objFilePath = utils::fs::getTempFilePath("o");
  assemble::runNasmSafe(asmFilePath, objFilePath);

  linking::runLdSafe(objFilePath, options.outputFile, options.createSharedLib);
}
