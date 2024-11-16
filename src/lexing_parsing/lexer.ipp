#pragma once

#include <ostream>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "errors.hpp"

constexpr size_t MAX_TOKEN_SIZE = 50;

namespace keywords
{
  constexpr const char KW_IF[] = "if";
  constexpr const char KW_ELSE[] = "else";
  constexpr const char KW_WHILE[] = "while";
  constexpr const char KW_RETURN[] = "return";
  constexpr const char KW_INT[] = "int";
  constexpr const char KW_VOID[] = "void";
  constexpr const char KW_CHAR[] = "char";
}

#define TOKEN_LIST \
    X(TT_NONE, "TT_NONE") \
    \
    X(TT_NUMBER, "TT_NUMBER") \
    X(TT_IDENT, "TT_IDENT") \
    \
    X(TT_K_IF, "TT_K_IF") \
    X(TT_K_ELSE, "TT_K_ELSE") \
    X(TT_K_WHILE, "TT_K_WHILE") \
    X(TT_K_RETURN, "TT_K_RETURN") \
    \
    X(TT_K_INT, "TT_K_INT") \
    X(TT_K_VOID, "TT_K_VOID") \
    X(TT_K_CHAR, "TT_K_CHAR") \
    \
    X(TT_PLUS, "TT_PLUS") \
    X(TT_MINUS, "TT_MINUS") \
    X(TT_STAR, "TT_STAR") \
    X(TT_SLASH, "TT_SLASH") \
    \
    X(TT_LBRACK, "TT_LBRACK") \
    X(TT_RBRACK, "TT_RBRACK") \
    X(TT_LPAR, "TT_LPAR") \
    X(TT_RPAR, "TT_RPAR") \
    X(TT_LCURL, "TT_LCURL") \
    X(TT_RCURL, "TT_RCURL") \
    \
    X(TT_COMMA, "TT_COMMA") \
    X(TT_SEMI, "TT_SEMI") \
    \
    X(TT_END, "TT_END")

enum TokenType {
#define X(token, str) token,
    TOKEN_LIST
#undef X
};

const char* tokenToString(TokenType token) {
    switch (token) {
#define X(token, str) case token: return str;
        TOKEN_LIST
#undef X
        default: return "UNKNOWN";
    }
}

inline std::ostream& operator<<(std::ostream& os, TokenType token) {
    return os << tokenToString(token);
}


struct FilePosition {
  size_t lineCount;
  size_t lineOffset;
  std::string_view lineView;
};

struct Token
{
  TokenType type;
  std::string_view value;
  FilePosition position;
};

class Lexer
{
public:
  Lexer(std::ifstream &inputFile)
  : _content(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>())
  {
  }

  inline Token createToken(TokenType type, std::string_view value, size_t offsetCorrection = 0)
  {
    return Token {
      type,
      value,
      {
        _lineCount,
        _pos - _lineStartOffset - offsetCorrection,
        currentLine(),
      }
    };
  }

  Token nextToken() {
    std::string currentStr{};
    while (_pos < _content.size())
    {
      char current = _content[_pos];

      if (current == '\n')
      {
        _lineCount++;
        _lineStartOffset = _pos + 1;
      }

      if (std::iswspace(current))
      {
        _pos++;
        continue;
      }

      if (std::isdigit(current)) return number();
      if (std::isalpha(current)) return identifier();

      if (current == '+') return createToken(TT_PLUS, std::string_view(_content.data()+_pos++, 1));
      if (current == '-') return createToken(TT_MINUS, std::string_view(_content.data()+_pos++, 1));
      if (current == '*') return createToken(TT_STAR, std::string_view(_content.data()+_pos++, 1));
      if (current == '/') return createToken(TT_SLASH, std::string_view(_content.data()+_pos++, 1));

      if (current == '[') return createToken(TT_LBRACK, std::string_view(_content.data()+_pos++, 1));
      if (current == ']') return createToken(TT_RBRACK, std::string_view(_content.data()+_pos++, 1));
      if (current == '(') return createToken(TT_LPAR, std::string_view(_content.data()+_pos++, 1));
      if (current == ')') return createToken(TT_RPAR, std::string_view(_content.data()+_pos++, 1));
      if (current == '{') return createToken(TT_LCURL, std::string_view(_content.data()+_pos++, 1));
      if (current == '}') return createToken(TT_RCURL, std::string_view(_content.data()+_pos++, 1));

      if (current == ',') return createToken(TT_COMMA, std::string_view(_content.data()+_pos++, 1));
      if (current == ';') return createToken(TT_SEMI, std::string_view(_content.data()+_pos++, 1));

      USER_THROW("Lexing failure: unknown character at pos[" << _pos << "]: [" << current << "]");
    }

    return createToken(TT_END, std::string_view());
  }

  std::string_view currentLine()
  {
    auto start = _content.begin() + _lineStartOffset;
    auto end = std::find_if(start, _content.end(), [](char c) { return c == '\n'; });
    std::string_view value(&*start, std::distance(start, end));
    return value;
  }

private:
  Token number()
  {
    auto start = _content.begin() + _pos;
    auto end = std::find_if(start, _content.end(), [](char c) { return !std::isdigit(c); });
    std::string_view value(&*start, std::distance(start, end));
    _pos += value.size();
    return createToken(TT_NUMBER, value);
  }

  Token identifier()
  {
    auto start = _content.begin() + _pos;
    auto end = std::find_if(start, _content.end(), [](char c) { return !std::isalnum(c); });
    std::string_view value(&*start, std::distance(start, end));
    _pos += value.size();

    if (value == keywords::KW_IF) return createToken(TT_K_IF, keywords::KW_IF);
    if (value == keywords::KW_ELSE) return createToken(TT_K_ELSE, keywords::KW_ELSE);
    if (value == keywords::KW_WHILE) return createToken(TT_K_WHILE, keywords::KW_WHILE);
    if (value == keywords::KW_RETURN) return createToken(TT_K_RETURN, keywords::KW_RETURN);

    if (value == keywords::KW_INT) return createToken(TT_K_INT, keywords::KW_INT);
    if (value == keywords::KW_VOID) return createToken(TT_K_VOID, keywords::KW_VOID);
    if (value == keywords::KW_CHAR) return createToken(TT_K_CHAR, keywords::KW_CHAR);

    return createToken(TT_IDENT, value);
  }

private:
  std::string _content;
  Token _currentToken;

  size_t _pos = 0;
  size_t _lineCount = 0;
  size_t _lineStartOffset = 0;
};
