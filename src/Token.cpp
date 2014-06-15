#include "Token.h"

#include <stdio.h>
#include <string>

//-----------------------------------------------------------------------------
// All multi-character operators in reverse order by length

const TokenTable keywordTable[] = {
  {KW_FOR,           "for"},
  {KW_IN,             "in"},
  {KW_SWITCH,     "switch"},
  {KW_CASE,         "case"},
  {KW_DEFAULT,   "default"},
  {KW_DO,             "do"},
  {KW_WHILE,       "while"},
  {KW_CONTINUE, "continue"},
  {KW_IF,             "if"},
  {KW_ELSE,         "else"},
  {KW_BREAK,       "break"},
  {KW_RETURN,     "return"},
  {KW_YIELD,       "yield"},

  {KW_ARGS,         "args"},
  {KW_RESULT,     "result"},
  {KW_TRUE,         "true"},
  {KW_FALSE,       "false"},

  {KW_TYPE,         "type"},
  {KW_FUNCTION, "function"},
  {KW_DEBUGGER, "debugger"},

  {KW_NEW,           "new"},
  {KW_DELETE,     "delete"},
};
const int keywordCount = sizeof(keywordTable) / sizeof(keywordTable[0]);

const TokenTable nativeTable[] = {
  {DT_INT8,       "int8"},
  {DT_INT16,     "int16"},
  {DT_INT32,     "int32"},
  {DT_INT64,     "int64"},
  {DT_UINT8,     "uint8"},
  {DT_UINT16,   "uint16"},
  {DT_UINT32,   "uint32"},
  {DT_UINT64,   "uint64"},
  {DT_FLOAT32, "float32"},
  {DT_FLOAT64, "float64"},
  {DT_RUNE,       "rune"},
  {DT_STRING,   "string"},
  {DT_STRUCT,   "struct"},
  {DT_CLASS,     "class"},
};
const int nativeCount = sizeof(nativeTable) / sizeof(nativeTable[0]);

const TokenTable operatorTable[] = {
  {OP_LROTEQ,  "<<<="}, {OP_RROTEQ,  ">>>="},
   
  {OP_LOGANDEQ, "&&="}, {OP_LOGOREQ,  "||="}, {OP_LOGXOREQ, "^^="}, {OP_LSHIFTEQ, "<<="}, 
  {OP_RSHIFTEQ, ">>="}, {OP_LROT,     "<<<"}, {OP_RROT,     ">>>"}, {OP_SAME,     "==="}, 
  {OP_NOTSAME,  "!=="}, {OP_ELLIPSIS, "..."},

  {OP_INC,       "++"}, {OP_DEC,       "--"}, {OP_ADDEQ,     "+="}, {OP_SUBEQ,     "-="},  
  {OP_MULEQ,     "*="}, {OP_DIVEQ,     "/="}, {OP_MODEQ,     "%="}, {OP_LESSEQ,    "<="},  
  {OP_GREATEREQ, ">="}, {OP_CONCAT,    ".."}, {OP_LOGEQ,     "=="}, {OP_LOGAND,    "&&"},  
  {OP_LOGOR,     "||"}, {OP_LOGXOR,    "^^"}, {OP_LOGNOT,    "!!"}, {OP_BITEQ,     "=="},  
  {OP_BITANDEQ,  "&="}, {OP_BITOREQ,   "|="}, {OP_BITXOREQ,  "^="}, {OP_LARROW,    "<-"},  
  {OP_RARROW,    "->"}, {OP_LSHIFT,    "<<"}, {OP_RSHIFT,    ">>"}, {OP_SCOPE,     "::"},  
  {OP_DECLARE,   ":="},
  
  {OP_BANG,       "!"}, {OP_POUND,      "#"}, {OP_DOLLAR,     "$"}, {OP_PERCENT,    "%"},
  {OP_AMPERSAND,  "&"}, {OP_STAR,       "*"}, {OP_PLUS,       "+"}, {OP_DASH,       "-"},
  {OP_PERIOD,     "."}, {OP_FSLASH,     "/"}, {OP_COLON,      ":"}, {OP_LANGLE,     "<"},
  {OP_EQUALS,     "="}, {OP_RANGLE,     ">"}, {OP_QUESTION,   "?"}, {OP_AT,         "@"},
  {OP_CARET,      "^"}, {OP_UNDER,      "_"}, {OP_BACKTICK,   "`"}, {OP_PIPE,       "|"},
  {OP_TILDE,      "~"}, {OP_BSLASH,     "\\"}
};
const int operatorCount = sizeof(operatorTable) / sizeof(operatorTable[0]);

const TokenTable delimiterTable[] = {
  {DL_LPAREN,     "("}, {DL_RPAREN,     ")"},
  {DL_LBRACE,     "{"}, {DL_RBRACE,     "}"},
  {DL_LSQUARE,    "["}, {DL_RSQUARE,    "]"},
  {DL_COMMA,      ","}, {DL_SEMICOLON , ";"},
};
const int delimiterCount = sizeof(delimiterTable) / sizeof(delimiterTable[0]);

int tokenValue(const TokenTable* table, int tokenCount, const char* start, const char* end) {
  for (int i = 0; i < tokenCount; i++) {
    if (strncmp(table[i].text, start, end-start) == 0) {
      return table[i].value;
    }
  }
  return -1;
}

const char* tokenText(const TokenTable * table, int tokenCount, TokenValue token) {
  for (int i = 0; i < tokenCount; i++) {
    if (table[i].value == token) {
      return table[i].text;
    }
  }
  return "<unknown>";
}


void Token::dump() {
  string s(start, end);
  switch(type) {
  case TT_INVALID:
    printf("<Invalid>\n");
    break;
  case TT_DIRECTIVE: {
    printf("Directive  - %s\n", s.c_str());
    break;
  }
  case TT_COMMENT: {
    printf("Comment    - %s\n", s.c_str());
    break;
  }
  case TT_KEYWORD: {
    printf("Keyword    - %s\n", s.c_str());
    break;
  }
  case TT_NATIVE: {
    printf("Native     - %s\n", s.c_str());
    break;
  }
  case TT_IDENTIFIER: {
    printf("Identifier - %s\n", s.c_str());
    break;
  }
  case TT_INTEGER: {
    printf("Integer    - %lld\n", s64);
    break;
  }
  case TT_FLOAT: {
    printf("Float      - %.16g\n", f64);
    break;
  }
  case TT_STRING: {
    printf("String     - %s\n", s.c_str());
    break;
  } 
  case TT_OPERATOR: {
    printf("Operator   - '%s'\n", tokenText(operatorTable, operatorCount, value));
    break;
  }
  case TT_DELIMITER: {
    printf("Delimiter  - '%s'\n", tokenText(delimiterTable, delimiterCount, value));
    break;
  }
  case TT_EOF: {
    printf("EOF\n");
    break;
  }
  default:
    printf("Unknown!\n");
    break;
  }
}