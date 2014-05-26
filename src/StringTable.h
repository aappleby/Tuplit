#pragma once

#include "Base.h"

#include "HashTable.h"

typedef int StringId;

class StringTable {
  StringTable();

  const char* get    (StringId id);
  StringId    add    (const char* text);
  void        remove (StringId id);

  struct Entry {
    char* text;
    uint64_t hash;
    union {
      int refcount;
      int next;
    };
  };

  int empty_head;
  HashTable table;
  std::vector<Entry>* entries;
};