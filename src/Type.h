#pragma once
#include "Base.h"


class Type {
public:
  const char* name;
  int dimensions;
  int length;
  int width;
  int height;

  static const Type* type;
  static const Type* tuple1;
  static const Type* tuple2;
  static const Type* tuple3;
  static const Type* tuple4;

  static const Type* func;
};

string TupleType(int length);

int TupleLength(const string& t);