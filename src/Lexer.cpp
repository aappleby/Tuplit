#include "Lexer.h"

#include "Token.h"

#include <assert.h>
#include <math.h>
#include <string>

using std::string;

// The character sets for binary, octal, decimal, and hexadecimal numeric representations.
const char* binDigits = "01";
const char* octDigits = "01234567";
const char* decDigits = "0123456789";
const char* hexDigits = "0123456789abcdefABCDEF";

// All valid characters that can begin a number.
const char* numberStart = "0123456789";

// All valid characters that can begin an operator.
const char* operatorStart = "=&|^!.+-*/%:?~";

const char* delimiters = "[](){},;";

// TODO(aappleby): Handle constant overflow.
const char* largestU64 = "18446744073709551616";
const char* largestS64 = "9223372036854775807";

//-----------------------------------------------------------------------------
// All multi-character operators in reverse order by length

struct TokenTable {
  int value;
  const char* text;
};


const TokenTable keywordTable[] = {
  {TT_FOR,           "for"},
  {TT_IN,             "in"},
  {TT_SWITCH,     "switch"},
  {TT_CASE,         "case"},
  {TT_DEFAULT,   "default"},
  {TT_DO,             "do"},
  {TT_WHILE,       "while"},
  {TT_CONTINUE, "continue"},
  {TT_IF,             "if"},
  {TT_ELSE,         "else"},
  {TT_BREAK,       "break"},
  {TT_RETURN,     "return"},
  {TT_YIELD,       "yield"},

  {TT_ARGS,         "args"},
  {TT_RESULT,     "result"},
  {TT_TRUE,         "true"},
  {TT_FALSE,       "false"},

  {TT_TYPE,         "type"},
  {TT_FUNCTION, "function"},
  {TT_DEBUGGER, "debugger"},

  {TT_NEW,           "new"},
  {TT_DELETE,     "delete"},
};
const int keywordCount = sizeof(keywordTable) / sizeof(keywordTable[0]);

const TokenTable operatorTable[] = {
  {TT_LROTEQ,  "<<<="}, {TT_RROTEQ,  ">>>="},
   
  {TT_LOGANDEQ, "&&="}, {TT_LOGOREQ,  "||="}, {TT_LOGXOREQ, "^^="}, {TT_LSHIFTEQ, "<<="}, 
  {TT_RSHIFTEQ, ">>="}, {TT_LROT,     "<<<"}, {TT_RROT,     ">>>"}, {TT_SAME,     "==="}, 
  {TT_NOTSAME,  "!=="}, {TT_ELLIPSIS, "..."},

  {TT_INC,       "++"}, {TT_DEC,       "--"}, {TT_ADDEQ,     "+="}, {TT_SUBEQ,     "-="},  
  {TT_MULEQ,     "*="}, {TT_DIVEQ,     "/="}, {TT_MODEQ,     "%="}, {TT_LESSEQ,    "<="},  
  {TT_GREATEREQ, ">="}, {TT_CONCAT,    ".."}, {TT_LOGEQ,     "=="}, {TT_LOGAND,    "&&"},  
  {TT_LOGOR,     "||"}, {TT_LOGXOR,    "^^"}, {TT_LOGNOT,    "!!"}, {TT_BITEQ,     "=="},  
  {TT_BITANDEQ,  "&="}, {TT_BITOREQ,   "|="}, {TT_BITXOREQ,  "^="}, {TT_LARROW,    "<-"},  
  {TT_RARROW,    "->"}, {TT_LSHIFT,    "<<"}, {TT_RSHIFT,    ">>"}, {TT_SCOPE,     "::"},  
  {TT_DECLARE,   ":="},
  
  {TT_BANG,       "!"}, {TT_POUND,      "#"}, {TT_DOLLAR,     "$"}, {TT_PERCENT,    "%"},
  {TT_AMPERSAND,  "&"}, {TT_STAR,       "*"}, {TT_PLUS,       "+"}, {TT_DASH,       "-"},
  {TT_PERIOD,     "."}, {TT_FSLASH,     "/"}, {TT_COLON,      ":"}, {TT_LANGLE,     "<"},
  {TT_EQUALS,     "="}, {TT_RANGLE,     ">"}, {TT_QUESTION,   "?"}, {TT_AT,         "@"},
  {TT_CARET,      "^"}, {TT_UNDER,      "_"}, {TT_BACKTICK,   "`"}, {TT_PIPE,       "|"},
  {TT_TILDE,      "~"}, {TT_BSLASH,     "\\"}
};
const int operatorCount = sizeof(operatorTable) / sizeof(operatorTable[0]);

