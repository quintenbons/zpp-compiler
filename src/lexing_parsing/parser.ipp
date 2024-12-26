#pragma once

#include <fstream>
#include <functional>
#include <optional>

#include "ast/scopes/registers.hpp"
#include "dbg/logger.hpp"
#include "lexer.ipp"
#include "ast/nodes/nodes.h"
#include "dbg/errors.hpp"
#include "dbg/utils.hpp"
#include "ast/literalTypes.hpp"

namespace parser
{

namespace { using namespace lexer; }

class Parser
{
private:
  using BinOp = ast::BinaryOperation::Operation;

public:
  Parser(Lexer &&lexer): _lexer{std::move(lexer)} {}
  Parser(std::ifstream &&inputFile): _lexer(inputFile) {}

  ast::TranslationUnit parseTranslationUnit() {
    std::vector<ast::FunctionDeclaration> funcDeclarations{};
    std::vector<ast::Function> funcList{};
    std::vector<ast::Class> classList{};
    nextToken();
    while (_currentToken.type != TT_END)
    {
      if (_currentToken.type == TT_K_CLASS)
        classList.emplace_back(parseClass());
      // TODO not assume that all function declarations are extern
      else if (_currentToken.type == lexer::TT_K_EXTERN)
        funcDeclarations.emplace_back(parseFunctionDeclaration());
      else
        funcList.emplace_back(parseFunction());
    }
    match(TT_END);

    return ast::TranslationUnit(std::move(funcDeclarations), std::move(funcList), std::move(classList));
  }

private:
  BinOp getBinaryOperation(TokenType op) {
    switch (op) {
      case TT_PLUS: return BinOp::ADD;
      case TT_MINUS: return BinOp::SUBSTRACT;
      case TT_STAR: return BinOp::MULTIPLY;
      case TT_SLASH: return BinOp::DIVIDE;
      default: return BinOp::NOT_AN_OPERATION;
    }
  }
  bool isBinaryOp(TokenType op) { return getBinaryOperation(op) != BinOp::NOT_AN_OPERATION; }

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

  bool maybeMatch(TokenType type) {
    if (_currentToken.type == type) {
      match(type);
      return true;
    }
    return false;
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
    /* if (_currentToken.type == TT_K_INT) return match(TT_K_INT); */
    /* if (_currentToken.type == TT_K_VOID) return match(TT_K_VOID); */
    /* ... */
#define X(token, str) \
    if (_currentToken.type == token) return match(token);
    PURE_TYPES_TOKEN_LIST
#undef X

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

    return ast::Type(pureType, pointerDepth);
  }

  ast::FunctionDeclaration parseFunctionDeclaration()
  {
    bool isExtern = maybeMatch(lexer::TT_K_EXTERN);
    ast::Type returnType = parseType();
    std::string_view name = match(TT_IDENT);
    ast::FunctionParameterList parametersNode = parseFunctionParams();
    match(TT_SEMI);
    return ast::FunctionDeclaration(isExtern, std::move(returnType), name, std::move(parametersNode));
  }

  ast::Function parseFunction()
  {
    // TODO use parseFunctionDeclaration
    ast::Type returnType = parseType();
    std::string_view name = match(TT_IDENT);
    ast::FunctionParameterList parametersNode = parseFunctionParams();
    ast::CodeBlock body = parseCodeBlock();
    return ast::Function(std::move(returnType), name, std::move(parametersNode), std::move(body));
  }

  ast::Class parseClass() {
    match(TT_K_CLASS);
    std::string_view name = match(TT_IDENT);
    match(TT_LCURL);

    ast::Class::MethodList methods;
    ast::Class::AttributeList attributes;

    while (_currentToken.type != TT_RCURL) {
      ast::AccessSpecifier attribute_specifier(ast::Visibility::Public);
      ast::Type type = parseType();
      std::string_view name = match(TT_IDENT);
      if (_currentToken.type == TT_LPAR) {
        ast::FunctionParameterList parametersNode = parseFunctionParams();
        ast::CodeBlock body = parseCodeBlock();
        ast::Method method(std::move(type), name, std::move(parametersNode), std::move(body));
        methods.emplace_back(std::move(method), std::move(attribute_specifier));
      }
      else {
        ast::Attribute attribute(std::move(type), ast::Variable(std::move(name)));
        attributes.emplace_back(std::move(attribute), std::move(attribute_specifier));
        match(TT_SEMI);
      }
    }
    match(TT_RCURL);
    match(TT_SEMI);

    return ast::Class(name, std::move(attributes), std::move(methods));
  }

