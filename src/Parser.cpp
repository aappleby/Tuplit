#include "Parser.h"
#include "Function.h"

Parser::Parser() {
  stackBase = 0;
  localTop = 0;

  currentFunction = new Function();

  // Add native types to the global namespace.

  globals["float"] = Atom::make_type("float");
  globals["int"] = Atom::make_type("int");
}

bool isNativeType() {
  return false;
}

//-----------------------------------------------------------------------------

ParseStatus Parser::skipExpected(TokenType type, TokenValue value) {
  Token t = lex[0];
  if (t.type != type) {
    assert(false);
    return PARSE_BADSEQUENCE;
  }
  if (t.value != value) {
    assert(false);
    return PARSE_BADSEQUENCE;
  }
  lex++;
  return PARSE_OK;
}

//----------

ParseStatus Parser::skipOptional(TokenType type, TokenValue value) {
  Token t = lex[0];
  if (t.type != type) {
    return PARSE_OK;
  }
  if (t.value != value) {
    return PARSE_OK;
  }
  lex++;
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
  Instruction o = { op, regA, regB };
  currentFunction->code.push_back(o);
  return PARSE_OK;
}

int Parser::addConstant(Atom const& c) {
  currentFunction->constants.push_back(c);
  return PARSE_OK;
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


// a : b
// a
//     b
int Parser::parseLhsAtom() {
  return PARSE_OK;
}

//-----------------------------------------------------------------------------

int Parser::parseAtom() {
  Token t = lex[0];

  Atom temp;

  bool hasName = false;
  bool hasType = false;
  bool hasValue = false;

  if (t.isIdentifier()) {
    // (a | (b
    // Could be name, type, or function call.
    // Don't bother handling function call for now.

    // See if the identifier matches a known type.
    Atom resolved = resolve(t.text);

    if (resolved.isType()) {
      // must be type
      temp.type_ = resolved.name_;
      hasType = true;
    } else {
      // must be name
      temp.name_ = t.text;
      hasName = true;
    }
  } else if (t.isLiteral()) {
    // (c)
  }

  // First piece done.

  lex++;
  t = lex[0];

  // If the next token is a delimiter, that's the end of the atom.
  if (t.isDelimiter()) {
    stack.push_back(temp);
    return PARSE_OK;
  }

  if (t.isOperator(OP_DECLARE)) {
    // (a := c)
    parseExpression();
    temp.setValue(stack.back());
    stack.pop_back();
    return PARSE_OK;
  }

  if (t.isOperator(OP_EQUALS)) {
    assert(!hasName);
    assert(hasType);
    // (b = c)
    parseExpression();
    temp.setValue(stack.back());
    stack.pop_back();
    return PARSE_OK;
  }

  if (t.isOperator(OP_COLON)) {
    // a : b
    assert(hasName);
    assert(!hasType);
    lex++;
    Token b = lex[0];
    temp.type_ = b.text;
    hasType = true;
    lex++;

    t = lex[0];
    if (!t.isOperator(OP_EQUALS)) {
      assert(false);
    }

    lex++;
    parseExpression();
    temp.setValue(stack.back());
    stack.pop_back();
    return PARSE_OK;
  }

  assert(false);
  return PARSE_ERROR;
}

//-----------------------------------------------------------------------------
// ( <atom>, ... )

int Parser::parseTuple() {
  skipExpected(TT_DELIMITER, DL_LPAREN);

  while(lex[0].type != TT_DELIMITER || lex[0].value != DL_LPAREN) {
    parseAtom();
    // Every atom should be followed by a delimiter.
    if (lex[0].type != TT_DELIMITER) {
      assert(false);
      return PARSE_ERROR;
    }
    skipOptional(TT_DELIMITER, DL_COMMA);
  }

  return PARSE_OK;
}

//-----------------------------------------------------------------------------

int Parser::parseLhs() {
  Token t = lex[0];  

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
  Token t = lex[0];

  if (t.isLiteral()) {
    // TODO(aappleby): handle expression
    switch (t.type) {
      case TT_INTEGER:
        stack.push_back(Atom::value(t.s64));
        lex++;
        return PARSE_OK;
      case TT_FLOAT:
        stack.push_back(Atom::value(t.f64));
        lex++;
        return PARSE_OK;
      case TT_STRING:
        stack.push_back(Atom::value(t.text));
        lex++;
        return PARSE_OK;
      case TT_RUNE:
      default:
        assert(false);
    }
  }

  /*
  Token lhs = lex[0];

  if (lhs.type != TT_IDENTIFIER) {
    printf("X!");
    return PARSE_OK;
  }

  Token op = lex++;

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
  */

  return PARSE_OK;
}

//-----------------------------------------------------------------------------

Atom Parser::resolve(string& name) {
  // TODO(aappleby) - Walk the scope and context chains here.
  for (int i = stackBase; i < localTop; i++) {
    if (stack[i].name_ == name) {
      return stack[i];
    }
  }

  AtomMap::iterator it = globals.find(name);
  if (it != globals.end()) {
    return (*it).second;
  }

  return Atom::nil;
}

//-----------------------------------------------------------------------------

int Parser::allocateVariable(string& name) {
  stack.push_back(Atom::name(name));
  return localTop++;
}

//-----------------------------------------------------------------------------
// <literal_expression>

int Parser::evalLiteral() {
  Token base = lex[0];
  Token op = lex[1];

  assert(op.isDelimiter(DL_SEMICOLON));

  switch(base.type) {
    case TT_KEYWORD:
      // put 'true' or 'false' on stack.
      return PARSE_OK;

    case TT_NATIVE:
      // put type on stack.
      return PARSE_OK;

    case TT_IDENTIFIER:
      // look up identifier & put on stack.
      return PARSE_OK;

    case TT_INTEGER:
      stack.push_back(Atom::value(base.s64));
      break;
    case TT_FLOAT:
      {
        Atom constant = Atom::value(base.f64);
        int cslot = addConstant(constant);
        stack.push_back(constant);
        emit(OC_LOADC, stack.size() - 1, currentFunction->constants.size() - 1);
        lex += 2;
      }
      break;
    case TT_RUNE:
    case TT_STRING:
      // put literal on stack.
      return PARSE_OK;

    case TT_EOF:
    case TT_INVALID:
    case TT_OPERATOR:
    case TT_DELIMITER:
    case TT_DIRECTIVE:
    case TT_COMMENT:
    default:
      assert(false);
      return PARSE_ERROR;
  };

  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// function () -> () {}

// TODO(aappleby): Support arbitrary lists of tuples, blocks, and bare words
// as function arguments.

int Parser::parseFunction() {
  assert(lex[0].type == TT_KEYWORD && lex[0].value == KW_FUNCTION);
  lex++;

  int error = parseTuple();
  if (error) return error;

  assert(lex[0].type == TT_OPERATOR && lex[0].value == OP_RARROW);
  error = parseTuple();
  if (error) return error;

  error = parseBlock();
  if (error) return error;

  // Args, result, and block are on the stack.
  // Create a new function to hold them and push it on the stack.
  assert(false);
  
  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// function(), literal, (tuple), call(), expression

int Parser::evalExpression() {
  Token base = lex[0];
  Token op = lex[1];

  if (op.isDelimiter(DL_SEMICOLON)) {
    // either "literal;" or "identifier;"
    return evalLiteral();
  }

  if (base.isKeyword(KW_FUNCTION)) {
    return parseFunction();
  }

  if (base.isDelimiter(DL_LPAREN)) {
    // Tuple
    return parseTuple();
  }

  if (base.isDelimiter(DL_LBRACE)) {
    return parseBlock();
  }

  switch(base.type) {
    case TT_EOF:
      // Something's broken.
      assert(false);
      break;
    case TT_INVALID:
      // Something's broken.
      assert(false);
      break;

    case TT_OPERATOR:
      // Unary operator
      assert(false);
      break;
    case TT_DELIMITER:
      // Tuple, array literal, block literal.
      assert(false);
      break;

    case TT_NATIVE:
      // Static cast to native type.
      assert(false);
      break;

    case TT_IDENTIFIER:
      // Function call or copy from variable.
      assert(false);

    case TT_INTEGER:
      // Expression starting with integer.
      assert(false);

    case TT_FLOAT:
      assert(false);
    case TT_RUNE:
      assert(false);
    case TT_STRING:
      assert(false);
    case TT_DIRECTIVE:
      assert(false);
    case TT_COMMENT:
      assert(false);
  }

  assert(false);
  return PARSE_ERROR;
}

//-----------------------------------------------------------------------------
// "<identifier> := <expression>"
// "<identifier> := <expression>"
// "<identifier> := <expression>"
// "<identifier> : <type> = <expression>"

int Parser::parseDecl() {
  Token& name = lex[0];
  assert(name.isIdentifier());
  assert(resolve(name.text).isNil());
  int slot = allocateVariable(name.text);
  stack[slot].name_ = name.text;

  if (lex[1].isOperator(OP_DECLARE))
  {
    evalExpression();
    stack[slot].type_ = stack.back().type_;
    emit(OC_MOVV, slot, stack.size() - 1);
    stack.pop_back();
    return PARSE_OK;
  }
  else if (lex[1].isOperator(OP_COLON))
  {
    // Resolve the type expression.
    Atom type = resolve(lex[2].text);
    assert(!type.isNil());
    stack[slot].type_ = type.name_;

    // Skip the equals sign.
    assert(lex[3].isOperator(OP_EQUALS));
    lex += 4;

    // Eval the expression. The expression type result should match the
    // declared type of the atom.
    evalExpression();
    assert(stack.back().type_ == type.name_);

    // TODO(aappleby): If the result isn't a symbol, the value should go
    // into the constant pool.

    // Move the expression value to the slot.
    stack[slot].setValue(stack.back());
    emit(OC_MOVV, slot, stack.size() - 1);
    stack.pop_back();
    emit(OC_POP, 1);
    return PARSE_OK;
  }
  else
  {
    assert(false);
    return PARSE_ERROR;
  }
}

//-----------------------------------------------------------------------------
// "x(...);" or "x(...) {};"

// TODO - add support for non-delimited args

int Parser::parseCall() {
  Token& name = lex[0];
  Token& lparen = lex[1];

  assert(name.type == TT_IDENTIFIER);
  assert(lparen.type == TT_DELIMITER && lparen.value == DL_LPAREN);

  Atom f = resolve(name.text);
  if (f.isNil()) {
    // Couldn't resolve function call name.
    return -1;
  }

  return 0;
}

//-----------------------------------------------------------------------------
// "x = 10", "x := 10", "x()"

int Parser::parseCallOrDecl() {
  Token name = lex[0];
  Token op = lex[1];

  assert(name.isIdentifier());
  assert(op.isOperator() || op.isDelimiter());

  if (op.isDelimiter()) {
    assert(op.isDelimiter(DL_LPAREN));
    // Function call.
    return parseCall();
  }

  if (op.isOperator(OP_DECLARE) | op.isOperator(OP_COLON)) {
    return parseDecl();
  }


  return 0;
}

//-----------------------------------------------------------------------------

int Parser::parseStatement() {
  Token t = lex[0];
  if (t.isKeyword()) {
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
  } else if (t.isIdentifier()) {
    return parseCallOrDecl();
  }
  else {
    return PARSE_BADSEQUENCE;
  }
}

//-----------------------------------------------------------------------------

int Parser::parseStatementList() {
  // Keep parsing statements until we see a right brace.
  while(lex[0].type != TT_DELIMITER ||
        lex[0].value != DL_RBRACE) {
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
  int stackStart = localTop;
  openBlock();
  parseBlock();
  closeBlock();
  int resultreg = localTop - 1;
  skipExpected(TT_DELIMITER, DL_RPAREN);
  localTop = stackStart;
  return resultreg;
}

//-----------------------------------------------------------------------------

int Parser::parseWhile() {
  // skip 'while' token
  lex++;

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

int Parser::parse() {
  while(!lex.isEOF()) {
    parseStatement();
  }
  return PARSE_OK;
}

//-----------------------------------------------------------------------------

void Parser::dump() {
  currentFunction->dump();
}

/*
void Parser::dumpNode(ParseNode* node, int depth) {
  ParseNode* cursor = node;

  while(cursor) {
    cursor->token.dump();
    cursor = cursor->next;
  }
}
*/



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
