#pragma once

#include "Base.h"
#include "Type.h"

#pragma warning(disable : 4201)

class Atom;
struct List;
struct Tuple;

// The actual machine representation of the data stored in the atom, independent
// of its declared type.
enum PhysicalType {
  PT_NIL = 0,
  PT_INT,
  PT_UINT,
  PT_FLOAT,
  PT_LIST,
  PT_TUPLE,
  PT_TEXT,
  PT_TYPE,
  PT_BLOB,
  PT_VOID
};

union MetaStuff {
  uint64_t bytes;
  struct {
    uint8_t flags;
    uint8_t has;
    uint8_t type;
    uint8_t stuff1;
    uint8_t stuff2;
    uint8_t stuff3;
    uint8_t stuff4;
    uint8_t somethingRefCount;
  };
};

union ValueStuff {
  int64_t  s64;
  uint64_t u64;
  double   f64;

  List*    list;
  Atom*    atom;
  uint8_t* blob;
  string*  text;
  Type*    type;
};

//------------------------------------------------------------------------------

class Atom {
public:
  Atom() {
    meta_.bytes = 0;
    meta_.type = PT_NIL;
    name_ = NULL;
    type_ = NULL;
    value_.u64 = 0;
  }

  Atom (MetaStuff meta,
        string* name,
        const Type* type,
        ValueStuff value) {
    meta_ = meta;
    name_ = name;
    type_ = type;
    value_ = value;
    updateHas();
  }


  Atom (PhysicalType mtype,
        std::string* name,
        const Type* type,
        Atom* value) {
    meta_.bytes = 0;
    meta_.type = mtype;
    name_ = name;
    type_ = type;
    value_.atom = value;
    updateHas();
  }

  static Atom nil;

  Atom(const Atom& a) {
    meta_ = a.meta_;
    name_ = a.name_;
    type_ = a.type_;
    value_ = a.value_;
  }

  Atom& operator = (const Atom& a) {
    assert(this != &nil);
    meta_ = a.meta_;
    name_ = a.name_;
    type_ = a.type_;
    value_ = a.value_;
    return *this;
  }

  bool isNil() const {
    return this == &nil;
  }

  //----------
  // Conversions from Tuplit types

  // An atom pointing at another atom is a tuple of length 1.
  explicit Atom(Atom* v) {
    meta_.bytes = 0;
    meta_.type = PT_TUPLE;
    meta_.has = AH_VALUE;
    name_ = NULL;
    type_ = Type::tuple1;
    value_.atom = v;
  }

  explicit Atom(List* v) {
    meta_.bytes = 0;
    meta_.type = PT_LIST;
    meta_.has = AH_VALUE;
    name_ = NULL;
    type_ = NULL;
    value_.list = v;
  }

  explicit Atom(uint8_t* v) {
    meta_.bytes = 0;
    meta_.type = PT_BLOB;
    meta_.has = AH_VALUE;
    name_ = NULL;
    type_ = NULL;
    value_.blob = v;
  }

  explicit Atom(std::string* v) {
    meta_.bytes = 0;
    meta_.type = PT_TEXT;
    meta_.has = AH_VALUE;
    name_ = NULL;
    type_ = NULL;
    value_.text = v;
  }

  //-----------
  // Conversions from base types

  explicit Atom(int32_t v) {
    meta_.bytes = 0;
    meta_.type = PT_INT;
    meta_.has = AH_VALUE;
    name_ = NULL;
    type_ = NULL;
    value_.s64 = v;
  }

  explicit Atom(int64_t v) {
    meta_.bytes = 0;
    meta_.type = PT_INT;
    meta_.has = AH_VALUE;
    name_ = NULL;
    type_ = NULL;
    value_.s64 = v;
  }

  explicit Atom(double v) {
    meta_.bytes = 0;
    meta_.type = PT_FLOAT;
    meta_.has = AH_VALUE;
    name_ = NULL;
    type_ = NULL;
    value_.f64 = v;
  }

  //-----------

  void setInt(int v) {
    assert(meta_.type == PT_INT);
    value_.s64 = v;
  }

  int getInt() {
    assert(meta_.type == PT_INT);
    return static_cast<int>(value_.s64);
  }

  void updateHas() {
    meta_.has = 0;
    if (name_) meta_.has |= AH_NAME;
    if (type_) meta_.has |= AH_TYPE;
    if (meta_.type) meta_.has |= AH_VALUE;
  }

//private:

  enum AtomHas {
    AH_NAME = 1,
    AH_TYPE = 2,
    AH_VALUE = 4,
  };

  MetaStuff     meta_;
  std::string*  name_;
  const Type*   type_;
  ValueStuff    value_;
};