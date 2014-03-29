#pragma once

#include <vector>
#include "stdint.h"
#include "HashTable.h"

typedef int StringId;

class StringTable {
  StringTable() {
    entries.resize(16);
    entries[15].next = -1;
    for (int i = 0; i < 15; i++) {
      entries[i].next = i + 1;
    }
    empty_head = 0;
  }

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
  std::vector<Entry> entries;
};