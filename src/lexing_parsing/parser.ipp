#pragma once

#include <fstream>
#include <functional>

#include "ast/scopes/registers.hpp"
#include "lexer.ipp"
#include "ast/nodes.hpp"
#include "dbg/errors.hpp"
#include "dbg/utils.hpp"

namespace parser
{

namespace { using namespace lexer; }

class Parser
{
public:
  Parser(Lexer &&lexer): _lexer{std::move(lexer)} {}
  Parser(std::ifstream &&inputFile): _lexer(inputFile) {}

  ast::TranslationUnit parseTranslationUnit() {
    std::vector<ast::Function> funcList{};
    std::vector<ast::Class> classList{};
    nextToken();
    while (_currentToken.type != TT_END)
    {
      if (_currentToken.type == TT_K_CLASS)
        classList.emplace_back(parseClass());
      else
        funcList.emplace_back(parseFunction());
    }
    match(TT_END);

    return {
      .functions = std::move(funcList),
      .classes = std::move(classList),
    };
  }

private:
  inline std::string_view getRawUntil(char breaker)
  {
    std::string_view raw = _lexer.getRawUntil(breaker);
    _currentToken = _lexer.nextToken();
    return raw;
  }

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

  ast::Class parseClass() {
    nextToken();
    ast::Class c;
    c.name = match(TT_IDENT);

    match(TT_LCURL);
    while (_currentToken.type != TT_RCURL) {
      ast::AccessSpecifier attribute_specifier { .level = ast::LevelSpecifier::Private };
      ast::Type type = parseType();
      std::string_view name = match(TT_IDENT);
      if (_currentToken.type == TT_LPAR) {
        ast::Function method = {
          .returnType = type,
          .name = name,
          .parameters = parseFunctionParams(),
          .body = parseCodeBlock(),
        };
        c.methods.emplace_back(std::make_pair(std::move(method), attribute_specifier));
      }
      else {
        ast::Attribute attribute = {
          .type = type,
          .name = name,
        };
        c.attributes.emplace_back(std::make_pair(attribute, attribute_specifier));
        match(TT_SEMI);
      }
    }
    match(TT_RCURL);
    match(TT_SEMI);

    return c;
  }

  ast::FunctionParameter parseSingleParam()
  {
    auto type = parseType();
    std::string_view name{};
    if (_currentToken.type == TT_IDENT) name = match(TT_IDENT);

    return {
      .type = std::move(type),
      .name = name,
    };
  }

  ast::FunctionParameterList parseFunctionParams()
  {
    match(TT_LPAR);
    auto functionParams = parseSeparatedList<ast::FunctionParameter, TT_COMMA, TRAILING_FORBIDDEN, TT_RPAR>([this]() { return parseSingleParam(); });
    match(TT_RPAR);
    return functionParams;
  }

  scopes::Register parseRegisterName()
  {
    std::string_view raw = parseRawSingleStringLiteral();
    USER_ASSERT(raw[0] == '=', "Only ={register} identifiers are supported", _currentToken.position);
    return scopes::strToReg(raw.substr(1));
  }

  // "a" "b" won't work, a single double quoted value is possible here, no escape characters are replaced
  std::string_view parseRawSingleStringLiteral()
  {
    USER_ASSERT(_currentToken.type == lexer::TT_DOUBLE_QUOTE, "Expected double quote for literal", _currentToken.position);
    std::string_view raw = getRawUntil('"');
    match(lexer::TT_DOUBLE_QUOTE);
    return raw;
  }

  // "a" "\tb" is a valid string literal with concat, \t will be replaced by a tab
  ast::StringLiteral parseStringLiteral()
  {
    std::vector<std::string_view> literals;
    size_t totalSize = 0;
    while (_currentToken.type == lexer::TT_DOUBLE_QUOTE)
    {
      std::string_view singleLiteral = parseRawSingleStringLiteral();
      totalSize += singleLiteral.size();
      literals.push_back(std::move(singleLiteral));
    }

    if (literals.empty()) USER_THROW("Expected a valid string literal", _currentToken.position);

    std::string content;
    content.reserve(totalSize+1);
    for (auto &lit: literals) content += lexer::Lexer::replaceEscapes(lit);

    return {
      .content = std::move(content),
    };
  }

  ast::NumberLiteral parseNumberLiteral()
  {
    auto numberView = match(TT_NUMBER);
    return ast::NumberLiteral{utils::readNumber<ast::NumberLiteral::UnderlyingT>(numberView)};
  }

  ast::Expression parseExpression()
  {
    return parseNumberLiteral();
  }

  ast::ReturnStatement parseReturnStatement()
  {
    match(TT_K_RETURN);
    auto expression = parseExpression();
    return ast::ReturnStatement{std::make_unique<ast::Expression>(std::move(expression))};
  }

  ast::InlineAsmStatement::BindingRequest parseBindingRequest()
  {
    scopes::Register registerTo = parseRegisterName();
    match(lexer::TT_LPAR);
    auto ident = match(TT_IDENT);
    match(lexer::TT_RPAR);

    return {
      .registerTo=registerTo,
      .varIdentifier=std::string(ident),
    };
  }

  ast::InlineAsmStatement parseInlineAsmStatement()
  {
    match(lexer::TT_K_ASM);
    match(lexer::TT_LPAR);

    ast::StringLiteral asmBlock = parseStringLiteral();


    std::vector<ast::InlineAsmStatement::BindingRequest> requests;
    if (_currentToken.type == lexer::TT_COLON)
    {
      match(TT_COLON);
      requests = parseSeparatedList<ast::InlineAsmStatement::BindingRequest, lexer::TT_COMMA, TRAILING_OPTIONAL, lexer::TT_RPAR>([this](){ return parseBindingRequest(); });
    }

    match(lexer::TT_RPAR);

    return {
      .asmBlock=std::move(asmBlock),
      .requests=std::move(requests),
    };
  }

  ast::Instruction parseSingleInstruction()
  {
    switch ( _currentToken.type )
    {
      case lexer::TT_K_RETURN:
        return parseReturnStatement();
      case lexer::TT_K_ASM:
        return parseInlineAsmStatement();
      default:
        USER_THROW("Unexpected token while parsing instruction [" << _currentToken.type << "]", _currentToken.position);
    }
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
