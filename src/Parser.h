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

  ParseStatus parse();

  void dump();

  Lexer lex;

private:

  // Given a name, finds the atom in scope (local or global) that matches it.
  // Returns the nil atom if not found.
  Atom resolve(string& name);

  ParseStatus skipExpected(TokenType type, TokenValue value);
  ParseStatus skipOptional(TokenType type, TokenValue value);

  int getPC();

  ParseStatus emit (Opcode op, int regA, int regB = 0);

  ParseStatus emitEval();

  int jumpTo(int pc);
  int patchJumpFrom (int pc);

  int allocateVariable(string& name);
  int addConstant(const Atom& c);

  ParseStatus evalLiteral();
  ParseStatus evalExpression();

  ParseStatus pushSymbol(Token& t);
  ParseStatus pushOperator(Token& t);
  ParseStatus popOperator();

  // Parse methods consume tokens and place code pieces on the stack.

  ParseStatus parseAtom();
  ParseStatus parseTuple();

  ParseStatus parseFunction();

  ParseStatus parseCall();
  ParseStatus parseDecl();
  ParseStatus parseCallOrDecl();

  ParseStatus parseLhsAtom();
  ParseStatus parseLhs();
  ParseStatus parseRhs();

  ParseStatus parseExpression();

  ParseStatus parseStatementList();
  ParseStatus parseStatement();

  // foo.bar.baz.glomp.stuff = ...
  ParseStatus parsePath();

  ParseStatus parseConditional();
  ParseStatus parseWhile();

  ParseStatus openBlock();
  ParseStatus parseBlock();
  ParseStatus closeBlock();

  std::vector<Atom> stack;
  // The start of the current stack frame.
  int stackBase;

  // Atoms below localTop in the stack correspond to named local variables.
  int localTop;

  Function* currentFunction;

  std::vector<Token> op_stack;

  //std::vector<ParseNode*> stack;
  //std::vector<ParseNode*> expressions;

  AtomMap globals;
};