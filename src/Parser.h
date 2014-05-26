#pragma once
#include "Atom.h"
#include "Lexer.h"
#include "Opcodes.h"
#include "Tuple.h"

#include <vector>
#include <map>

class Function;
class Variable;
class Block;
class Module;

//-----------------------------------------------------------------------------

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
  PN_NIL,

  PN_DECLARE,
  PN_ASSIGN,
  PN_EVAL,

  PN_UNOP,
  PN_BINOP,
  PN_TRIOP,

  PN_ID,
  PN_FUNCTION,
  PN_LITERAL,

  PN_ATOM,
  PN_TUPLE,
  PN_LIST,
  PN_MAP,
};


//-----------------------------------------------------------------------------

class ParseNode {
public:
  ParseNode(ParseNodeType type_, Token token_) : type(type_), token(token_) {
  }

  ParseNodeType type;
  Token token;

  ParseNode* a;
  ParseNode* b;
  ParseNode* c;

  ParseNode* next;
};

//-----------------------------------------------------------------------------

class Variable {
  Function* context;
  string name;
  string file;
  int line;
  int startPC;
  int endPC;
};

//-----------------------------------------------------------------------------

class Block {
  int startPC;
  int endPC;
};

//-----------------------------------------------------------------------------

class Function {
  std::string name;
  Tuple args;
  Tuple results;
  std::vector<Instruction> code;

  std::vector<string> regs;
};

//-----------------------------------------------------------------------------

class Module {
  std::vector<Function> functions;
};

//-----------------------------------------------------------------------------

class Parser {
public:
  int parse(Lexer* lex_);

  void dump();

private:

  void dumpNode(ParseNode* node, int depth);

  void push(ParseNode* node) {
    stack.push_back(node);
  }

  ParseNode* pop() {
    if (stack.empty()) return NULL;
    ParseNode* result = stack.back();
    stack.pop_back();
    return result;
  }

  ParseStatus skipExpected(TokenType type, TokenValue value);

  int getPC();

  int emit (Opcode op, int regA, int regB);

  int jumpTo(int pc);
  int patchJumpFrom (int pc);

  int parseCallOrDecl();

  int parseCall(Token functionName);

  int parseLhsAtom();
  int parseAtom();
  int parseLhs();
  int parseRhs();

  int parseTuple();
  int parseExpression();

  int parseStatementList();
  int parseStatement();

  // foo.bar.baz.glomp.stuff = ...
  int parsePath();

  int parseConditional();
  int parseWhile();

  int openBlock();
  int parseBlock();
  int closeBlock();

  Lexer* lex;

  int stackTop;

  std::vector<ParseNode*> stack;
  std::vector<ParseNode*> expressions;

  std::map<string, Function*> functions;
  std::map<string, Variable*> variables;
};