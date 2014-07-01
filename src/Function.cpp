#include "Function.h"


void Function::dump() {
  printf("Function %s\n", name.c_str());

  printf("Constants:\n");
  constants.dump();
  printf("\n");

  printf("Args:\n");
  args.dump();
  printf("\n");

  printf("Locals:\n");
  locals.dump();
  printf("\n");

  printf("Results:\n");
  results.dump();
  printf("\n");
}