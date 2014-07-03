#include "HashTable.h"
//#include <assert.h>
#include <stdio.h>
#include <stdlib.h>



uint64_t mix1(uint64_t k) {
  //k *= 0xff51afd7ed558ccd;
  //k = (k >> 45) | (k << 19);
  //k *= 0xff51afd7ed558ccd;
  return k;
}

uint64_t mix2(uint64_t k) {
  k *= 0xff51afd7ed558ccd;
  k = (k >> 45) | (k << 19);
  return k;
}

HashTable::HashTable(int size) {
  cells = NULL;
  cell_count = 0;
  size_at_rehash = 0;
  used_count = 0;

  resize(size);
}

HashTable::~HashTable() {
  delete [] cells;
}

void HashTable::resize(int new_size) {
  Cell* old_cells = cells;
  uint64_t old_count = cell_count;
  size_at_rehash = used_count;

  cell_count = new_size;
  cells = new Cell[new_size];
  for (int i = 0; i < cell_count; i++) {
    cells[i].key = EMPTY;
    cells[i].value = EMPTY;
  }

  for (int i = 0; i < old_count; i++) {
    if (old_cells[i].key != EMPTY) {
      int cell = bumpA(old_cells[i].key);
      cells[cell].key = old_cells[i].key;
      cells[cell].value = old_cells[i].value;
    }
  }

  delete [] old_cells;
}

int HashTable::add(uint64_t key, uint64_t value) {
  assert(key != EMPTY);
  assert(find(key) == -1);

  int cell = bumpA(key);
  if (cell >= 0) {
    cells[cell].key = key;
    cells[cell].value = value;
    used_count++;
    return cell;
  }

  resize(cell_count * 2);
  return add(key, value);
}

int HashTable::find(uint64_t key) {
  uint64_t hash = key;

  hash = mix1(hash);

  for (int i = 0; i < HASH_COUNT; i++) {
    int index = hash & (cell_count - 1);
    if (cells[index].key == key) return index;
    hash = mix2(hash);
  }

  return -1;
}

int HashTable::bumpA(uint64_t key) {

  uint64_t hashA = mix1(key);
  for (int i = 0; i < HASH_COUNT; i++) {
    int indexA = hashA & (cell_count - 1);

    if (cells[indexA].key == EMPTY) {
      return indexA;
    }

    uint64_t hashB = mix1(cells[indexA].key);
    for (int i = 0; i < HASH_COUNT; i++) {
      int indexB = hashB & (cell_count - 1);
      if (indexB == indexA) goto skipB;

      if (cells[indexB].key == EMPTY) {
        cells[indexB] = cells[indexA];
        return indexA;
      }

      uint64_t hashC = mix1(cells[indexB].key);
      for (int i = 0; i < HASH_COUNT; i++) {
        int indexC = hashC & (cell_count - 1);
        if (indexC == indexA) goto skipC;
        if (indexC == indexB) goto skipC;

        if (cells[indexC].key == EMPTY) {
          cells[indexC] = cells[indexB];
          cells[indexB] = cells[indexA];
          return indexA;
        }

        uint64_t hashD = mix1(cells[indexC].key);
        for (int i = 0; i < HASH_COUNT; i++) {

          int indexD = hashD & (cell_count - 1);
          if (indexD == indexA) goto skipD;
          if (indexD == indexB) goto skipD;
          if (indexD == indexC) goto skipD;

          if (cells[indexD].key == EMPTY) {
            cells[indexD] = cells[indexC];
            cells[indexC] = cells[indexB];
            cells[indexB] = cells[indexA];
            return indexA;
          }
skipD:
          hashD = mix2(hashD);
        }
skipC:
        hashC = mix2(hashC);
      }
skipB:
      hashB = mix2(hashB);
    }
    hashA = mix2(hashA);
  }

  return -1;
}