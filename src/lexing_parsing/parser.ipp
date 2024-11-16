#pragma once

#include <fstream>

#include "lexer.ipp"
#include "errors.hpp"

class Parser
{
public:
  Parser(Lexer &&lexer): _lexer{std::move(lexer)} {}
  Parser(std::ifstream &&inputFile): _lexer(inputFile) {}

  void parseProgram() {
    _currentToken = _lexer.nextToken();
    match(TT_K_INT);
    matchIdent("main");
    match(TT_END);
  }

private:
  void match(TokenType type) {
    if (_currentToken.type == type) _currentToken = _lexer.nextToken();
    else THROW("Unexpected token type=[" << _currentToken.type << "] for value=[" << _currentToken.value << "] expected=[" << type << "]");
  }

  void matchIdent(std::string_view ident)
  {
    ASSERT(_currentToken.value == ident, "what");
    match(TT_IDENT);
  }

private:
  Lexer _lexer;
  Token _currentToken;
};
