#pragma once
#include "Atom.h"

struct Tuple {

  Tuple(int size) {
    head = new Atom[size + 1];
    head[0].setInt(size);
  }

  Tuple(const Tuple& t) {
    int len = t.length();
    head = new Atom[len + 1];
    memcpy(head, t.head, len * sizeof(Atom));
  }

  ~Tuple() {
    delete [] head;
  }

  int length() const {
    return head[0].getInt();
  }

  Atom& operator [] (int index) {
    return head[index + 1];
  }

  const Atom& operator [] (int index) const {
    return head[index + 1];
  }

private:
  Atom* head;

  Tuple() {}
};


Tuple meld(Tuple a, Tuple b);
