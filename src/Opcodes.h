#pragma once

// in to current routine
#define REG_IN(A)        (A | 0x100)
// out from current routine
#define REG_OUT(A)       (A | 0x200)

// in to subroutine
#define REG_ARG(A)       (A | 0x300)
// out from subroutine
#define REG_RET(A)       (A | 0x400)

// in from constant table
#define REG_CONSTANT(A)  (A | 0x500)
// in from context/closure/global
#define REG_CONTEXT(A)   (A | 0x600)

// constant value -128 to 127
#define REG_IMMEDIATE(A) ((A & 0xFF) | 0x700)

// meta-values
#define REG_META(A)      (A | 0x800)

// register set -
// param stack
// result stack
// context stack
// upvalues?
// function-level constants?
// vm-level constants?

enum Opcode {
  OC_NOP,    // nop

  OC_GET,    // ra = rb[rc]
  OC_SET,    // ra[rb] = rc

  OC_LOADC,  // ra = constants[rb];

  OC_CLR,    // ra = 0
  OC_MOV,    // ra = rb
  OC_MOVV,   // ra.value = rb.value
  OC_IMOV,   // r[a + b] = r[c + d]
  OC_BMOV,   // (ra ... r[a+c]) = (rb ... r[b+c])    (block move)
  OC_SWAP,   // temp = ra; ra = rb; rb = temp

  OC_NAME,   // ra = (ra.name : ? = ?)
  OC_TYPE,   // ra = (? : ra.type = ?)
  OC_VALUE,  // ra = (? : ? = ra.value)
  OC_ATOM,   // ra = (rb : rc = rd)

  OC_MELD,   // ?
  OC_LEN,    // ra = len(rb)
  OC_CAR,    // ra = car(rb)
  OC_CDR,    // ra = cdr(rb)
  OC_CONS,   // ra = cons(rb, rc)
  OC_TUPLE,  // ra = tuple(rb ... rc)
  OC_LIST,   // ra = list(rb ... rc)
  OC_SPLAT,  // (ra ... rb) = rc

  OC_PUSH,   // stack += (ra ... rb)
  OC_POP,    // stack -= ra

  OC_ADD,    // ra = rb + rc
  OC_SUB,    // ra = rb - rc
  OC_MUL,    // ra = rb * rc
  OC_DIV,    // ra = rb / rc
  OC_MOD,    // ra = rb % rc
  OC_NEG,    // ra = -rb
  OC_INC,    // ra++;
  OC_DEC,    // ra--;

  OC_AND,    // ra = rb & rc
  OC_OR,     // ra = rb | rc
  OC_XOR,    // ra = rb ^ rc
  OC_NOT,    // ra = ~rb

  OC_ASR,    // ra = rb >> rc
  OC_ASL,    // ra = rb << rc
  OC_SHR,    // ra = rb >>> rc
  OC_SHL,    // ra = rb <<< rc
  OC_ROR,    // ra = rb <|< rc
  OC_ROL,    // ra = rb >|> rc

  OC_EQ,     // ra = rb == rc
  OC_LT,     // ra = rb < rc
  OC_LE,     // ra = rb <= rc

  // ra = function, rb = args base, rc = # args, rd = # results
  OC_CALL,   // rc = # args returned by ra(arg0, arg1, ... arg[b])
  OC_EVAL,   // rb = # args returned by eval(splat(ra))?
  OC_RET,
  OC_YIELD,

  OC_JMP,    // pc += ra;
  OC_JNE,    // if (ra != rb) pc += rc;
  OC_JEQ,    // if (ra == rb) pc += rc;
  OC_JLT,    // if (ra <  rb) pc += rc;
  OC_JLE,    // if (ra <= rb) pc += rc;
  OC_JZ,     // if (ra == 0) pc += rb;
  OC_JNZ,    // if (ra != 0) pc += rb;

  OC_LOOP1A, // if (rb = 0) { pc += c } else { ra = 0; }
  OC_LOOP1B, // ra++; if (ra < rb) { pc -= c }

  OC_DUMP,   // dump ra to console
  OC_PRINTC, // printf("%s",toString(ra.value));
};

extern const char* opcodeStrings[];

struct Instruction {
  Opcode opcode;
  int ra;
  int rb;
  int rc;
  int rd;
};
