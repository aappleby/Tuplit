#include "Type.h"

// The 'Type' type.
static Type sType   = { "Type", 0, 1 };

// Hardcoded tuple types, total hack.
static Type sTuple1 = { "Tuple(1)", 0, 1 };
static Type sTuple2 = { "Tuple(2)", 1, 2 };
static Type sTuple3 = { "Tuple(3)", 1, 3 };
static Type sTuple4 = { "Tuple(4)", 1, 4 };
static Type sFunc   = { "Function", 1, 1 };

const Type* Type::type   = &sType;
const Type* Type::tuple1 = &sTuple1;
const Type* Type::tuple2 = &sTuple2;
const Type* Type::tuple3 = &sTuple3;
const Type* Type::tuple4 = &sTuple4;
const Type* Type::func   = &sFunc;

string TupleType(int length) {
  char buffer[32];
  _itoa_s(length, buffer, 32, 10);
  return string("tuple") + string(buffer);
}

int TupleLength(const string& t) {
  if (t == "tuple1") return 1;
  if (t == "tuple2") return 1;
  if (t == "tuple3") return 1;
  if (t == "tuple4") return 1;
  return 0;
}
