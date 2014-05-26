#include "Type.h"

// The 'Type' type.
static Type sType   = { "Type", 0, 1 };

// Hardcoded tuple types, total hack.
static Type sTuple1 = { "Tuple(1)", 0, 1 };
static Type sTuple2 = { "Tuple(2)", 1, 2 };
static Type sTuple3 = { "Tuple(3)", 1, 3 };
static Type sTuple4 = { "Tuple(4)", 1, 4 };

const Type* Type::type   = &sType;
const Type* Type::tuple1 = &sTuple1;
const Type* Type::tuple2 = &sTuple2;
const Type* Type::tuple3 = &sTuple3;
const Type* Type::tuple4 = &sTuple4;

const Type* TupleType(int length) {
  switch(length) {
    case 1: return Type::tuple1;
    case 2: return Type::tuple2;
    case 3: return Type::tuple3;
    case 4: return Type::tuple4;
    default: return 0;
  }
}

const int TupleLength(Type* t) {
  if (t == Type::tuple1) return 1;
  if (t == Type::tuple2) return 2;
  if (t == Type::tuple3) return 3;
  if (t == Type::tuple4) return 4;
  return 0;
}