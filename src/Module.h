// A module is a collection of data and functions produced by parsing
// a source file.


#pragma once

#include "Atom.h"
#include "Function.h"

class Module {
public:

  std::map<std::string, Function> functions;
  std::map<std::string, Atom> tuples;

private:
};