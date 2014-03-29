#pragma once
#include "stdint.h"

#pragma warning(disable : 4201)

enum PrimitiveTypes {
  PT_NIL,
  PT_INT,
  PT_UINT,
  PT_FLOAT,
  PT_LIST,
  PT_TUPLE,
  PT_STRING,
  PT_BLOB,
  PT_VOID
};

enum AtomHas {
  AH_NAME = 1,
  AH_TYPE = 2,
  AH_VALUE = 4,
};

class Atom;
struct List;
struct Tuple;

class Atom {
public:
  Atom() : primitiveType(PT_NIL), nameHash(0), typeHash(0), u64(0) {}

  Atom(const Atom& a) : header(a.header), nameHash(a.nameHash), typeHash(a.typeHash), u64(a.u64) {}

  Atom& operator = (const Atom& a) {
    header = a.header;
    nameHash = a.nameHash;
    typeHash = a.typeHash;
    u64 = a.u64;
    return *this;
  }

  explicit Atom(Tuple* v)   : primitiveType(PT_TUPLE), has(AH_VALUE), nameHash(0), typeHash(0), pTuple(v) {}
  explicit Atom(List* v)    : primitiveType(PT_LIST),  has(AH_VALUE), nameHash(0), typeHash(0), pList(v) {}
  explicit Atom(uint8_t* v) : primitiveType(PT_BLOB),  has(AH_VALUE), nameHash(0), typeHash(0), pBlob(v) {}
  explicit Atom(int v)      : primitiveType(PT_INT),   has(AH_VALUE), nameHash(0), typeHash(0), s64(v) {}
  explicit Atom(double v)   : primitiveType(PT_FLOAT), has(AH_VALUE), nameHash(0), typeHash(0), f64(v) {}

  void setInt(int v) {
    s64 = v;
  }
  int getInt() {
    return static_cast<int>(s64);
  }

  Tuple& getTuple() {
    return *pTuple;
  }

private:

  // header
  union {
    uint64_t header;
    struct {
      uint8_t flags;
      uint8_t has;
      uint8_t primitiveType;
      uint8_t stuff1;
      uint8_t stuff2;
      uint8_t stuff3;
      uint8_t stuff4;
      uint8_t somethingRefCount;
    };
  };

  // name hash
  uint64_t nameHash;
  
  // type hash
  uint64_t typeHash;

  // value
  union {
    int64_t  s64;
    uint64_t u64;
    double   f64;
    List*    pList;
    Tuple*   pTuple;
    uint8_t* pBlob;
    void*    pVoid;
  };
};