const TokenTable delimiterTable[] = {
  {TT_LPAREN,     "("}, {TT_RPAREN,     ")"},
  {TT_LBRACE,     "{"}, {TT_RBRACE,     "}"},
  {TT_LSQUARE,    "["}, {TT_RSQUARE,    "]"},
  {TT_COMMA,      ","}, {TT_SEMICOLON , ";"},
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

const char* tokenText(const TokenTable * table, int tokenCount, int64_t token) {
  for (int i = 0; i < tokenCount; i++) {
    if (table[i].value == token) {
      return table[i].text;
    }
  }
  return "<unknown>";
}

int Lexer::lexOperator() {
  for (int i = 0; i < operatorCount; i++) {
    size_t len = strlen(operatorTable[i].text);
    if (strncmp(operatorTable[i].text, cursor, len) == 0) {
      addToken(Token(TT_OPERATOR, operatorTable[i].value, cursor, cursor + len));
      cursor += len;
      return LEX_OK;
    }
  }
  return LEX_ERROR;
}

int Lexer::lexDelimiter() {
  for (int i = 0; i < delimiterCount; i++) {
    size_t len = strlen(delimiterTable[i].text);
    if (strncmp(delimiterTable[i].text, cursor, len) == 0) {
      addToken(Token(TT_DELIMITER, delimiterTable[i].value, cursor, cursor + len));
      cursor += len;
      return LEX_OK;
    }
  }
  return LEX_ERROR;
}

int Lexer::lexKeyword() {
  for (int i = 0; i < keywordCount; i++) {
    size_t len = strlen(keywordTable[i].text);
    if (strncmp(keywordTable[i].text, cursor, len) == 0) {
      addToken(Token(TT_KEYWORD, keywordTable[i].value, cursor, cursor + len));
      cursor += len;
      return LEX_OK;
    }
  }
  return LEX_ERROR;
}

//-----------------------------------------------------------------------------

int match1(const char* v, const char* chars) {
  while (*chars) {
    if (*v == *chars) {
      return 1;
    }
    chars++;
  }
  return 0;
}

//-----------------------------------------------------------------------------

const char* skip1(const char* cursor1, char c) {
  return (*cursor1 == c) ? cursor1 + 1 : cursor1;
}

const char* skip1(const char* cursor, const char* chars) {
  return cursor + match1(cursor, chars);
}

const char* skipN(const char* start, const char* chars) {
  const char* cursor1 = start;
  const char* cursor2 = chars;

  while (*cursor1) {
    if (*cursor1 == *cursor2) {
      cursor1++;
      cursor2 = chars;
    }
    else {
      cursor2++;
      if (*cursor2 == 0) break;
    }
  }
  return cursor1;
}

//-----------------------------------------------------------------------------

int toDigit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'z') return c - 'a' + 10;
  if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
  return 0;
}

//----------

uint64_t toInt(const char* start, const char* end, int base) {
  uint64_t result = 0;
  uint64_t scale = 1;
  while (end != start) {
    end--;
    result += toDigit(*end) * scale;
    scale *= base;
  }
  return result;
}

//----------

double toFloat(const char* start, const char* decimal, const char* end, int base) {
  double result = 0;
  if (*decimal == '.') {
    while (end != start) {
      end--;
      if (*end == '.') break;
      result += toDigit(*end);
      result /= base;
    }
  }

  double scale = 1;
  while (end != start) {
    end--;
    result += toDigit(*end) * scale;
    scale *= base;
  }

  return result;
}

//-----------------------------------------------------------------------------
// [-+]?[0-9]*\.?[0-9]*([eE][-+]?[0-9]+)?

