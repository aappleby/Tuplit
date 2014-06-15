#pragma once

#include "Base.h"
#include "Type.h"

#pragma warning(disable : 4201)

class Atom;
class Function;
class Type;

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
  PT_VOID,
  PT_FUNC,
};

union ValueStuff {
  int64_t   s64;
  uint64_t  u64;
  double    f64;

  Atom*     atom;
  uint8_t*  blob;
  Function* func;
  Type*     type;
};

//------------------------------------------------------------------------------

class Atom {
public:
  Atom() {
    name_ = "";
    type_ = "";
    ptype_ = PT_NIL;
    value_.u64 = 0;
  }

  Atom(const Atom& a) {
    name_ = a.name_;
    type_ = a.type_;
    ptype_ = a.ptype_;
    value_ = a.value_;
  }

  Atom& operator = (const Atom& a) {
    assert(this != &nil);
    name_ = a.name_;
    type_ = a.type_;
    ptype_ = a.ptype_;
    value_ = a.value_;
    return *this;
  }

  //----------

  Atom(const string& name, const string& type)
  : name_(name), type_(type), ptype_(PT_NIL) {}

  Atom(const string& name, const string& type, int64_t value, const string& text)
  : name_(name), type_(type), ptype_(PT_INT), text_(text) { value_.s64 = value; }

  Atom(const string& name, const string& type, uint64_t value, const string& text)
  : name_(name), type_(type), ptype_(PT_UINT), text_(text) { value_.u64 = value; }

  Atom(const string& name, const string& type, double value, const string& text)
  : name_(name), type_(type), ptype_(PT_FLOAT), text_(text) { value_.f64 = value; }

  Atom(const string& name, const string& type, const string& text)
  : name_(name), type_(type), ptype_(PT_TEXT), text_(text) {}

  //----------
  // Extract name/type/value of atoms.

  static Atom name(const Atom& a) {
    Atom result;
    result.name_ = a.name_;
    return result;
  }

  static Atom type(const Atom& a) {
    Atom result;
    result.type_ = a.type_;
    return result;
  }

  static Atom value(const Atom& a) {
    Atom result;
    result.ptype_ = a.ptype_;
    result.value_.u64 = a.value_.u64;
    result.text_ = a.text_;
    return result;
  }

  static Atom make_type(const char* name, Type* value = NULL) {
    Atom result;
    result.name_ = name;
    result.type_ = "<type>";
    result.ptype_ = PT_TYPE;
    result.value_.type = value;
    return result;
  }

  //----------
  // Construct partial atoms.

  static Atom name(const char* name) {
    Atom result;
    result.name_ = name;
    return result;
  }

  static Atom name(string& name) {
    Atom result;
    result.name_ = name;
    return result;
  }

  static Atom type(string& type) {
    return Atom("", type);
  }

  static Atom value(int64_t value) {
    return Atom("", "int", value, "");
  }

  static Atom value(double value) {
    return Atom("", "float", value, "");
  }

  static Atom value(string& value) {
    return Atom("", "string", value);
  }

  // Symbolic token, 
  static Atom symbol(string& type) {
    return Atom("<symbol>", type);
  }

  //----------

  bool isNil() const {
    return ptype_ == PT_NIL;
  }

  bool isFunction() const {
    return ptype_ == PT_FUNC;
  }

  bool isType() const {
    return ptype_ == PT_TYPE;
  }

  bool isSymbol() const {
    return name_ == "<symbol>";
  }

  //----------
  // Conversions from Tuplit types

  explicit Atom(uint8_t* v) {
    name_ = "";
    type_ = "";
    ptype_ = PT_BLOB;
    value_.blob = v;
  }

  explicit Atom(string& v) {
    name_ = "";
    type_ = "";
    ptype_ = PT_TEXT;
    value_.u64 = 0;
    text_ = v;
  }

  explicit Atom(Function* f) {
    name_ = "";
    type_ = "function";
    ptype_ = PT_FUNC;
    value_.func = f;
  }

  //-----------
  // Conversions from base types

  explicit Atom(int32_t v) {
    name_ = "";
    type_ = "int32";
    ptype_ = PT_INT;
    value_.s64 = v;
  }

  explicit Atom(int64_t v) {
    name_ = "";
    type_ = "int64";
    ptype_ = PT_INT;
    value_.s64 = v;
  }

  explicit Atom(double v) {
    name_ = "";
    type_ = "double";
    ptype_ = PT_FLOAT;
    value_.f64 = v;
  }

  //-----------
  // Copy/merge from another atom.

  void setName(Atom const& a) {
    name_ = a.name_;
  }

  void setType(Atom const& a) {
    type_ = a.type_;
  }

  void setValue(Atom const& a) {
    ptype_ = a.ptype_;
    value_.u64 = a.value_.u64;
    text_ = a.text_;
  }

  //-----------

  void setInt(int v) {
    assert(ptype_ == PT_INT);
    value_.s64 = v;
  }

  int getInt() {
    assert(ptype_ == PT_INT);
    return static_cast<int>(value_.s64);
  }

  //-----------

  void dump() {
    switch(ptype_) {
      case PT_FLOAT:
        {
          string s = name_.empty() ? "?" : name_;
          printf("%s : %s = %f", s.c_str(), type_.c_str(), value_.f64);
        }
        break;
      case PT_TEXT:
        printf("%s : %s = %s", name_.c_str(), type_.c_str(), text_.c_str());
        break;
      default:
        printf("<unknown>");
        break;
    }
  };

  //-----------
  // Global constant atoms;

  static const Atom nil;

//private:

  // True if this is a symbolic atom - it represents the result of a
  // symbolic computation and not an actual computation.
  bool          symbol_;

  std::string   name_;
  std::string   type_;
  PhysicalType  ptype_;
  ValueStuff    value_;
  string        text_;
};

typedef std::map<string, Atom> AtomMap;