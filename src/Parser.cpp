#include "Parser.h"

bool isNativeType() {
  return false;
}

//-----------------------------------------------------------------------------

ParseStatus Parser::skipExpected(TokenType type, TokenValue value) {
  Token t = lex->get();
  if (t.type != type) {
    assert(false);
    return PARSE_BADSEQUENCE;
  }
  if (t.value != value) {
    assert(false);
    return PARSE_BADSEQUENCE;
  }
  return PARSE_OK;
}

//-----------------------------------------------------------------------------

int Parser::patchJumpFrom(int pc) {
  return 0;
}

int Parser::jumpTo(int pc) {
  return 0;
}

int Parser::emit(Opcode op, int regA, int regB) {
  return 0;
}

int Parser::getPC() {
  return 0;
}

/*
eight := 8;

DECLARE
  ATOM "eight" : ? = ?
  ATOM ? : ? = 8
*/

// a : b = c
// a : b
// a     = c
// a
//     b = c
//     b
//         c
int Parser::parseAtom() {
  Token a = lex->next();
  push(new ParseNode(PN_ID, a));
  return PARSE_OK;
}


// a : b
// a
//     b
int Parser::parseLhsAtom() {
  return PARSE_OK;
}

//-----------------------------------------------------------------------------

int Parser::parseLhs() {
  Token t = lex->get();  

  if (t.type == TT_IDENTIFIER) {
    return parseAtom();
  }

  /*
  if (t.type != TT_IDENTIFIER) {
    printf("X!");
    return PARSE_OK;
  }
  */
  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// Returns register containing eval result.

int Parser::parseExpression() {
  Token lhs = lex->next();

  if (lhs.type != TT_IDENTIFIER) {
    printf("X!");
    return PARSE_OK;
  }

  Token op = lex->next();

  assert(op.type == TT_OPERATOR);
  switch(op.value) {
  case OP_DECLARE:
    printf("declare");
    break;
  case OP_EQUALS:
    printf("equals");
    break;
  default:
    assert(false);
    return PARSE_BADSEQUENCE;
  }

  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// Lexer is on the lparen of the args.

int Parser::parseCall(Token functionName) {
  return 0;
}

//-----------------------------------------------------------------------------
// "x = 10", "x := 10", "x()"

int Parser::parseCallOrDecl() {
  Token name = lex->next();
  Token op = lex->peek();

  assert(name.type == TT_IDENTIFIER);
  assert((op.type == TT_OPERATOR) || (op.type == TT_DELIMITER));

  if (op.type == TT_DELIMITER) {
    assert(op.value == DL_LPAREN);
    // Function call.
  }


  return 0;
}

//-----------------------------------------------------------------------------

int Parser::parseStatement() {
  Token t = lex->get();
  if (t.type == TT_KEYWORD) {
    switch(t.value) {
      case KW_WHILE:
        return parseWhile();

      case KW_FOR:
      case KW_SWITCH:
      case KW_DO:
      case KW_CONTINUE:
      case KW_IF:
      case KW_ELSE:
      case KW_BREAK:
      case KW_RETURN:
      case KW_YIELD:
        assert(false && "Keyword not implemented yet");
        break;

      /*
      KW_IN,
      KW_CASE,
      KW_ARGS,
      KW_RESULT,
      KW_TRUE,
      KW_FALSE,
      KW_TYPE,
      KW_FUNCTION,
      KW_DEBUGGER,
      KW_NEW,
      KW_DELETE,
      */

      default:
        return PARSE_BADSEQUENCE;
    }
    return PARSE_OK;
  } else if (t.type == TT_IDENTIFIER) {
    return parseCallOrDecl();
  }
  else {
    return PARSE_BADSEQUENCE;
  }
}

//-----------------------------------------------------------------------------

int Parser::parseStatementList() {
  while(lex->peek().type != TT_DELIMITER || lex->peek().value != DL_RBRACE) {
    int error = parseStatement();
    if (error) return error;
  }

  return 0;
}

//-----------------------------------------------------------------------------

int Parser::openBlock() {
  return 0;
}

int Parser::parseBlock() {
  // open new scope
  // parse statement list
  return 0;
}

int Parser::closeBlock() {
  return 0;
}

//-----------------------------------------------------------------------------
// (condition expression)
// emits code, returns result register

int Parser::parseConditional() {
  // open paren block
  skipExpected(TT_DELIMITER, DL_LPAREN);
  int stackStart = stackTop;
  openBlock();
  parseBlock();
  closeBlock();
  int resultreg = stackTop - 1;
  skipExpected(TT_DELIMITER, DL_RPAREN);
  stackTop = stackStart;
  return resultreg;
}

//-----------------------------------------------------------------------------

int Parser::parseWhile() {
  // skip 'while' token
  lex->next();

  // store pc 
  int whilestart = getPC();

  // emit conditional statement
  int conditionReg = parseConditional();
  
  // emit blank jump if false
  int fixup = emit(OC_JZ, conditionReg, 0);

  // parse block
  parseBlock();

  // emit jump to conditional statement
  jumpTo(whilestart);

  // patch blank jump
  patchJumpFrom(fixup);

  return 0;
};

//-----------------------------------------------------------------------------

int Parser::parse(Lexer* lex_) {
  lex = lex_;
  while(lex->peek().type != TT_EOF) {
    parseStatement();
    if (!stack.empty()) {
      expressions.push_back(stack.back());
      stack.pop_back();
    }
  }
  return PARSE_OK;
}

//-----------------------------------------------------------------------------

void Parser::dump() {
}

void Parser::dumpNode(ParseNode* node, int depth) {
  ParseNode* cursor = node;

  while(cursor) {
    cursor->token.dump();
    cursor = cursor->next;
  }
}



/*
string interpretAtomDeclaration(TokenStream & tokens, Atom& out) {
  string first = tokens[0];

  // this should use context-sensitive type lookup.
  if (isType(first)) {
    // "int = x", "int"
    string second = tokens[1];
    if (second == "," || second == ")") {
      tokens.consume(1);
      return Atom("", first, "");
    }
    if (second == "=") {
      second = tokens[1];
      if (isValue(second)) {
        tokens.consume(2);
        return Atom("", first, second);
      }
      return "Error, value declaration is not a value";
    }
    return "Error, unknown token following type";
  }
  else if (isValue(first)) {
    // "1"
    tokens.consume(1);
    return Atom("", deduceType(first), first);
  }
  else if (isValidName(first)) {
    // "r int = 1", "r int", "r = 1", "r"
    string second = tokens[1];
    if (second == "," || second == ")") {
      tokens.consume(1);
      return Atom(first, "", "");
    }
    // "r int = 1", "r int", "r = 1"
    if (isType(second)) {
      // "r int = 1", "r int"
      string third = tokens[2];
      if (third == "," || third == ")") {
        tokens.consume(2);
        return Atom(first, second, "");
      }
      else if (third == "=") {
        string fourth = tokens[3];
        if (isValue(fourth)) {
          tokens.consume(4);
          return Atom(first, second, fourth);
        }
        else {
          return "Error, value declaration is not a value".
        }
      }
      else {
        return "Error, garbage after atom type declaration";
      }
    }
    else if (second == "=") {
      // "r = 1"
      string third = tokens[2];
      if (isValue(third)) {
        tokens.consume(3);
        out = Atom(first, "", third);
        return "";
      }
    }
    else {
      return "Error, garbage after atom name";
    }
  }
}
*/

// "(r int = 1, g int = 2, b int = 3)"
// "(", "r", "int", "="
/*
int interpretTupleDeclaration(TokenStream& tokens, Tuple& out) {
  int result = 0;
  string openParen = tokens.next();
  assert(openParen == "(");
  vector<Atom> atoms;
  while (tokens.peekNext() != ")") {
    Atom a;
    result = interpretAtomDeclaration(tokens, a);
  }
}
*/