int Lexer::lexNumber() {
  const char* digits = decDigits;
  int base = 10;

  assert(match1(cursor, numberStart) && "Initial character of number was invalid");

  // If the number starts with a '0', the next character defines the base.
  if (*cursor == '0') {
    switch (cursor[1]) {
    case 'b':
    case 'B':
      digits = binDigits;
      base = 2;
      break;
    case 'c':
    case 'C':
      digits = octDigits;
      base = 8;
      break;
    case 'd':
    case 'D':
      digits = decDigits;
      base = 10;
      break;
    case 'h':
    case 'H':
    case 'x':
    case 'X':
      digits = hexDigits;
      base = 16;
      break;
    default:
      // Bare zero.
      addToken(Token(TT_INTEGER, 0, cursor, cursor + 1));
      cursor++;
      return LEX_OK;
    };
    cursor += 2;
  }

  // Scan digit & decimal block.
  const char* numberStart = cursor;
  const char* decimal = skipN(numberStart, digits);
  const char* fractStart = skip1(decimal, '.');
  const char* numberEnd = skipN(fractStart, digits);

  // If the number block contains a decimal point or is followed by the exponent delimiter, it's a floating point constant.
  if ((base != 16 && *numberEnd == 'e') ||
      (base != 16 && *numberEnd == 'E') ||
      (base == 16 && *numberEnd == 'p') ||
      (base == 16 && *numberEnd == 'P')) {
    // Floating point constant with exponent
    const char* expSign = numberEnd + 1;
    const char* expStart = skip1(expSign, "+-");
    const char* expEnd = skipN(expStart, digits);
    assert(expEnd - expStart > 0);
    int exp = static_cast<int>(toInt(expStart, expEnd, base));
    if (*expSign == '-') exp = -exp;

    double value = toFloat(numberStart, decimal, numberEnd, base);
    value *= pow(base, static_cast<double>(exp));
    addToken(Token(TT_FLOAT, value, numberStart, expEnd));
    cursor = expEnd;
    return LEX_OK;
  } else if (*decimal == '.') {
    // Floating point constant without exponent.
    double value = toFloat(numberStart, decimal, numberEnd, base);
    addToken(Token(TT_FLOAT, value, numberStart, numberEnd));
    cursor = numberEnd;
    return LEX_OK;
  } else {
    // Integer constant.
    uint64_t value = toInt(numberStart, numberEnd, base);
    addToken(Token(TT_INTEGER, value, numberStart, numberEnd));
    cursor = numberEnd;
    return LEX_OK;
  }
}

//-----------------------------------------------------------------------------

int Lexer::lexIdentifier() {
  assert(isalpha(*cursor));
  const char* start = cursor;
  const char* end = cursor + 1;
  while(*end && isalnum(*end)) end++;
  addToken(Token(TT_IDENTIFIER, start, end));
  cursor = end;
  return LEX_OK;
}

//-----------------------------------------------------------------------------

int Lexer::lexDirective() {
  assert(*cursor == '#');
  const char* start = cursor + 1;
  const char* end = cursor + 1;
  while(*end && isalnum(*end)) end++;
  addToken(Token(TT_DIRECTIVE, start, end));
  cursor = end;
  return LEX_OK;
}

//-----------------------------------------------------------------------------

int Lexer::lexRawString() {
  // Multi-line comment.

  assert(cursor[0] == '"');
  assert(cursor[1] == '"');
  assert(cursor[2] == '"');
  if (cursor[3] == 0) return LEX_ERROR;

  if (cursor[3] == '\n') {
    const char* start = cursor + 4;
    const char* end = cursor + 4;

    while(*end) {
      if (end[0] == '\n' && end[1] == '"' && end[2] == '"' && end[3] == '"') {
        addToken(Token(TT_STRING, start, end));
        cursor = end + 4;
        return LEX_OK;
      }

      if (*end == '\n') {
        linecount++;
      }

      end++;
    }
  } else {
    const char* start = cursor + 3;
    const char* end = cursor + 3;

    while(*end) {
      if (end[0] == '"' && end[1] == '"' && end[2] == '"') {
        addToken(Token(TT_STRING, start, end));
        cursor = end + 3;
        return LEX_OK;
      }

      if (*end == '\n') {
        linecount++;
      }

      end++;
    }
  }


  return LEX_ERROR;
}

//-----------------------------------------------------------------------------
// Hey, let's micro-optimze our base64 decoder the lulz.

static int db(unsigned char c) {
  c -= 65;
  if (c < 26) return c;
  c -= 32;
  if (c < 26) return c + 26;
  c -= 207;
  if (c < 10) return c + 52;
  if (c == 251) return 62;
  if (c == 255) return 63;
  return -1;
}

void decodeBase64(const char* start, const char* end, std::string& result) {
  result.clear();

  uint32_t accum = 0;
  int collected = 0;

  const char* in = start;
  while(in != end) {
    char c = *in++;
    if (c == '\r' || c == '\n') continue;

    if (c == '=') {
      if (*in == '=') {
        // Found second terminator, last block has one byte.
        result.push_back(char(accum >> 4));
        return;
      } else {
        // No second terminator, last block has two bytes.
        result.push_back(char(accum >> 10));
        result.push_back(char(accum >> 2));
        return;
      }
    }

    int x = db(c);
    if (x >= 0) {
      accum = (accum << 6) | x;
      collected++;
    }

    if (collected == 4) {
      result.push_back(char(accum >> 16));
      result.push_back(char(accum >> 8));
      result.push_back(char(accum >> 0));
      accum = 0;
      collected = 0;
    }
  }
}

