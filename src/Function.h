#pragma once
#include "Base.h"
#include "Tuple.h"
#include "Opcodes.h"

//-----------------------------------------------------------------------------

class Function {
public:
  // The name of the function.
  std::string name;

  // The arguments to the function - (a, b, c) in the function foo(a, b, c);
  Tuple args;

  // The named local variables in the function. Tuplit uses SSA form; these
  // atoms will be constant inside a block.
  // TODO(aappleby) - All locals, or only locals for the outermost block?
  Tuple locals;

  // The results of the function - (x, y) in the function foo(q) -> (x, y)
  Tuple results;

  // Literal or evaulated constants used in the function.
  Tuple constants;

  // The actual opcodes.
  std::vector<Instruction> code;

  //----------

  void dump();
  void dumpCode();
};

//-----------------------------------------------------------------------------
