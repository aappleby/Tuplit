#pragma once
#include "stdint.h"

// identifiers, keywords, operators and delimiters, and literals

enum TokenType {
  // Control tokens

  TT_EOF     = 0,
  TT_INVALID = -1,  // token value = buffer offset of error

  // Types

  TT_COMMENT    = 10001,
  TT_IDENTIFIER = 10002,
  TT_KEYWORD    = 10003,
  TT_OPERATOR   = 10004,
  TT_DELIMITER  = 10005,
  TT_INTEGER    = 10006,
  TT_FLOAT      = 10007,
  TT_RUNE       = 10008,
  TT_STRING     = 10009,
  TT_DIRECTIVE  = 10010,

  // Keywords

  TT_FOR,
  TT_IN,
  TT_SWITCH,
  TT_CASE,
  TT_DEFAULT,
  TT_DO,
  TT_WHILE,
  TT_CONTINUE,
  TT_IF,
  TT_ELSE,
  TT_BREAK,
  TT_RETURN,
  TT_YIELD,

  TT_ARGS,
  TT_RESULT,
  TT_TRUE,
  TT_FALSE,

  TT_TYPE,
  TT_FUNCTION,
  TT_DEBUGGER,

  TT_NEW,
  TT_DELETE,

  // Monograph operators in ascii order.

  TT_BANG      = '!',
  TT_POUND     = '#',
  TT_DOLLAR    = '$',
  TT_PERCENT   = '%',
  TT_AMPERSAND = '&',

  TT_STAR      = '*',
  TT_PLUS      = '+',
  TT_DASH      = '-',
  TT_PERIOD    = '.',
  TT_FSLASH    = '/',
  TT_COLON     = ':',
  TT_LANGLE    = '<',
  TT_EQUALS    = '=',   
  TT_RANGLE    = '>',
  TT_QUESTION  = '?',

  TT_AT        = '@',
  TT_BSLASH    = '\\',
  TT_CARET     = '^',
  TT_UNDER     = '_',
  TT_BACKTICK  = '`',

  TT_PIPE      = '|',
  TT_TILDE     = '~',

  // Monograph aliases.

  TT_ADD       = TT_PLUS,
  TT_SUB       = TT_DASH,
  TT_MUL       = TT_STAR,
  TT_DIV       = TT_FSLASH,
  TT_MOD       = TT_PERCENT,

  TT_ASSIGN    = TT_EQUALS,
  TT_LESS      = TT_LANGLE,
  TT_GREATER   = TT_RANGLE,

  TT_BITAND    = TT_AMPERSAND,
  TT_BITOR     = TT_PIPE,
  TT_BITXOR    = TT_CARET,
  TT_BITNOT    = TT_BANG,

  TT_MERGE     = TT_COLON,

  // Digraphs

  TT_INC       = 2001,  // ++
  TT_DEC       = 2002,  // --

  TT_ADDEQ     = 2003,  // +=
  TT_SUBEQ     = 2004,  // -=
  TT_MULEQ     = 2005,  // *=
  TT_DIVEQ     = 2006,  // /=
  TT_MODEQ     = 2007,  // %=

  TT_LESSEQ    = 2008,  // <=
  TT_GREATEREQ = 2009,  // >=

  TT_CONCAT    = 2010,  // ..

  TT_LOGEQ     = 2011,  // ==
  TT_LOGAND    = 2012,  // &&
  TT_LOGOR     = 2013,  // ||
  TT_LOGXOR    = 2014,  // ^^
  TT_LOGNOT    = 2015,  // !!

  TT_BITEQ     = 2016,  // ==
  TT_BITANDEQ  = 2017,  // &=
  TT_BITOREQ   = 2018,  // |=
  TT_BITXOREQ  = 2019,  // ^=

  TT_LARROW    = 2020,  // <-
  TT_RARROW    = 2021,  // ->
  TT_LSHIFT    = 2022,  // <<
  TT_RSHIFT    = 2023,  // >>

  TT_SCOPE     = 2024,  // ::
  TT_DECLARE   = 2025,  // :=

  // N-graphs

  TT_LOGANDEQ  = 3001,  // &&=
  TT_LOGOREQ   = 3002,  // ||=
  TT_LOGXOREQ  = 3003,  // ^^=

  TT_LSHIFTEQ  = 3004,  // <<=
  TT_RSHIFTEQ  = 3005,  // >>=
  TT_LROT      = 3006,  // <<<
  TT_RROT      = 3007,  // >>>

  TT_SAME      = 3008,  // ===
  TT_NOTSAME   = 3009,  // !==

  TT_ELLIPSIS  = 3010,  // ...

  TT_LROTEQ    = 4001,  // <<<=
  TT_RROTEQ    = 4002,  // >>>=

  // Delimiters

  TT_LPAREN    = '(',
  TT_RPAREN    = ')',
  TT_LBRACE    = '{',
  TT_RBRACE    = '}',
  TT_LSQUARE   = '[',
  TT_RSQUARE   = ']',
  TT_COMMA     = ',',
  TT_SEMICOLON = ';',
};

struct Token {
  Token(int type_, int value_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { s64 = value_; }
  Token(int type_, uint64_t value_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { u64 = value_; }
  Token(int type_, double value_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { f64 = value_; }
  Token(int type_, const char* start_, const char* end_)
    : type(type_), start(start_), end(end_) { u64 = 0; }

  int line;
  int type;

  union {
    uint64_t u64;  // Unsigned 64-bit int.
    int64_t  s64;  // Signed 64-bit int.
    double   f64;  // 64-bit float.
    int symbol;
  };

  const char* start;
  const char* end;
};
