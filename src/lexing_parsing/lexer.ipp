#pragma once

#include <optional>
#include <ostream>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "dbg/errors.hpp"
#include "dbg/logger.hpp"

namespace lexer
{

constexpr size_t MAX_TOKEN_SIZE = 50;

namespace keywords
{
  constexpr const char KW_IF[] = "if";
  constexpr const char KW_ELSE[] = "else";
  constexpr const char KW_WHILE[] = "while";
  constexpr const char KW_DO[] = "do";
  constexpr const char KW_FOR[] = "for";
  constexpr const char KW_RETURN[] = "return";
  constexpr const char KW_EXTERN[] = "extern";
  constexpr const char KW_CLASS[] = "class";
  constexpr const char KW_PUBLIC[] = "public";
  constexpr const char KW_PROTECTED[] = "protected";
  constexpr const char KW_PRIVATE[] = "private";
  constexpr const char KW_INT[] = "int";
  constexpr const char KW_VOID[] = "void";
  constexpr const char KW_CHAR[] = "char";
  constexpr const char KW_ASM[] = "asm";
}

#define PURE_TYPES_TOKEN_LIST \
  X(TT_K_INT, "TT_K_INT") \
  X(TT_K_VOID, "TT_K_VOID") \
  X(TT_K_CHAR, "TT_K_CHAR")

#define TOKEN_LIST \
    X(TT_NONE, "TT_NONE") \
    \
    X(TT_NUMBER, "TT_NUMBER") \
    X(TT_IDENT, "TT_IDENT") \
    \
    X(TT_K_IF, "TT_K_IF") \
    X(TT_K_ELSE, "TT_K_ELSE") \
    X(TT_K_WHILE, "TT_K_WHILE") \
    X(TT_K_DO, "TT_K_DO") \
    X(TT_K_FOR, "TT_K_FOR") \
    X(TT_K_RETURN, "TT_K_RETURN") \
    X(TT_K_EXTERN, "TT_K_EXTERN") \
    X(TT_K_CLASS, "TT_K_CLASS") \
    X(TT_K_PUBLIC, "TT_K_PUBLIC") \
    X(TT_K_PROTECTED, "TT_K_PROTECTED") \
    X(TT_K_PRIVATE, "TT_K_PRIVATE") \
    \
    PURE_TYPES_TOKEN_LIST \
    \
    X(TT_K_ASM, "TT_K_ASM") \
    \
    X(TT_EQUAL, "TT_EQUAL") \
    X(TT_PLUS, "TT_PLUS") \
    X(TT_MINUS, "TT_MINUS") \
    X(TT_STAR, "TT_STAR") \
    X(TT_SLASH, "TT_SLASH") \
    \
    X(TT_CMP_EQ, "TT_CMP_EQ") \
    X(TT_CMP_NEQ, "TT_CMP_NEQ") \
    X(TT_CMP_LEQ, "TT_CMP_LEQ") \
    X(TT_CMP_GEQ, "TT_CMP_GEQ") \
    X(TT_CMP_LT, "TT_CMP_LT") \
    X(TT_CMP_GT, "TT_CMP_GT") \
    \
    X(TT_LBRACK, "TT_LBRACK") \
    X(TT_RBRACK, "TT_RBRACK") \
    X(TT_LPAR, "TT_LPAR") \
    X(TT_RPAR, "TT_RPAR") \
    X(TT_LCURL, "TT_LCURL") \
    X(TT_RCURL, "TT_RCURL") \
    X(TT_DOUBLE_QUOTE, "TT_DOUBLE_QUOTE") \
    \
    X(TT_AMPERSAND, "TT_AMPERSAND") \
    \
    X(TT_COMMA, "TT_COMMA") \
    X(TT_COLON, "TT_COLON") \
    X(TT_SEMI, "TT_SEMI") \
    \
    X(TT_END, "TT_END")

enum TokenType {
#define X(token, str) token,
    TOKEN_LIST
#undef X
};

const char *tokenToString(TokenType token) {
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

  inline FilePosition currentPosition(size_t offsetCorrection = 0)
  {
    return {
      _lineCount,
      _pos - _lineStartOffset - offsetCorrection,
      currentLine(),
    };
  }

  inline Token createToken(TokenType type, std::string_view value, size_t offsetCorrection = 0)
  {
    return Token {
      type,
      value,
      currentPosition(offsetCorrection),
    };
  }

  inline std::string_view getRawUntil(char breaker)
  {
    auto startPos = _pos;
    while (peek() != breaker)
    {
      if (peek() == '\n') incrementLineCount();
      _pos++;
    }
    return std::string_view(_content.data()+startPos, _pos-startPos);
  }

  Token nextToken() {
    std::string currentStr{};
    while (_pos < _content.size())
    {
      if (skipIgnoredCharacters()) continue;

      char current = peek();

      if (std::isdigit(current)) return number();
      if (std::isalpha(current)) return identifier();

      if (auto doubleChar = peekDoubleCharOperation()) {
        _pos += 2;
        return *doubleChar;
      }

      if (current == '=') return createToken(TT_EQUAL, std::string_view(_content.data()+_pos++, 1));
      if (current == '+') return createToken(TT_PLUS, std::string_view(_content.data()+_pos++, 1));
      if (current == '-') return createToken(TT_MINUS, std::string_view(_content.data()+_pos++, 1));
      if (current == '*') return createToken(TT_STAR, std::string_view(_content.data()+_pos++, 1));
      if (current == '/') return createToken(TT_SLASH, std::string_view(_content.data()+_pos++, 1));

      if (current == '<') return createToken(TT_CMP_LT, std::string_view(_content.data()+_pos++, 1));
      if (current == '>') return createToken(TT_CMP_GT, std::string_view(_content.data()+_pos++, 1));

      if (current == '[') return createToken(TT_LBRACK, std::string_view(_content.data()+_pos++, 1));
      if (current == ']') return createToken(TT_RBRACK, std::string_view(_content.data()+_pos++, 1));
      if (current == '(') return createToken(TT_LPAR, std::string_view(_content.data()+_pos++, 1));
      if (current == ')') return createToken(TT_RPAR, std::string_view(_content.data()+_pos++, 1));
      if (current == '{') return createToken(TT_LCURL, std::string_view(_content.data()+_pos++, 1));
      if (current == '}') return createToken(TT_RCURL, std::string_view(_content.data()+_pos++, 1));

      if (current == ',') return createToken(TT_COMMA, std::string_view(_content.data()+_pos++, 1));
      if (current == ':') return createToken(TT_COLON, std::string_view(_content.data()+_pos++, 1));
      if (current == ';') return createToken(TT_SEMI, std::string_view(_content.data()+_pos++, 1));

      if (current == '&') return createToken(TT_AMPERSAND, std::string_view(_content.data()+_pos++, 1));

      if (current == '"') return createToken(TT_DOUBLE_QUOTE, std::string_view(_content.data()+_pos++, 1));

      USER_THROW("Lexing failure: unknown character at pos[" << _pos << "]: [" << current << "]", currentPosition());
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

  static std::string replaceEscapes(std::string_view input) {
    std::string result;
    result.reserve(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
      if (input[i] == '\\')
      {
        if (i + 1 < input.size())
        {
          switch (input[i + 1])
          {
            case 't':   result += '\t'; break;
            case 'n':   result += '\n'; break;
            case '\\':  result += '\\'; break;
            case '"':   result += '"';  break;
            default: THROW("Unknown escape sequence: \\" + std::string(1, input[i + 1]));
          }
          ++i;
        }
        else
        {
          THROW("Incomplete escape sequence at end of string");
        }
      } else {
        result += input[i];
      }
    }

    return result;
  }

private:
  size_t numCharsLeft() { return _content.size() - _pos - 1; }
  bool isEnd() { return !numCharsLeft(); }

  template<size_t lookAhead=0>
  char peek() {
    if constexpr (lookAhead > 0)
      if (numCharsLeft() < lookAhead) return '\0';

    return _content[_pos + lookAhead];
  }

  std::optional<Token> peekDoubleCharOperation() {
    if (isEnd()) return std::nullopt;

    char cur = peek<0>();
    char nxt = peek<1>();

    if (cur == '=' && nxt == '=') return createToken(TT_CMP_EQ, std::string_view(_content.data()+_pos++, 2));
    if (cur == '!' && nxt == '=') return createToken(TT_CMP_NEQ, std::string_view(_content.data()+_pos++, 2));
    if (cur == '<' && nxt == '=') return createToken(TT_CMP_LEQ, std::string_view(_content.data()+_pos++, 2));
    if (cur == '>' && nxt == '=') return createToken(TT_CMP_GEQ, std::string_view(_content.data()+_pos++, 2));

    return std::nullopt;
  }

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
    if (value == keywords::KW_DO) return createToken(TT_K_DO, keywords::KW_DO);
    if (value == keywords::KW_FOR) return createToken(TT_K_FOR, keywords::KW_FOR);
    if (value == keywords::KW_RETURN) return createToken(TT_K_RETURN, keywords::KW_RETURN);
    if (value == keywords::KW_EXTERN) return createToken(TT_K_EXTERN, keywords::KW_EXTERN);
    if (value == keywords::KW_CLASS) return createToken(TT_K_CLASS, keywords::KW_CLASS);
    if (value == keywords::KW_PUBLIC) return createToken(TT_K_PUBLIC, keywords::KW_PUBLIC);
    if (value == keywords::KW_PROTECTED) return createToken(TT_K_PROTECTED, keywords::KW_PROTECTED);
    if (value == keywords::KW_PRIVATE) return createToken(TT_K_PRIVATE, keywords::KW_PRIVATE);

    if (value == keywords::KW_INT) return createToken(TT_K_INT, keywords::KW_INT);
    if (value == keywords::KW_VOID) return createToken(TT_K_VOID, keywords::KW_VOID);
    if (value == keywords::KW_CHAR) return createToken(TT_K_CHAR, keywords::KW_CHAR);

    if (value == keywords::KW_ASM) return createToken(TT_K_ASM, keywords::KW_ASM);

    return createToken(TT_IDENT, value);
  }

  inline bool skipIgnoredCharacters()
  {
    size_t firstPos = _pos;
    size_t lastPos;
    do
    {
      lastPos = _pos;
      skipWhitespaces();
      skipNewLines();
      skipComments();
    } while (_pos != lastPos);

    return _pos != firstPos;
  }

  inline void skipWhitespaces()
  {
    while (isWhiteSpace(peek()))
    {
      _pos++;
    }
  }

  inline void incrementLineCount()
  {
    if (isEnd()) return;
    _lineCount++;
    _lineStartOffset = _pos + 1;
  }

  inline void skipNewLines()
  {
    while (peek() == '\n')
    {
      ++_pos;
      incrementLineCount();
    }
  }

  inline void skipComments()
  {
    if (peek() == '/' && peek<1>() == '/')
    {
      auto lineEnd = std::find(_content.begin() + _pos, _content.end(), '\n');
      _pos = std::distance(_content.begin(), lineEnd) + 1;
      incrementLineCount();
    }

    if (peek() == '/' && peek<1>() == '*')
    {
      _pos = _pos + 2;

      while (_pos < _content.size() - 1 && (peek() != '*' || peek<1>() != '/'))
      {
        if (_pos == '\n') incrementLineCount();
        ++_pos;
      }

      if (_pos >= _content.size() - 1) _pos = _content.size();
      else _pos += 2;
    }
  }

  inline static constexpr bool isWhiteSpace(char a)
  {
    return a == ' ' || a == '\t';
  }

private:
  std::string _content;
  Token _currentToken;

  size_t _pos = 0;
  size_t _lineCount = 0;
  size_t _lineStartOffset = 0;
};

} /* namespace lexer */
