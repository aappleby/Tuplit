#include "Atom.h"

const Atom Atom::nil = Atom();

void Atom::dump() {
  switch(ptype_) {
    case PT_INT:
      {
        string s = name_.empty() ? "?" : name_;
        printf("%s : %s = %d", s.c_str(), type_.c_str(), value_.s64);
      }
      break;
    case PT_UINT:
      {
        string s = name_.empty() ? "?" : name_;
        printf("%s : %s = %u", s.c_str(), type_.c_str(), value_.u64);
      }
      break;
    case PT_FLOAT:
      {
        string s = name_.empty() ? "?" : name_;
        printf("%s : %s = %f", s.c_str(), type_.c_str(), value_.f64);
      }
      break;
    case PT_TEXT:
      printf("%s : %s = '%s'", name_.c_str(), type_.c_str(), text_.c_str());
      break;
    default:
      printf("<unknown>");
      break;
  }
};
