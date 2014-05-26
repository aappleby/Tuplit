#include "StringTable.h"

#include <vector>

StringTable::StringTable() {
  entries->resize(16);
  entries->at(15).next = -1;
  for (int i = 0; i < 15; i++) {
    entries->at(i).next = i + 1;
  }
  empty_head = 0;
}

const char* StringTable::get(StringId id) {
  return entries->at(id).text;
}

StringId StringTable::add(const char* text) {
  uint64_t hash = 0;
  int len = 0;
  const char* cursor = text;
  while(*cursor) {
    hash ^= *cursor++;
    hash *= 0xff51afd7ed558ccd;
    hash = (hash >> 32) | (hash << 32);
    len++;
  }

  int cell = table.find(hash);
  if (cell >= 0) {
    int id = static_cast<int>(table.get(cell));
    entries->at(id).refcount++;
    return id;
  }

  // New string, create a new string table entry for it.
  int id = static_cast<int>(entries->size());
  entries->resize(id + 1);

  Entry& entry = entries->at(id);
  entry.text = new char[len + 1];
  strcpy_s(entry.text, len + 1, text);
  entry.hash = hash;
  entry.refcount = 1;

  table.add(hash, id);
  return id;
}

void StringTable::remove(StringId id) {
  Entry& entry = entries->at(id);
  entry.refcount--;
}

