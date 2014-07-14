#include "Lexer.h"

#include "Token.h"

//#include <assert.h>
#include <math.h>
#include <string>

// The character sets for binary, octal, decimal, and hexadecimal numeric representations.
const char* binDigits = "01";
const char* octDigits = "01234567";
const char* decDigits = "0123456789";
const char* hexDigits = "0123456789abcdefABCDEF";

const char* identifierStart = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#";
const char* identifierSet   = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_$#";

// All valid characters that can begin a number.
const char* numberStart = "0123456789";
const char* numberSet   = "0123456789+-eE.";

// All valid whitespace characters
const char* spaceSet = " \t\r\n";

// All valid characters that can begin an operator.
const char* operatorSet   = "<>=&|^!.+-*/%:?";

const char* delimiterSet = "[](){},;";

// TODO(aappleby): Handle constant overflow.
const char* largestU64 = "18446744073709551615";
const char* largestS64 = "9223372036854775807";

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

int matchText(const char * start, const char* end, const char* text) {
  while ((start < end) && *text && (*start == *text)) {
    start++;
    text++;
  }
  return (*text == 0) && (start == end) ? 1 : 0;
}

TokenValue matchTable(const char* start, const char* end, const TokenTable* table, int count) {
  for (int i = 0; i < count; i++) {
    if (matchText(start, end, table[i].text)) {
      return table[i].value;
    }
  }
  return TV_INVALID;
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
// this is not a valid regex, it's just a note.
// [-+]?(0x[a-fA-F0-9]+|[0-9]+)*\.?[0-9]*([eEpP][-+]?[0-9]+)?

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
      addToken(Token(TT_INTEGER, uint64_t(0), cursor, cursor + 1));
      cursor++;
      return LEX_OK;
    };
    cursor += 2;
  }

  // Scan digit & decimal block.
  const char* numberStart2 = cursor;
  const char* decimal = skipN(numberStart2, digits);
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

    double value = toFloat(numberStart2, decimal, numberEnd, base);
    value *= pow(base, static_cast<double>(exp));
    addToken(Token(TT_FLOAT, value, numberStart2, expEnd));
    cursor = expEnd;
    return LEX_OK;
  } else if (*decimal == '.') {
    // Floating point constant without exponent.
    double value = toFloat(numberStart2, decimal, numberEnd, base);
    addToken(Token(TT_FLOAT, value, numberStart2, numberEnd));
    cursor = numberEnd;
    return LEX_OK;
  } else {
    // Integer constant.
    uint64_t value = toInt(numberStart2, numberEnd, base);
    addToken(Token(TT_INTEGER, value, numberStart2, numberEnd));
    cursor = numberEnd;
    return LEX_OK;
  }
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
        lineIsDirective = true;
        const char* start = cursor + 1;
        const char* end = skipN(start, identifierSet);
        addToken(Token(TT_DIRECTIVE, start, end));
        cursor = end;
        continue;
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

    if (match1(cursor, numberStart)) {
      lexNumber();
      continue;
    }

    if (match1(cursor, operatorSet)) {
      const char* start = cursor;
      const char* end = skipN(start, operatorSet);
      TokenValue op = matchTable(start, end, operatorTable, operatorCount);
      addToken(Token(TT_OPERATOR, op, start, end));
      cursor = end;
      continue;
    }

    if (match1(cursor, delimiterSet)) {
      const char* start = cursor;
      const char* end = cursor + 1;
      TokenValue op = matchTable(start, end, delimiterTable, delimiterCount);
      if (op != TV_INVALID) {
        addToken(Token(TT_DELIMITER, op, start, end));
      }
      cursor = end;
      continue;
    }

    if (match1(cursor, identifierStart)) {
      // Keyword, native type, or identifier.
      const char* start = cursor;
      const char* end = skipN(start, identifierSet);

      TokenValue value = TV_INVALID;
      if ((value = matchTable(start, end, keywordTable, keywordCount)) != TV_INVALID) {
        addToken(Token(TT_KEYWORD, value, start, end));
      }
      else if ((value = matchTable(start, end, nativeTable, nativeCount)) != TV_INVALID) {
        addToken(Token(TT_NATIVE, value, start, end));
      }
      else {
        addToken(Token(TT_IDENTIFIER, start, end));
      }

      cursor = end;
      continue;
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

    if (t.value == DL_RBRACE || t.value == DL_RPAREN || t.value == DL_RSQUARE) {
      depth -= 2;
    }

    for (int j = 0; j < depth; j++) putc(' ', stdout);
    t.dump();

    if (t.value == DL_LBRACE || t.value == DL_LPAREN || t.value == DL_LSQUARE) {
      depth += 2;
    }
  }
}

//-----------------------------------------------------------------------------