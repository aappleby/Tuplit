#pragma once

struct Atom;

// All atoms, and by extension all referenceable objects in the VM, live in the atom pool.

struct AtomPool {
  // slabs grouped by allocation size...
};