  ast::FunctionParameter parseSingleParam()
  {
    auto type = parseType();
    std::string_view name{};
    if (_currentToken.type == TT_IDENT) name = match(TT_IDENT);

    return ast::FunctionParameter(std::move(type), ast::Variable(std::move(name)));
  }

  decltype(auto) parseCondition() {
    // TODO: conditions can also be declarations
    return parseExpression();
  }

  ast::ConditionalStatement parseConditionalStatement() {
    match(TT_K_IF);
    match(TT_LPAR);
    auto condition = parseCondition();
    match(TT_RPAR);
    auto ifBody = parseCodeBlock();

    if (_currentToken.type == TT_K_ELSE) {
      match(TT_K_ELSE);
      if (_currentToken.type == TT_K_IF) {
        return ast::ConditionalStatement(std::move(condition), std::move(ifBody), ast::CodeBlock(std::vector<ast::Statement>{ast::Statement(parseConditionalStatement())}));
      }
      return ast::ConditionalStatement(std::move(condition), std::move(ifBody), parseCodeBlock());
    } 
    else return ast::ConditionalStatement(std::move(condition), std::move(ifBody));
  }

  ast::ConditionalStatement parseWhileStatement() {
    TODO("Implement while statement");
  }

  ast::ConditionalStatement parseDoStatement() {
    TODO("Implement while statement");
  }

  ast::ForStatement parseForStatement() {
    // TODO: for (for-range-declaration : expression) statement
    match(TT_K_FOR);
    match(TT_LPAR);

    // TODO: initStatement can also be an expression (or empty?)
    auto initStatement = parseDeclaration();
    match(TT_SEMI);

    std::optional<ast::Expression> condition = std::nullopt;
    if (_currentToken.type != TT_SEMI) condition.emplace(parseCondition());
    match(TT_SEMI);

    std::optional<ast::Expression> expr = std::nullopt;
    if (_currentToken.type != lexer::TT_RPAR) expr.emplace(parseExpression());
    match(TT_RPAR);

    // TODO: should be statement
    auto body = parseCodeBlock();

    return ast::ForStatement(std::move(initStatement), std::move(condition), std::move(expr), std::move(body));
  }

  ast::FunctionParameterList parseFunctionParams()
  {
    match(TT_LPAR);
    auto functionParams = parseSeparatedList<ast::FunctionParameter, TT_COMMA, TRAILING_FORBIDDEN, TT_RPAR>([this]() { return parseSingleParam(); });
    match(TT_RPAR);
    return ast::FunctionParameterList(std::move(functionParams));
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

    return ast::StringLiteral(content);
  }

  ast::NumberLiteral parseNumberLiteral()
  {
    auto numberView = match(TT_NUMBER);
    ast::NumberLiteralUnderlyingType number = utils::readNumber<ast::NumberLiteralUnderlyingType>(numberView);
    return ast::NumberLiteral(number);
  }

  ast::Expression parseTerm()
  {
    if (_currentToken.type == TT_IDENT)
    {
      std::string_view ident = match(TT_IDENT);
      if (_currentToken.type == TT_LPAR) return ast::Expression(parseFunctionCall(ident));

      ast::Variable var = ast::Variable(std::move(ident));
      if (!maybeMatch(lexer::TT_EQUAL)) return ast::Expression(std::move(var));

      auto expr = parseExpression();
      return ast::Expression(ast::Assign(std::move(var), std::move(expr)));
    }

    auto numberLiteral = parseNumberLiteral();
    return ast::Expression(std::move(numberLiteral));
  }

