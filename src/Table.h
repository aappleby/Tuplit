#pragma once
#include "Atom.h"
#include "HashTable.h"

// Like Lua, the 'Table' data structure is used for dense arrays, sparse
// arrays, and maps. Unlike Lua, there is no split between 'array part' and
// 'map part' - if the key table is present, the entire thing is a map. If the
// key table is missing, the entire thing is an array. Positive integer keys
// are always hashed first to their normal location in the array, and we
// enforce a N% density before the table is transitioned to sparse/map.

class Table {
public:

  struct Key {
    union {
      int64_t  s64;
      uint64_t u64;
      double   f64;
      std::string* pString;
    };
  };

  Key* keys;
  Atom* values;
};