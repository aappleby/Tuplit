#pragma once
#include "Atom.h"

#include <vector>

struct AtomStack {
  std::vector<Atom> atoms;
  int top;

  void push(const Atom& atom) {
    atoms.push_back(atom);
  }

  void pop(int count = 1) {
    atoms.erase(atoms.end() - count, atoms.end());
  }

  Atom& operator [] (int index) {
    return atoms[top + index];
  }

  const Atom& operator [] (int index) const {
    return atoms[top + index];
  }
};