#pragma once

#include <fstream>
#include <functional>

#include "lexer.ipp"
#include "errors.hpp"

class Parser
{
public:
  Parser(Lexer &&lexer): _lexer{std::move(lexer)} {}
  Parser(std::ifstream &&inputFile): _lexer(inputFile) {}

  void parseProgram() {
    nextToken();
    parseFunction();
    match(TT_END);
  }

private:
  inline void nextToken()
  {
    _currentToken = _lexer.nextToken();

    // for debug purposes
    // LOG_INLINE(_currentToken.value << " ");
    // static size_t endCount = 0;
    // if (_currentToken.type == TT_END && ++endCount > 10)
    // {
    //   USER_THROW("END TOKEN SEEN TOO MANY TIMES");
    // }
  }

  std::string_view match(TokenType type) {
    USER_ASSERT(_currentToken.type == type, "Unexpected token type=[" << _currentToken.type << "] for value=[" << _currentToken.value << "] expected=[" << type << "]", _currentToken.position);
    std::string_view cur = _currentToken.value;
    nextToken();
    return cur;
  }

  enum TrailingSeparator
  {
    TRAILING_FORBIDDEN,
    TRAILING_OPTIONAL,
    TRAILING_REQUIRED,
  };

  template<TokenType ttSeparator, TrailingSeparator trailingMode, TokenType ttBreaker = TT_NONE>
  inline void parseSeparatedList(std::function<void()> parseElement) {
    while (ttBreaker == TT_NONE || _currentToken.type != ttBreaker)
    {
      parseElement();

      if constexpr (ttSeparator != TT_NONE)
      {
        if (_currentToken.type == ttSeparator) match(ttSeparator);
        else if constexpr (trailingMode == TRAILING_REQUIRED) USER_THROW("Expected trailing " << ttSeparator, _currentToken.position);
        else break;
      }

      if constexpr (ttBreaker != TT_NONE && trailingMode == TRAILING_FORBIDDEN)
      {
        USER_ASSERT(_currentToken.type != ttBreaker, "Found trailing " << ttSeparator << " in list, expected new element", _currentToken.position);
      }
    }
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
    if (_currentToken.type == TT_K_CHAR) return match(TT_K_CHAR);
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
    return pureType;
  }

  void parseFunction()
  {
    std::string returnType(parseType());
    std::string functionName(match(TT_IDENT));
    parseFunctionParams();
    parseCodeBlock();
  }

  void parseSingleParam()
  {
    parseType();
    if (_currentToken.type == TT_IDENT) match(TT_IDENT);
  }

  void parseFunctionParams()
  {
    match(TT_LPAR);
    parseSeparatedList<TT_COMMA, TRAILING_FORBIDDEN>([this]() { parseSingleParam(); });
    match(TT_RPAR);
  }

  void parseExpression()
  {
    match(TT_NUMBER);
  }

  void parseSingleInstruction()
  {
    // TODO more possible instructions
    match(TT_K_RETURN);
    parseExpression();
  }

  void parseCodeBlock()
  {
    match(TT_LCURL);
    parseSeparatedList<TT_SEMI, TRAILING_REQUIRED, TT_RCURL>([this]() { parseSingleInstruction(); });
    match(TT_RCURL);
  }

private:
  Lexer _lexer;
  Token _currentToken;
};