//-----------------------------------------------------------------------------

int Lexer::lexBase64String() {
  // Multi-line comment.

  assert(cursor[0] == '\'');
  assert(cursor[1] == '\'');
  assert(cursor[2] == '\'');
  if (cursor[3] == 0) return LEX_ERROR;

  if (cursor[3] == '\n') {
    const char* start = cursor + 4;
    const char* end = cursor + 4;

    while(*end) {
      if (end[0] == '\n' && end[1] == '\'' && end[2] == '\'' && end[3] == '\'') {
        decodeBase64(start, end, scratch);
        char* result = new char[scratch.size()];
        memcpy(result, &scratch[0], scratch.size());
        addToken(Token(TT_STRING, result, result + scratch.size()));
        cursor = end + 4;
        return LEX_OK;
      }

      if (*end == '\n') {
        linecount++;
      }

      scratch.push_back('x');

      end++;
    }
  } else {
    const char* start = cursor + 3;
    const char* end = cursor + 3;

    while(*end) {
      if (end[0] == '\'' && end[1] == '\'' && end[2] == '\'') {
        decodeBase64(start, end, scratch);
        char* result = new char[scratch.size()];
        memcpy(result, &scratch[0], scratch.size());
        addToken(Token(TT_STRING, result, result + scratch.size()));
        cursor = end + 3;
        return LEX_OK;
      }

      if (*end == '\n') {
        linecount++;
      }

      end++;
    }
  }


  return LEX_ERROR;
}

//-----------------------------------------------------------------------------

int Lexer::lexString() {
  cursor++;
  scratch.clear();
  const char delimiter = '"';

  while(cursor < codeEnd && *cursor != delimiter) {
    if (cursor[0] == '\\') {
      switch(cursor[1]) {
        case 'a' : scratch.push_back('\a'); break;
        case 'b' : scratch.push_back('\b'); break;
        case 'f' : scratch.push_back('\f'); break;
        case 'n' : scratch.push_back('\n'); break;
        case 'r' : scratch.push_back('\r'); break;
        case 't' : scratch.push_back('\t'); break;
        case 'v' : scratch.push_back('\v'); break;
        case '\\': scratch.push_back('\\'); break;
        case '0' : scratch.push_back(0);    break;
      }
      if (cursor[1] == delimiter) scratch.push_back(delimiter);
      cursor += 2;
    } else {
      scratch.push_back(*cursor++);
    }
  }
  if (cursor < codeEnd) cursor++;

  size_t len = scratch.size();
  char* result = new char[len];
  memcpy(result, &scratch[0], len);
  addToken(Token(TT_STRING, result, result + len));
  return LEX_OK;
}

//-----------------------------------------------------------------------------

int Lexer::lexRune() {
  return LEX_ERROR;
}

//-----------------------------------------------------------------------------

int Lexer::lexComment() {
  // Multi-line comment.

  assert(cursor[0] == '/');
  assert(cursor[1] == '*');
  const char* start = cursor;
  const char* end = cursor + 2;

  while(*end) {
    if ((end[0] == '*') && (end[1] == '/')) {
      end += 2;
      addToken(Token(TT_COMMENT, start, end));
      cursor = end;
      return LEX_OK;
    }

    if (*end == '\n') {
      linecount++;
    }

    end++;
  }

  return LEX_ERROR;
}

//-----------------------------------------------------------------------------

void Lexer::addToken(Token t) {
  t.line = linecount;
  tokens.push_back(t);
}

//-----------------------------------------------------------------------------

