#include "Atom.h"

const Atom Atom::nil = Atom();

void Atom::dump() {
  /*
  if (isNil()) {
    printf("<nil>");
    return;
  }
  */

  string& s = name_.empty() ? "?" : name_;
  string& t = type_.empty() ? "?" : type_;
  printf("%s : %s = ", s.c_str(), t.c_str());

  switch (ptype_) {
    case PT_INT:
      printf("%d", value_.s64);
      break;
    case PT_UINT:
      printf("%u", value_.u64);
      break;
    case PT_FLOAT:
      printf("%f", value_.f64);
      break;
    case PT_TEXT:
      printf("'%s'", text_.c_str());
      break;
    case PT_NIL:
      printf("<nil>");
      break;
    case PT_HOOK:
      printf("<hook @ %p>", value_.hook);
      break;
    default:
      printf("<unknown>");
      break;
  }
};
