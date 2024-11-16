#pragma once

#include <fstream>
#include <functional>

#include "lexer.ipp"
#include "ast/nodes.hpp"
#include "core/errors.hpp"
#include "core/utils.hpp"

namespace parser
{

namespace { using namespace lexer; }

class Parser
{
public:
  Parser(Lexer &&lexer): _lexer{std::move(lexer)} {}
  Parser(std::ifstream &&inputFile): _lexer(inputFile) {}

  ast::TranslationUnit parseProgram() {
    std::vector<ast::Function> funcList{};
    nextToken();
    funcList.emplace_back(parseFunction());
    match(TT_END);
    return funcList;
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

  template<typename T, TokenType ttSeparator, TrailingSeparator trailingMode, TokenType ttBreaker = TT_NONE>
  inline std::vector<T> parseSeparatedList(std::function<T()> parseElement) {
    std::vector<T> elementList;

    while (ttBreaker == TT_NONE || _currentToken.type != ttBreaker)
    {
      elementList.push_back(parseElement());

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

    return std::move(elementList);
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

  ast::Type parseType()
  {
    std::string_view pureType = parsePureType();
    int pointerDepth = 0;
    while (_currentToken.type == TT_STAR)
    {
      match(TT_STAR);
      pointerDepth++;
    }
    return { pureType, pointerDepth };
  }

  ast::Function parseFunction()
  {
    return {
      .returnType = parseType(),
      .name = match(TT_IDENT),
      .parameters = parseFunctionParams(),
      .body = parseCodeBlock(),
    };
  }

  ast::FunctionParameter parseSingleParam()
  {
    auto type = parseType();
    std::string_view name{};
    if (_currentToken.type == TT_IDENT) name = match(TT_IDENT);

    return {
      .type = type,
      .name = name,
    };
  }

  ast::FunctionParameterList parseFunctionParams()
  {
    match(TT_LPAR);
    auto functionParams = parseSeparatedList<ast::FunctionParameter, TT_COMMA, TRAILING_FORBIDDEN, TT_NONE>([this]() { return parseSingleParam(); });
    match(TT_RPAR);
    return functionParams;
  }

  ast::Expression parseNumberLiteral()
  {
    auto numberView = match(TT_NUMBER);
    return ast::NumberLiteral{utils::readNumber<ast::NumberLiteral::UnderlyingT>(numberView)};
  }

  ast::Expression parseExpression()
  {
    return parseNumberLiteral();
  }

  ast::Instruction parseSingleInstruction()
  {
    match(TT_K_RETURN);
    auto expression = parseExpression();
    return ast::ReturnStatement{std::make_unique<ast::Expression>(std::move(expression))};
  }

  ast::CodeBlock parseCodeBlock()
  {
    match(TT_LCURL);
    auto instructions = parseSeparatedList<ast::Instruction, TT_SEMI, TRAILING_REQUIRED, TT_RCURL>([this]() { return parseSingleInstruction(); });
    match(TT_RCURL);
    return instructions;
  }

private:
  Lexer _lexer;
  Token _currentToken;
};

} /* namespace parser */