int Lexer::lex(std::string& code) {
  linecount = 1;
  bool linestart = true;
  bool lineIsDirective = false;

  codeStart = code.c_str();
  codeEnd = codeStart + code.size();

  cursor = codeStart;
  while (*cursor != 0) {
    if (strncmp(cursor, "stoplexing", 10) == 0) break;

    if (*cursor == ' ') {
      cursor++;
      continue;
    }

    if (*cursor == '\t') {
      cursor++;
      continue;
    }

    if (*cursor == '\n') {
      cursor++;
      linecount++;
      linestart = true;
      continue;
    }

    if (linestart) {
      linestart = false;
      if (*cursor == '#') {
        if (lexDirective() == LEX_OK) {
          lineIsDirective = true;
          continue;
        }
      }
    }

    if (cursor[0] == '"' && cursor[1] == '"' && cursor[2] == '"') {
      if (lexRawString() == LEX_OK) continue;
    }

    if (cursor[0] == '\'' && cursor[1] == '\'' && cursor[2] == '\'') {
      if (lexBase64String() == LEX_OK) continue;
    }

    if (cursor[0] == '/' && cursor[1] == '/') {
      // Single-line comment
      const char* start = cursor;
      const char* end = cursor;
      while(*end && *end != '\n') end++;
      addToken(Token(TT_COMMENT, start, end));
      cursor = end;
      continue;
    }

    if (cursor[0] == '/' && cursor[1] == '*') {
      if (lexComment() == LEX_OK) {
        continue;
      }
    }

    // TODO(aappleby): Need real string literal parsing with escaped quotes and stuff.
    if (cursor[0] == '"') {
      if (lexString() == LEX_OK) continue;
    }

    // TODO(aappleby): Need real rune literal parsing with escaped quotes and stuff.
    if (cursor[0] == '\'') {
      const char* start = cursor + 1;
      const char* end = strstr(start, "'");

      if (end == NULL) {
        addToken(Token(TT_RUNE, start, codeEnd));
        cursor = codeEnd;
      } else {
        addToken(Token(TT_RUNE, start, end));
        cursor = end + 1;
      }
      continue;
    }

    if ((*cursor >= '0') && (*cursor <= '9')) {
      lexNumber();
      continue;
    }

    // Might be an operator.
    if (lexOperator() == LEX_OK) {
      continue;
    }

    // Might be a delimiter.
    if (lexDelimiter() == LEX_OK) {
      continue;
    }

    // Might be a keyword.
    if (lexKeyword() == LEX_OK) {
      continue;
    }

    // Might be an identifier.
    if ((*cursor >= 'a' && *cursor <= 'z') ||
        (*cursor >= 'A' && *cursor <= 'Z')) {
      if (lexIdentifier() == LEX_OK) {
        continue;
      }
    }

    // No idea what this is so skip it.
    printf("Don't know what '%c' is...", *cursor);
    cursor++;
  }

  addToken(Token(TT_EOF, codeEnd, codeEnd));
  addToken(Token(TT_EOF, codeEnd, codeEnd));
  addToken(Token(TT_EOF, codeEnd, codeEnd));
  addToken(Token(TT_EOF, codeEnd, codeEnd));

  return LEX_OK;
}

//-----------------------------------------------------------------------------

void Lexer::dump() {
  int depth = 0;
  for (size_t i = 0; i < tokens.size(); i++) {
    Token& t = tokens[i];
    printf("%4d: ", t.line);
    string s(t.start, t.end);
    switch(t.type) {
    case TT_INVALID:
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("<Invalid>\n");
      break;
    case TT_DIRECTIVE: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("Directive  - %s\n", s.c_str());
      break;
    }
    case TT_COMMENT: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("Comment    - %s\n", s.c_str());
      break;
    }
    case TT_KEYWORD: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("Keyword    - %s\n", s.c_str());
      break;
    }
    case TT_IDENTIFIER: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("Identifier - %s\n", s.c_str());
      break;
    }
    case TT_INTEGER: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("Int        - %lld\n", t.s64);
      break;
    }
    case TT_FLOAT: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("Float      - %.16g\n", t.f64);
      break;
    }
    case TT_STRING: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("String     - %s\n", s.c_str());
      break;
    } 
    case TT_OPERATOR: {
      for (int j = 0; j < depth; j++) putc(' ', stdout);
      printf("Operator   - '%s'\n", tokenText(operatorTable, operatorCount, t.symbol));
      break;
    }
    case TT_DELIMITER: {
      if (t.symbol == '{' || t.symbol == '(' || t.symbol == '[') {
        for (int j = 0; j < depth; j++) putc(' ', stdout);
        printf("Delimiter  - '%s'\n", tokenText(delimiterTable, delimiterCount, t.symbol));
        depth += 4;
      }
      else if (t.symbol == '}' || t.symbol == ')' || t.symbol == ']') {
        depth -= 4;
        for (int j = 0; j < depth; j++) putc(' ', stdout);
        printf("Delimiter  - '%s'\n", tokenText(delimiterTable, delimiterCount, t.symbol));
      } else {
        for (int j = 0; j < depth; j++) putc(' ', stdout);
        printf("Delimiter  - '%s'\n", tokenText(delimiterTable, delimiterCount, t.symbol));
      }
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
}

//-----------------------------------------------------------------------------