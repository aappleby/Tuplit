#pragma once
#include "Base.h"

// identifiers, keywords, operators and delimiters, and literals

enum TokenType {
  // Control tokens

  TT_EOF     = 0,
  TT_INVALID = -1,  // token value = buffer offset of error

  // Types

  TT_KEYWORD    = 10000,
  TT_OPERATOR   = 20000,
  TT_DELIMITER  = 30000,
  TT_NATIVE     = 40000,  // Native types, (u)int(8/16/32/64), float(32/64)

  TT_IDENTIFIER = 50000,
  TT_INTEGER    = 60000,
  TT_FLOAT      = 70000,
  TT_RUNE       = 80000,
  TT_STRING     = 90000,

  TT_DIRECTIVE  = 100000,
  TT_COMMENT    = 110000,
};


enum TokenValue {
  TV_INVALID  = -1,

  // Keywords
  KW_FOR        = 10001,
  KW_IN,
  KW_SWITCH,
  KW_CASE,
  KW_DEFAULT,
  KW_DO,
  KW_WHILE,
  KW_CONTINUE,
  KW_IF,
  KW_ELSE,
  KW_BREAK,
  KW_RETURN,
  KW_YIELD,

  KW_ARGS,
  KW_RESULT,
  KW_TRUE,
  KW_FALSE,

  KW_TYPE,
  KW_FUNCTION,
  KW_DEBUGGER,

  KW_NEW,
  KW_DELETE,

  // Monograph operators in ascii order.

  OP_BANG      = '!',
  OP_POUND     = '#',
  OP_DOLLAR    = '$',
  OP_PERCENT   = '%',
  OP_AMPERSAND = '&',

  OP_STAR      = '*',
  OP_PLUS      = '+',
  OP_DASH      = '-',
  OP_PERIOD    = '.',
  OP_FSLASH    = '/',
  OP_COLON     = ':',
  OP_LANGLE    = '<',
  OP_EQUALS    = '=',
  OP_RANGLE    = '>',
  OP_QUESTION  = '?',

  OP_AT        = '@',
  OP_BSLASH    = '\\',
  OP_CARET     = '^',
  OP_UNDER     = '_',
  OP_BACKTICK  = '`',

  OP_PIPE      = '|',
  OP_TILDE     = '~',

  // Monograph aliases.

  OP_ADD       = OP_PLUS,
  OP_SUB       = OP_DASH,
  OP_MUL       = OP_STAR,
  OP_DIV       = OP_FSLASH,
  OP_MOD       = OP_PERCENT,

  OP_ASSIGN    = OP_EQUALS,
  OP_LESS      = OP_LANGLE,
  OP_GREATER   = OP_RANGLE,

  OP_BITAND    = OP_AMPERSAND,
  OP_BITOR     = OP_PIPE,
  OP_BITXOR    = OP_CARET,
  OP_BITNOT    = OP_BANG,

  OP_MERGE     = OP_COLON,

  // Digraphs

  OP_INC       = 22001,  // ++
  OP_DEC       = 22002,  // --

  OP_ADDEQ     = 22003,  // +=
  OP_SUBEQ     = 22004,  // -=
  OP_MULEQ     = 22005,  // *=
  OP_DIVEQ     = 22006,  // /=
  OP_MODEQ     = 22007,  // %=

  OP_LESSEQ    = 22008,  // <=
  OP_GREATEREQ = 22009,  // >=

  OP_CONCAT    = 22010,  // ..

  OP_LOGEQ     = 22011,  // ==
  OP_LOGAND    = 22012,  // &&
  OP_LOGOR     = 22013,  // ||
  OP_LOGXOR    = 22014,  // ^^
  OP_LOGNOT    = 22015,  // !!

  OP_BITEQ     = 22016,  // ==
  OP_BITANDEQ  = 22017,  // &=
  OP_BITOREQ   = 22018,  // |=
  OP_BITXOREQ  = 22019,  // ^=

  OP_LARROW    = 22020,  // <-
  OP_RARROW    = 22021,  // ->
  OP_LSHIFT    = 22022,  // <<
  OP_RSHIFT    = 22023,  // >>

  OP_SCOPE     = 22024,  // ::
  OP_DECLARE   = 22025,  // :=

  // N-graphs

