#include "Parser.h"

/*
eight := 8;

DECLARE
  ATOM "eight" : ? = ?
  ATOM ? : ? = 8
*/

int Parser::parseLhs() {
  Token a = lex->next();
  if (a.type != TT_IDENTIFIER) {
    printf("X!");
    return PARSE_OK;
  }

  Token b = lex->peek();
  /*
  switch (b.type()) {

  }
  */
  return PARSE_OK;
}

int Parser::parseExpression() {
  Token lhs = lex->next();

  if (lhs.type != TT_IDENTIFIER) {
    printf("X!");
    return PARSE_OK;
  }

  Token op = lex->next();

  if (op.type == TT_OPERATOR) {
    if (op.symbol == TT_OPERATOR) {
      printf("typed ");
    }

    if (op.symbol == TT_EQUALS) {
      printf("assign ");
    }
  }

  return PARSE_OK;
}

int Parser::parseStatement() {
  return PARSE_OK;
}

int Parser::parse(Lexer* lex_) {
  lex = lex_;
  while(lex->peek().type != TT_EOF) {
    parseExpression();
    expressions.push_back(stack.back());
    stack.pop_back();
  }
  return PARSE_OK;
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
