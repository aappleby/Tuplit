#pragma once
#include "Atom.h"

// Wrapper object around a block of atoms, does _not_ own the atoms.

struct Tuple {
  Tuple(Atom* atoms, int length) {
    atoms_ = atoms;
    length_ = length;
  }

  Tuple(const Tuple& t) {
    atoms_ = t.atoms_;
    length_ = t.length_;
  }

  ~Tuple() {
  }

  operator Atom() const {
    return Atom(PT_TUPLE, NULL, TupleType(length_), atoms_);
  }

  int length() const {
    return length_;
  }

  Atom& operator [] (int index) {
    return atoms_[index];
  }

  const Atom& operator [] (int index) const {
    return atoms_[index];
  }
  
  const Atom* byName(string* name) {
    for (int i = 0; i <= length_; i++) {
      if (atoms_[i].name_ == name) {
        return &atoms_[i];
      }
    }
  }

private:
  Atom* atoms_;
  int length_;

  Tuple() {}
};


void meld(Tuple a, Tuple b);
