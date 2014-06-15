#include "Tuple.h"

void Tuple::dump() {
  printf("(");
  for (int i = 0; i < size(); i++) {
    at(i).dump();
    if (i != size() - 1) printf(",");
  }
  printf(")");
};

void meld(Tuple a, Tuple b) {
  //return Tuple(3);
}