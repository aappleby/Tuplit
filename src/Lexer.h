#pragma once
#include <vector>

#include "Token.h"

enum LexStatus {
  // All's good.
  LEX_OK = 0,

  // The lexer hit the end of the file.
  LEX_EOF,

  // The input contained an unlexable character.
  LEX_BADCHARACTER,

  // The lexer found a token, but the token was malformed.
  LEX_BADTOKEN,

  // The tokens are present, but they break some rule about token ordering.
  LEX_BADSEQUENCE,

  // Something in the lexer broke.
  LEX_ERROR,
};

//-----------------------------------------------------------------------------

class Lexer {
public:
  Lexer() {
    read_cursor = 0;
  };
  int lex(std::string& code);

  void dump();

  void reset() {
    for (size_t i = 0; i < tokens.size(); i++) {
      // Delete storage for any strings that weren't in our code blob.
      if (tokens[i].start < codeStart ||
          tokens[i].start > codeEnd) {
        delete [] tokens[i].start;
        tokens[i].start = NULL;
      }
    }

    tokens.clear();
    scratch.clear();
  }

  void start() {
    read_cursor = 0;
  }

  Token operator[](int offset) {
    return tokens[read_cursor + offset];
  }

  void operator++(int) {
    read_cursor++;
  }
  void operator += (int offset) {
    read_cursor += offset;
  }

  Token get(int offset = 0) {
    return tokens[read_cursor + offset];
  }

  Token next(int offset = 1) {
    read_cursor += offset;
    return get(read_cursor);
  }

  void stripComments(void) {
    std::vector<Token> newTokens;
    for (size_t i = 0; i < tokens.size(); i++) {
      Token& t = tokens[i];
      if (t.type != TT_COMMENT) newTokens.push_back(t);
    }
    tokens.swap(newTokens);
  }

  bool isEOF() {
    return tokens[read_cursor].type == TT_EOF;
  }
  
protected:

  void addToken(Token t);

  int lexEOF      ();
  int lexInvalid  ();

  int lexSpace    ();
  int lexOperator ();
  int lexDelimiter();
  int lexKeyword();
  int lexNative();
  int lexIdentifier();
  int lexComment();
  int lexDirective();

  int lexString   ();
  int lexNumber   ();
  int lexNewline  ();

  int lexRawString();
  int lexBase64String();
  int lexRune();

  //----------

  std::vector<Token> tokens;
  std::string scratch;

  int read_cursor;
  int linecount;

  const char* codeStart;
  const char* codeEnd;

  const char* cursor;

  bool skipComments_;
};

//-----------------------------------------------------------------------------