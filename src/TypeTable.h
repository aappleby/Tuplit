#pragma once

#include "stdint.h"
#include <map>

struct Tuple;

// Maps type hashes to type tuples.

struct TypeTable {
  std::map<uint64_t, Tuple*> hash;
};