  ast::Expression parseExpression()
  {
    // Trivial expression
    auto expr = parseTerm();
    if (!isBinaryOp(_currentToken.type)) return expr;

    // Composed expression
    auto lhs = std::make_unique<ast::Expression>(std::move(expr));

    for (auto op = getBinaryOperation(_currentToken.type); op != BinOp::NOT_AN_OPERATION; op = getBinaryOperation(_currentToken.type)) {
      nextToken();
      auto rhs = std::make_unique<ast::Expression>(parseTerm());
      lhs = std::make_unique<ast::Expression>(ast::BinaryOperation(op, std::move(lhs), std::move(rhs)));
    }

    // ugly. we should just return unique_ptrs for all parsing methods instead
    return std::move(*lhs);
  }

  ast::ReturnStatement parseReturnStatement()
  {
    match(TT_K_RETURN);
    auto expression = parseExpression();
    return ast::ReturnStatement(std::move(expression));
  }

  ast::InlineAsmStatement::BindingRequest parseBindingRequest()
  {
    scopes::Register registerTo = parseRegisterName();
    match(lexer::TT_LPAR);
    auto ident = match(TT_IDENT);
    match(lexer::TT_RPAR);

    // plain old data
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

    return ast::InlineAsmStatement(std::move(asmBlock), std::move(requests));
  }

  ast::Instruction parseSingleInstruction()
  {
    switch ( _currentToken.type )
    {
      case lexer::TT_K_RETURN:
        return ast::Instruction(parseReturnStatement());
      case lexer::TT_K_ASM:
        return ast::Instruction(parseInlineAsmStatement());
      case lexer::TT_IDENT:
        return ast::Instruction(parseExpression());
      /* case lexer::TT_K_INT: return ast::Instruction(...); */
      /* case lexer::TT_K_VOID: return ast::Instruction(...); */
      /* ... */
      #define X(token, str) \
        case token: return ast::Instruction(parseDeclaration());
        PURE_TYPES_TOKEN_LIST
      #undef X
      default:
        USER_THROW("Unexpected token while parsing instruction [" << _currentToken.type << "]", _currentToken.position);
    }
  }

  ast::Declaration parseDeclaration()
  {
    ast::Type type = parseType();
    std::string_view name = match(TT_IDENT);
    if (_currentToken.type == TT_EQUAL)
    {
      match(TT_EQUAL);
      auto expression = parseExpression();
      return ast::Declaration(std::move(type), ast::Variable(std::move(name)), std::move(expression));
    }
    else 
    {
      return ast::Declaration(std::move(type), ast::Variable(std::move(name)));
    }
  }

  ast::FunctionCall parseFunctionCall(std::string_view name)
  {
    match(TT_LPAR);
    std::vector<ast::Expression> arguments;
    while (_currentToken.type != TT_RPAR)
    {
      arguments.push_back(parseExpression());
      if (_currentToken.type != TT_RPAR)
        match(TT_COMMA);
    }
    match(TT_RPAR);
    return ast::FunctionCall(name, std::move(arguments));
  }

  ast::Statement parseStatement() 
  {
    if (_currentToken.type == TT_LCURL) return ast::Statement(parseCodeBlock());
    if (_currentToken.type == TT_K_IF) return ast::Statement(parseConditionalStatement());
    if (_currentToken.type == TT_K_WHILE) return ast::Statement(parseWhileStatement());
    if (_currentToken.type == TT_K_DO) return ast::Statement(parseDoStatement());
    if (_currentToken.type == TT_K_FOR) return ast::Statement(parseForStatement());
    auto statement = ast::Statement(parseSingleInstruction());
    match(lexer::TT_SEMI);
    return statement;
  }

  ast::CodeBlock parseCodeBlock()
  {
    match(TT_LCURL);
    std::vector<ast::Statement> statements;
    while (_currentToken.type != TT_RCURL)
    {
      statements.push_back(parseStatement());
      maybeMatch(TT_SEMI);
    }
    match(TT_RCURL);
    return ast::CodeBlock(std::move(statements)); 
  }

private:
  Lexer _lexer;
  Token _currentToken;
};

} /* namespace parser */