  OP_LOGANDEQ  = 23001,  // &&=
  OP_LOGOREQ   = 23002,  // ||=
  OP_LOGXOREQ  = 23003,  // ^^=

  OP_LSHIFTEQ  = 23004,  // <<=
  OP_RSHIFTEQ  = 23005,  // >>=
  OP_LROT      = 23006,  // <<<
  OP_RROT      = 23007,  // >>>

  OP_SAME      = 23008,  // ===
  OP_NOTSAME   = 23009,  // !==

  OP_ELLIPSIS  = 23010,  // ...

  OP_LROTEQ    = 24001,  // <<<=
  OP_RROTEQ    = 24002,  // >>>=

  // Delimiters

  DL_LPAREN    = '(',
  DL_RPAREN    = ')',
  DL_LBRACE    = '{',
  DL_RBRACE    = '}',
  DL_LSQUARE   = '[',
  DL_RSQUARE   = ']',
  DL_COMMA     = ',',
  DL_SEMICOLON = ';',

  // Declared types

  DT_INT8      = 40001,
  DT_INT16,
  DT_INT32,
  DT_INT64,

  DT_UINT8,
  DT_UINT16,
  DT_UINT32,
  DT_UINT64,

  DT_FLOAT32,
  DT_FLOAT64,

  DT_RUNE,
  DT_STRING,

  DT_STRUCT,
  DT_CLASS,
};


struct TokenTable {
  TokenValue value;
  const char* text;
};


extern const TokenTable keywordTable[];
extern const int keywordCount;

extern const TokenTable nativeTable[];
extern const int nativeCount;

extern const TokenTable operatorTable[];
extern const int operatorCount;

extern const TokenTable delimiterTable[];
extern const int delimiterCount;

struct Token {
  Token(TokenType type_, TokenValue value_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { value = value_; while(start_ != end_) { text.push_back(*start_++); }; }
  Token(TokenType type_, uint64_t value_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { u64 = value_; while(start_ != end_) { text.push_back(*start_++); };  }
  Token(TokenType type_, double value_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { f64 = value_; while(start_ != end_) { text.push_back(*start_++); };  }
  Token(TokenType type_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { u64 = 0; while(start_ != end_) { text.push_back(*start_++); };  }

  void dump();

  int line;
  TokenType type;

  union {
    TokenValue value;
    uint64_t   u64;  // Unsigned 64-bit int.
    int64_t    s64;  // Signed 64-bit int.
    double     f64;  // 64-bit float.
  };

  bool operator == (TokenValue value2) const {
    return value == value2;
  }

  bool operator != (TokenValue value2) const {
    return value != value2;
  }

  bool operator < (TokenValue value2) const {
    return value < value2;
  }

  //----------

  bool isKeyword(TokenValue which = TV_INVALID) {
    if (type != TT_KEYWORD) return false;
    if (which > 0 && value != which) return false;
    return true;
  }

  bool isOperator(TokenValue which = TV_INVALID) {
    if (type != TT_OPERATOR) return false;
    if (which > 0 && value != which) return false;
    return true;
  }

  bool isDelimiter(TokenValue which = TV_INVALID) {
    if (type != TT_DELIMITER) return false;
    if (which > 0 && value != which) return false;
    return true;
  }
  
  // Returns true if this is a delimiter that can terminate an expression.
  bool isTerminator() {
    if (type != TT_DELIMITER) return false;
    if (value == DL_COMMA) return true;
    if (value == DL_RBRACE) return true;
    if (value == DL_RSQUARE) return true;
    if (value == DL_RPAREN) return true;
    if (value == DL_SEMICOLON) return true;
    return false;
  }

  bool isIdentifier() {
    return type == TT_IDENTIFIER;
  }

  bool isLiteral() {
    if (type == TT_INTEGER) return true;
    if (type == TT_FLOAT) return true;
    if (type == TT_RUNE) return true;
    if (type == TT_STRING) return true;
    return false;
  }

  bool isInteger() { return type == TT_INTEGER; }
  bool isFloat()   { return type == TT_FLOAT; }
  bool isRune()    { return type == TT_RUNE; }
  bool isString()  { return type == TT_STRING; }

  bool isDirective() {
    return type == TT_DIRECTIVE;
  }

  bool isComment() {
    return type == TT_COMMENT;
  }

  //----------

  const char* start;
  const char* end;
  string text;
};
