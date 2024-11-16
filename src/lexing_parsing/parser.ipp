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
    parseFunction();
    match(TT_END);
  }

private:
  inline void skipToken()
  {
    _currentToken = _lexer.nextToken();
  }

  std::string_view match(TokenType type) {
    USER_ASSERT(_currentToken.type == type, "Unexpected token type=[" << _currentToken.type << "] for value=[" << _currentToken.value << "] expected=[" << type << "]", _currentToken.position);
    std::string_view cur = _currentToken.value;
    skipToken();
    return cur;
  }

  void matchIdent(std::string_view ident)
  {
    std::string_view cur = match(TT_IDENT);
    USER_ASSERT(cur == ident, "Specific expected ident was not matched. Expected=[" << ident << "] got=[" << cur << "]");
  }

  std::string_view parsePureType()
  {
    if (_currentToken.type == TT_K_INT) return match(TT_K_INT);
    if (_currentToken.type == TT_K_VOID) return match(TT_K_VOID);
    return match(TT_IDENT);
  }

  std::string_view parseType()
  {
    std::string_view pureType = parsePureType();
    int pointerDepth = 0;
    while (_currentToken.type == TT_STAR)
    {
      match(TT_STAR);
      pointerDepth++;
    }
    std::cout << pureType << pointerDepth << std::endl;
    return pureType;
  }

  void parseFunction()
  {
    std::string returnType(parseType());
    std::string functionName(match(TT_IDENT));
    parseFunctionParams();
    parseCodeBlock();
  }

  void parseFunctionParams()
  {
    match(TT_LPAR);
    while (_currentToken.type != TT_RPAR)
    {
      parseType();
      if (_currentToken.type == TT_IDENT)
      {
        match(TT_IDENT);
      }
      if (_currentToken.type == TT_COMMA) match(TT_COMMA);
    }
    match(TT_RPAR);
  }

  void parseCodeBlock()
  {
    match(TT_LCURL);
    while (_currentToken.type != TT_RCURL)
    {
      parseInstruction();
    }
    match(TT_RCURL);
  }

  void parseInstruction()
  {
    std::cout << "todo" << std::endl;
    skipToken();
  }

private:
  Lexer _lexer;
  Token _currentToken;
};
