#include "Tuple.h"

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