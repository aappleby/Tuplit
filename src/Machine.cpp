#include "Machine.h"
#include "Type.h"

Atom * immediate = NULL;

//------------------------------------------------------------------------------

Machine::Machine() {
  temp_base = temp = new Atom[1000];
  in_base = in = new Atom[1000];
  out_base = out = new Atom[1000];
  arg_base = arg = new Atom[1000];
  ret_base = ret = new Atom[1000];

  immediate = new Atom[256];
  for (int i = 0; i < 127; i++) {
    immediate[i] = Atom(i);
  }
  for (int i = 128; i < 256; i++) {
    immediate[i] = Atom(i - 256);
  }
}

//------------------------------------------------------------------------------

void Machine::run(Instruction* code) {
  Instruction* pc = code;

  Atom constants[10];
  Atom context[10];
  Atom meta[10];

  Atom* stacks[] = {
    temp,
    in,
    out,
    arg,
    ret,

    constants,
    context,
    immediate,
  };

  bool running = true;
  while(running) {
    /*
    if ((pc->ra & 0xF800) ||
        (pc->rb & 0xF800) ||
        (pc->rc & 0xF800) ||
        (pc->rd & 0xF800)) {
      // meta-op, we're reading/writing the state of the VM itself.

      switch(pc->opcode) {
      case OC_MOV:
        break;
      case OC_LEN:
        break;
      }
      pc++;
      continue;
    }
    */

    Atom& ra = stacks[pc->ra >> 8][pc->ra & 0xFF];
    Atom& rb = stacks[pc->rb >> 8][pc->rb & 0xFF];
    Atom& rc = stacks[pc->rc >> 8][pc->rc & 0xFF];
    Atom& rd = stacks[pc->rd >> 8][pc->rd & 0xFF];

    //----------

    switch(pc->opcode) {
    case OC_NOP:
      pc++;
      break;

    case OC_GET:
      rd = get(ra, rb);
      pc++;
      break;

    case OC_SET:
      set(ra, rb, rc);
      continue;

    case OC_MOV:
      ra = rb;
      break;

    case OC_IMOV:
      stacks[pc->ra >> 8][static_cast<int8_t>((pc->ra + pc->rb) & 0xFF)] =
        stacks[pc->rc >> 8][static_cast<int8_t>((pc->rc + pc->rd) & 0xFF)];
      break;

    case OC_SWAP:
      {
        Atom t = ra;
        ra = rb;
        rb = t;
      }
      break;

    //----------
    // Atom break/build.

    case OC_NAME:
      ra = name(rb);
      break;

    case OC_TYPE:
      ra = type(rb);
      break;

    case OC_VALUE:
      ra = value(rb);
      break;

    case OC_ATOM:
      ra = atom(rb, rc, rd);
      break;

    //----------
    // List & tuple manipulation
    
    case OC_MELD:
      break;

    case OC_LEN:
      if ((pc->rb & 0xF800) == 0) {
        // length of thing in atom
        ra = len(rb);
      } else {
        // meta length
        switch(pc->rb >> 11) {
        case 0:
          // length of the temp stack;
          ra = Atom(10);
          break;
        case 1:
          // length of the in stack
          ra = Atom(10);
          break;
        case 2:
          // length of the out stack
          ra = Atom(10);
          break;
        case 3:
          // length of the arg stack
          ra = Atom(10);
          break;
        case 4:
          // length of the ret stack
          ra = Atom(10);
          break;
        case 5:
          // length of the constants block
          ra = Atom(10);
          break;
        case 6:
          // length of the context block
          ra = Atom(10);
          break;
        case 7:
          // length of the immediate block
          ra = Atom(256);
          break;
        }
      }
      break;

    case OC_CAR:
      ra = car(rb);
      break;

    case OC_CDR:
      ra = cdr(rb);
      break;

    case OC_CONS:
      ra = cons(rb, rc);
      break;

    case OC_TUPLE:
      ra = tuple(pc->rb, pc->rc);
      break;

    case OC_LIST:
      ra = list(pc->rb, pc->rc);
      break;

    case OC_SPLAT:
      // Inverse of OC_TUPLE, splats a tuple into the stack.
      break;

    //----------
    // Math

    case OC_ADD:
      rd = add(ra, rb);
      break;

    case OC_SUB:
    case OC_MUL:
    case OC_DIV:
    case OC_MOD:
      ra = Atom(rb.value_.s64 % rc.value_.s64);
      break;
    case OC_NEG:
      break;

    //----------
    // Logical ops.
    case OC_AND:
    case OC_OR:
    case OC_XOR:
    case OC_NOT:
      break;

    //----------
    // Bitwise ops.
    case OC_ASR:
    case OC_ASL:
    case OC_SHR:
    case OC_SHL:
    case OC_ROR:
    case OC_ROL:
      break;

    //----------
    // Comparisons
    case OC_EQ:
    case OC_LT:
    case OC_LE:
      break;


    //----------
    // Control flow
    case OC_CALL:
      break;
    case OC_EVAL:
      break;
    case OC_RET:
      running = false;
      break;
    case OC_YIELD:
      running = false;
      break;

    case OC_JMP:
      pc += ra.value_.s64;
      pc--;
      break;

    case OC_JNE:
      if (ra.value_.s64 != rb.value_.s64) {
        pc += rc.value_.s64;
        pc--;
      }
      break;

    case OC_JZ:
      if (ra.value_.s64 == 0) {
        pc += rb.value_.s64;
        pc--;
      }
      break;

    case OC_JEQ:
    case OC_JLT:
    case OC_JLE:

    //----------
    // Debugging;
    case OC_DUMP:
      break;

    case OC_PRINTC:
      printf("OC_PRINTC\n");
      break;

    //----------
    // Error handling
    default:
      assert(false && "oops, bad opcode");
      running = false;
      break;
    }

    //----------

    pc++;
  }
}

