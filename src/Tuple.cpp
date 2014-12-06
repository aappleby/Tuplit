#include "Tuple.h"

Tuple::Tuple(std::initializer_list<Atom> x) {
}

void Tuple::dump() {
  printf("(\n");
  for (size_t i = 0; i < size(); i++) {
    printf("  ");
    at(i).dump();
    if (i != size() - 1) printf(",");
    printf("\n");
  }
  printf(")\n");
};

void meld(Tuple a, Tuple b) {
  //return Tuple(3);
}


Tuple q = {
    {"FOO", "BAR"},
    {"FOO", "BAR", 1, "1"},
    {"FOO", "BAR", 1, "1"},
};

void blah() {
  int q = 10;
  printf("hello world");
}