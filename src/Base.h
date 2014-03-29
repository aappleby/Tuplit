#pragma once
#include "Stack.h"

// Fundamental methods used by the VM


// Pops two tuples from the stack, pushes the concatenation of the two.
int concat(AtomStack& stack);