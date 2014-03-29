#pragma once
#include "Lexer.h"
#include "Atom.h"

#include <vector>

enum ParseStatus {
  // All's good.
  PARSE_OK = 0,

  // The lexer hit the end of the file.
  PARSE_EOF,

  // The tokens are present, but they break some rule about token ordering.
  PARSE_BADSEQUENCE,

  // Something in the lexer broke.
  PARSE_ERROR,
};

//-----------------------------------------------------------------------------

enum ParseNodeType {
  PN_DECLARE,
  PN_ASSIGN,
  PN_EVAL,

  PN_UNOP,
  PN_BINOP,
  PN_TRIOP,

  PN_FUNCTION,
  PN_LITERAL,
  PN_ATOM,
  PN_TUPLE,
  PN_LIST,
  PN_MAP,
  PN_NIL,
};


//-----------------------------------------------------------------------------

class ParseNode {
  ParseNodeType type;
  Token* value;

  ParseNode* a;
  ParseNode* b;
  ParseNode* c;

  ParseNode* next;
};

//-----------------------------------------------------------------------------

class Parser {
public:
  int parse(Lexer* lex_);

private:

  void push(ParseNode* node) {
    stack.push_back(node);
  }

  ParseNode* pop() {
    if (stack.empty()) return NULL;
    ParseNode* result = stack.back();
    stack.pop_back();
    return result;
  }

  int parseLhs();
  int parseRhs();

  // foo.bar.baz.glomp.stuff = ...
  int parsePath();

  int parseBlock();
  int parseTuple();
  int parseExpression();
  int parseStatement();

  Lexer* lex;

  std::vector<ParseNode*> stack;
  std::vector<ParseNode*> expressions;
};