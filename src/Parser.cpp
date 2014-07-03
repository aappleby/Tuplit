#include "Parser.h"
#include "Function.h"

Parser::Parser() {
  stack.reserve(65536);
  stackBase = 0;
  localTop = 0;

  currentFunction = new Function();

  // Add native types to the global namespace.

  globals["float32"] = Atom::make_type("float32");
  globals["float64"] = Atom::make_type("float64");
  globals["int32"] = Atom::make_type("int32");
  globals["int64"] = Atom::make_type("int64");
  globals["uint32"] = Atom::make_type("uint32");
  globals["uint64"] = Atom::make_type("uint64");
  globals["string"] = Atom::make_type("string");
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

ParseStatus Parser::emit(Opcode op, int regA, int regB) {
  Instruction o = { op, regA, regB };
  printf("%-9s %3d,%3d,%3d\n",opcodeStrings[o.opcode], o.ra, o.rb, o.rc);
  currentFunction->code.push_back(o);
  return PARSE_OK;
}

int Parser::addConstant(Atom const& c) {
  Tuple& constants = currentFunction->constants;

  // TODO(aappleby): Constant deduping.
  /*
  for (int i = 0; i < (int)constants.size(); i++) {
    if (constants[i] == c) {
      return i;
    }
  }
  */

  int slot = currentFunction->constants.size();
  currentFunction->constants.push_back(c);
  return slot;
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
ParseStatus Parser::parseLhsAtom() {
  return PARSE_OK;
}

//-----------------------------------------------------------------------------

ParseStatus Parser::parseAtom() {
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
    stack.push(temp);
    return PARSE_OK;
  }

  if (t.isOperator(OP_DECLARE)) {
    // (a := c)
    parseExpression();
    temp.setValue(stack.back());
    stack.pop();
    return PARSE_OK;
  }

  if (t.isOperator(OP_EQUALS)) {
    assert(!hasName);
    assert(hasType);
    // (b = c)
    parseExpression();
    temp.setValue(stack.pop());
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
    temp.setValue(stack.pop());
    return PARSE_OK;
  }

  assert(false);
  return PARSE_ERROR;
}

//-----------------------------------------------------------------------------
// ( <atom>, ... )

ParseStatus Parser::parseTuple() {
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

ParseStatus Parser::parseLhs() {
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

ParseStatus Parser::parseExpression() {
  Token t = lex[0];

  if (t.isLiteral()) {
    // TODO(aappleby): handle expression
    switch (t.type) {
      case TT_INTEGER:
        stack.push(Atom::value(t.s64));
        lex++;
        return PARSE_OK;
      case TT_FLOAT:
        stack.push(Atom::value(t.f64));
        lex++;
        return PARSE_OK;
      case TT_STRING:
        stack.push(Atom::value(t.text));
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
  stack.push(Atom::name(name));
  return localTop++;
}

//-----------------------------------------------------------------------------
// <literal_expression>

ParseStatus Parser::evalLiteral() {
  Token base = lex[0];

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
      stack.push(Atom::value(base.s64));
      break;
    case TT_FLOAT:
      stack.push(Atom::value(base.f64));
      break;
    case TT_RUNE:
    case TT_STRING:
      stack.push(Atom::value(base.text));
      break;

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

  // Literal is on the stack. Emit the code that puts it there.
  Atom& value = stack.back();
  int cslot = addConstant(value);
  emit(OC_LOADC, stack.size() - 1, cslot);

  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// function () -> () {}

// TODO(aappleby): Support arbitrary lists of tuples, blocks, and bare words
// as function arguments.

ParseStatus Parser::parseFunction() {
  assert(lex[0].type == TT_KEYWORD && lex[0].value == KW_FUNCTION);
  lex++;

  ParseStatus error = parseTuple();
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
// Process the ops on the stack and emit code to evaluate the postfix'd
// expression.

ParseStatus Parser::emitEval() {
  while(op_stack.size()) {
    popOperator();
  }
  return PARSE_OK;
}

int getPriority(Token& op) {
  //if (op == DL_LPAREN) return 10000;
  //if (op == DL_RPAREN) return 10000;

  static TokenValue priorities[] = {
    OP_SUB, OP_ADD, OP_MOD, OP_DIV, OP_MUL
  };
  for (int i = 0; i < sizeof(priorities) / sizeof(priorities[0]); i++) {
    if (op == priorities[i]) return i;
  }
  return -1;
}

ParseStatus Parser::pushOperator(Token& t) {
  if (t.isDelimiter()) {
    assert((t == DL_LPAREN) || (t == DL_RPAREN));
    if (t == DL_LPAREN) {
      op_stack.push(t);
    } else {
      // Apply all pending operators until we hit the matching paren.
      while(op_stack.back() != DL_LPAREN) {
        popOperator();
      }
      // Remove the matching paren from the operator stack.
      op_stack.pop();
    }
    return PARSE_OK;
  }

  // check priority
  while(!op_stack.empty() &&
        (getPriority(op_stack.back()) >= getPriority(t))) {
    popOperator();
  }

  op_stack.push(t);

  return PARSE_OK;
}

ParseStatus Parser::pushSymbol(Token& t) {
  if (t.isLiteral()) {
    return evalLiteral();
  } else {
    assert(false);
  }
  // resolve symbol
  // push value on stack
  return PARSE_OK;
}

// Apply the operator on the top of the stack
ParseStatus Parser::popOperator() {
  Token op = op_stack.pop();

  Atom& a = stack[-2];
  Atom& b = stack[-1];

  if (op == OP_ADD) {
    emit(OC_ADD, -2, -1);
  } else if (op == OP_SUB) {
    emit(OC_SUB, -2, -1);
  } else if (op == OP_MUL) {
    emit(OC_MUL, -2, -1);
  } else if (op == OP_DIV) {
    emit(OC_DIV, -2, -1);
  } else {
    assert(false);
  }
  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// function(), literal;, (tuple), call(), expression

ParseStatus Parser::evalExpression() {
  while (!lex[0].isDelimiter(DL_SEMICOLON)) {
    Token t = lex[0];

    if (t.isLiteral()) {
      ParseStatus error = pushSymbol(t);
      if (error) return error;
    } else if (t.isOperator() || t.isDelimiter()) {
      ParseStatus error = pushOperator(t);
      if (error) return error;
    }
    lex++;
    continue;
  }
  // Skip the semicolon.
  lex++;

  return emitEval();

  /*
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
  */
}

//-----------------------------------------------------------------------------
// "<identifier> := <expression>"
// "<identifier> : <type> = <expression>"

ParseStatus Parser::parseDecl() {
  Token& name = lex[0];
  Token& op1 = lex[1];
  lex += 2;

  assert(name.isIdentifier());
  assert(resolve(name.text).isNil());
  int slot = allocateVariable(name.text);
  Atom& local = stack[slot];
  local.name_ = name.text;

  // Resolve the type expression if present.
  // TODO(aappleby): Allow complex type expressions.
  if (op1.isOperator(OP_COLON))
  {
    Atom type = resolve(lex[0].text);
    assert(!type.isNil());
    local.type_ = type.name_;
    lex++;
    assert(lex[0].isOperator(OP_EQUALS));
    lex++;
  }

  // Evaluate the right hand side.
  evalExpression();
  Atom& value = stack.back();

  // Local type is inferred from the expression.
  if (op1.isOperator(OP_DECLARE)) {
    local.type_ = value.type_;
  } else if (local.type_ != value.type_) {
      // Attempt to convert the item to the given type if necessary.
    if (local.type_ == "int32" && value.type_ == "int64") {
      value.type_ = "int32";
    } else if (local.type_ == "float32" && value.type_ == "float64") {
      value.type_ = "float32";
    } else {
      assert(false);
    }
  }

  if (value.isSymbol()) {
    // Move the expression value to the slot.
    local.setValue(value);
    emit(OC_MOVV, slot, stack.size() - 1);
    emit(OC_POP, 1);
  } else {
    value.setName(name.text);
    int cslot = addConstant(value);
    emit(OC_LOADC, slot, cslot);
  }

  stack.pop();
  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// "x(...);" or "x(...) {};"

// TODO - add support for non-delimited args

ParseStatus Parser::parseCall() {
  Token& name = lex[0];
  Token& lparen = lex[1];

  assert(name.type == TT_IDENTIFIER);
  assert(lparen.type == TT_DELIMITER && lparen.value == DL_LPAREN);

  Atom f = resolve(name.text);
  if (f.isNil()) {
    // Couldn't resolve function call name.
    return PARSE_ERROR;
  }

  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// "x = 10", "x := 10", "x()"

ParseStatus Parser::parseCallOrDecl() {
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

  return PARSE_OK;
}

//-----------------------------------------------------------------------------

ParseStatus Parser::parseStatement() {
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

ParseStatus Parser::parseStatementList() {
  // Keep parsing statements until we see a right brace.
  while(lex[0].type != TT_DELIMITER ||
        lex[0].value != DL_RBRACE) {
    ParseStatus error = parseStatement();
    if (error) return error;
  }

  return PARSE_OK;
}

//-----------------------------------------------------------------------------

ParseStatus Parser::openBlock() {
  return PARSE_OK;
}

ParseStatus Parser::parseBlock() {
  // open new scope
  // parse statement list
  return PARSE_OK;
}

ParseStatus Parser::closeBlock() {
  return PARSE_OK;
}

//-----------------------------------------------------------------------------
// (condition expression)
// emits code, returns result register

ParseStatus Parser::parseConditional() {
  /*
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
  */
  return PARSE_OK;
}

//-----------------------------------------------------------------------------

ParseStatus Parser::parseWhile() {
  /*
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
  */

  return PARSE_OK;
};

//-----------------------------------------------------------------------------

ParseStatus Parser::parse() {
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
