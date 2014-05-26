#pragma once
#include "Atom.h"
#include "Opcodes.h"

class Machine {
public:

  Machine();

  void run(Instruction* code);

  Atom get(Atom& a, Atom& b);
  void set(Atom& a, Atom& b, Atom& c);

  Atom name  (Atom& a);
  Atom type  (Atom& a);
  Atom value (Atom& a);

  Atom atom  (Atom& ra, Atom& rb, Atom& rc);
  Atom tuple (int a, int b);
  Atom list  (int a, int b);

  Atom len   (Atom& ra);

  Atom car   (Atom& ra);
  Atom cdr   (Atom& ra);
  Atom cons  (Atom& ra, Atom& rb);

  Atom add   (Atom& ra, Atom& rb);
  Atom sub   (Atom& ra, Atom& rb);
  Atom mul   (Atom& ra, Atom& rb);
  Atom div   (Atom& ra, Atom& rb);
  Atom mod   (Atom& ra, Atom& rb);
  Atom neg   (Atom& ra);

  Atom * temp;
  Atom * in;
  Atom * out;
  Atom * arg;
  Atom * ret;

  Atom * temp_base;
  Atom * in_base;
  Atom * out_base;
  Atom * arg_base;
  Atom * ret_base;

  int cycle;
};