//------------------------------------------------------------------------------

Atom Machine::get(Atom& a, Atom& b) {
  assert((a.ptype_ == PT_TUPLE) ||
         (a.ptype_ == PT_LIST));

  assert((b.ptype_ == PT_INT) ||
         (b.ptype_ == PT_TEXT));

  if (a.ptype_ == PT_TUPLE) {
    if (b.ptype_ == PT_INT) {
      assert(b.value_.s64 >= 0);
      assert(b.value_.s64 < TupleLength(a.type_));
      return a.value_.atom[b.value_.s64];
    } else {
      for (int i = 0; i < TupleLength(a.type_); i++) {
        if (a.value_.atom[i].name_ == b.text_) {
          return a.value_.atom[i];
        }
      }
    }
  }

  return Atom::nil;
}

void Machine::set(Atom& a, Atom & b, Atom& c) {
}

//------------------------------------------------------------------------------

Atom Machine::name(Atom& a) {
  return Atom::name(a);
}

Atom Machine::type(Atom& a) {
  return Atom::type(a);
}

Atom Machine::value(Atom& a) {
  return Atom::value(a);
}

Atom Machine::len(Atom& ra) {
  return Atom();
}

//------------------------------------------------------------------------------

Atom Machine::atom(Atom& ra, Atom& rb, Atom& rc) {
  return Atom();
}

Atom Machine::tuple(int a, int b) {
  return Atom();
}

Atom Machine::list(int a, int b) {
  return Atom();
}

//------------------------------------------------------------------------------

Atom Machine::car(Atom& a) {
  assert((a.ptype_ == PT_LIST) ||
         (a.ptype_ == PT_TUPLE));
  return Atom();
}

Atom Machine::cdr(Atom& a) {
  assert((a.ptype_ == PT_LIST) ||
         (a.ptype_ == PT_TUPLE));
  return Atom();
}

Atom Machine::cons(Atom& a, Atom& b) {
  return Atom();
}

//------------------------------------------------------------------------------

Atom Machine::add(Atom& a, Atom& b) {
  if (a.ptype_ != b.ptype_) return Atom::nil;

  if (a.ptype_ == PT_INT) {
    return Atom(a.value_.s64 + b.value_.s64);
  }

  if (a.ptype_ == PT_FLOAT) {
    return Atom(a.value_.f64 + b.value_.f64);
  }

  return Atom::nil;
}

Atom Machine::sub(Atom& a, Atom& b) {
  return Atom(a.value_.s64 - b.value_.s64);
}

Atom Machine::mul(Atom& a, Atom& b) {
  return Atom(a.value_.s64 * b.value_.s64);
}

Atom Machine::div(Atom& a, Atom& b) {
  return Atom(a.value_.s64 / b.value_.s64);
}

Atom Machine::mod(Atom& a, Atom& b) {
  return Atom(a.value_.s64 % b.value_.s64);
}

Atom Machine::neg(Atom& a) {
  return Atom();
}

//------------------------------------------------------------------------------
