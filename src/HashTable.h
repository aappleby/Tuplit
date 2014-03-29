#pragma once

#include "stdint.h"

class HashTable {
public:
  HashTable(int size = 64);
  ~HashTable();

  uint64_t get (int cell) {
    return cells[cell].value;
  }
  void set (int cell, uint64_t value) {
    cells[cell].value = value;
  }
  uint64_t getKey (int cell) {
    return cells[cell].value;
  }
  void erase (int cell) {
    if (cells[cell].key != EMPTY) {
      //used_count--;
      cells[cell].key = EMPTY;
    }
  }

  int find (uint64_t key);
  int add (uint64_t key, uint64_t value);

private:

  static const int HASH_COUNT = 4;
  static const uint64_t EMPTY = 0x454d50545943454c; // "EMPTYCEL"

  void resize (int new_size);

  int bumpA(uint64_t key);

  struct Cell {
    uint64_t key;
    uint64_t value;
  };

  int used_count;
  int size_at_rehash;

  int cell_count;
  Cell* cells;
};

