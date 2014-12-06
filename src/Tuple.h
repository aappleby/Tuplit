#pragma once
#include "Atom.h"

// Wrapper object around a block of atoms, does _not_ own the atoms.

/*
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

  int length() const {
    return length_;
  }

  Atom& operator [] (int index) {
    return atoms_[index];
  }

  const Atom& operator [] (int index) const {
    return atoms_[index];
  }
  
  const Atom* byName(string& name) {
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
*/

class Tuple : public std::vector<Atom> {
public:
  Tuple() {
  };

  Tuple(std::initializer_list<Atom> x);

  void dump();
};

void meld(Tuple a, Tuple b);
