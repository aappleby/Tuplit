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
public:
  Function* context;
  string name;
  string file;
  int line;
  int startPC;
  int endPC;
};

//-----------------------------------------------------------------------------

class Block {
public:
  int startPC;
  int endPC;
};

//-----------------------------------------------------------------------------

class Module {
  std::vector<Function> functions;
};

//-----------------------------------------------------------------------------

class Parser {
public:
  Parser();

  int parse();

  void dump();

  Lexer lex;

private:

  // Given a name, finds the atom in scope (local or global) that matches it.
  // Returns the nil atom if not found.
  Atom resolve(string& name);

  ParseStatus skipExpected(TokenType type, TokenValue value);
  ParseStatus skipOptional(TokenType type, TokenValue value);

  int getPC();

  int emit (Opcode op, int regA, int regB = 0);

  int jumpTo(int pc);
  int patchJumpFrom (int pc);

  int allocateVariable(string& name);
  int addConstant(const Atom& c);

  int evalLiteral();
  int evalExpression();

  // Parse methods consume tokens and place code pieces on the stack.

  int parseAtom();
  int parseTuple();

  int parseFunction();

  int parseCall();
  int parseDecl();
  int parseCallOrDecl();

  int parseLhsAtom();
  int parseLhs();
  int parseRhs();

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

  std::vector<Atom> constants;
  std::vector<Atom> stack;
  // The start of the current stack frame.
  int stackBase;

  // Atoms below localTop in the stack correspond to named local variables.
  int localTop;

  Function* currentFunction;

  //std::vector<ParseNode*> stack;
  //std::vector<ParseNode*> expressions;

  AtomMap globals;
};