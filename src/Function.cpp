#include "Function.h"


void Function::dump() {
  printf("Function %s\n", name.c_str());

  printf("Constants: ");
  constants.dump();
  printf("\n");

  printf("Args: ");
  args.dump();
  printf("\n");

  printf("Locals: ");
  locals.dump();
  printf("\n");

  printf("Results: ");
  results.dump();
  printf("\n");
}