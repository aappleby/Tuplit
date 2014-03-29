#include "Base.h"
#include "Tuple.h"

/*
int concat(AtomStack& stack) {

  Tuple& a = stack[0].getTuple();
  Tuple& b = stack[1].getTuple();

  Tuple* result = Tuple::alloc(a.length() + b.length());

  for (int i = 0; i < a.length(); i++) {
    (*result)[i] = a[i];
  }

  for (int i = 0; i < b.length(); i++) {
    (*result)[i + a.length()] = b[i];
  }

  stack.pop(2);
  stack.push(Atom(result));

  return 0;
}

int car (AtomStack& stack) {
  Tuple& a = stack[0].getTuple();

  stack.pop();
  stack.push(a[0]);

  return 0;
}

int cdr (AtomStack& stack) {
  Tuple& a = stack[0].getTuple();

  Tuple* result = Tuple::alloc(a.length() - 1);

  for (int i = 1; i < a.length(); i++) {
    (*result)[i - 1] = a[i];
  }

  stack.pop(2);
  stack.push(Atom(result));

  return 0;
}
*/