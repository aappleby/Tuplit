#include "Opcodes.h"

#define $0 REG_IMMEDIATE(0)
#define $1 REG_IMMEDIATE(1)
#define $2 REG_IMMEDIATE(2)
#define $3 REG_IMMEDIATE(3)
#define $4 REG_IMMEDIATE(4)
#define $5 REG_IMMEDIATE(5)
#define $6 REG_IMMEDIATE(6)
#define $7 REG_IMMEDIATE(7)
#define $8 REG_IMMEDIATE(8)
#define $9 REG_IMMEDIATE(9)

#define $n1 REG_IMMEDIATE(-1)
#define $n2 REG_IMMEDIATE(-2)
#define $n3 REG_IMMEDIATE(-3)
#define $n4 REG_IMMEDIATE(-4)
#define $n5 REG_IMMEDIATE(-5)

#define in0 REG_IN(0)
#define in1 REG_IN(1)

#define out0 REG_OUT(0)
#define out1 REG_OUT(1)

#define arg0 REG_ARG(0)
#define arg1 REG_ARG(1)
#define arg2 REG_ARG(2)

#define ret0 REG_RET(0)
#define ret1 REG_RET(1)

#define globals REG_CONTEXT(0)
#define types REG_CONTEXT(1)

#define const0 REG_CONSTANT(0)
#define const1 REG_CONSTANT(1)

#define r0 0
#define r1 1
#define r2 2
#define r3 3

const char* opcodeStrings[] = {
  "OC_NOP",    // nop

  "OC_GET",    // ra = rb[rc]
  "OC_SET",    // ra[rb] = rc

  "OC_LOADC",  // ra = constants[rb];

  "OC_CLR",    // ra = 0
  "OC_MOV",    // ra = rb
  "OC_MOVV",   // ra.value = rb.value
  "OC_IMOV",   // r[a + b] = r[c + d]
  "OC_BMOV",   // (ra ... r[a+c]) = (rb ... r[b+c])    (block move)
  "OC_SWAP",   // temp = ra; ra = rb; rb = temp

  "OC_NAME",   // ra = (ra.name : ? = ?)
  "OC_TYPE",   // ra = (? : ra.type = ?)
  "OC_VALUE",  // ra = (? : ? = ra.value)
  "OC_ATOM",   // ra = (rb : rc = rd)

  "OC_MELD",   // ?
  "OC_LEN",    // ra = len(rb)
  "OC_CAR",    // ra = car(rb)
  "OC_CDR",    // ra = cdr(rb)
  "OC_CONS",   // ra = cons(rb", rc)
  "OC_TUPLE",  // ra = tuple(rb ... rc)
  "OC_LIST",   // ra = list(rb ... rc)
  "OC_SPLAT",  // (ra ... rb) = rc

  "OC_PUSH",   // stack += (ra ... rb)
  "OC_POP",    // stack -= ra

  "OC_ADD",    // ra = rb + rc
  "OC_SUB",    // ra = rb - rc
  "OC_MUL",    // ra = rb * rc
  "OC_DIV",    // ra = rb / rc
  "OC_MOD",    // ra = rb % rc
  "OC_NEG",    // ra = -rb
  "OC_INC",    // ra++;
  "OC_DEC",    // ra--;

  "OC_AND",    // ra = rb & rc
  "OC_OR",     // ra = rb | rc
  "OC_XOR",    // ra = rb ^ rc
  "OC_NOT",    // ra = ~rb

  "OC_ASR",    // ra = rb >> rc
  "OC_ASL",    // ra = rb << rc
  "OC_SHR",    // ra = rb >>> rc
  "OC_SHL",    // ra = rb <<< rc
  "OC_ROR",    // ra = rb <|< rc
  "OC_ROL",    // ra = rb >|> rc

  "OC_EQ",     // ra = rb == rc
  "OC_LT",     // ra = rb < rc
  "OC_LE",     // ra = rb <= rc

  // ra = function, rb = args base, rc = # args, rd = # results
  "OC_CALL",   // rc = # args returned by ra(arg0, arg1, ... arg[b])
  "OC_EVAL",   // rb = # args returned by eval(splat(ra))?
  "OC_RET",
  "OC_YIELD",

  "OC_JMP",    // pc += ra;
  "OC_JNE",    // if (ra != rb) pc += rc;
  "OC_JEQ",    // if (ra == rb) pc += rc;
  "OC_JLT",    // if (ra <  rb) pc += rc;
  "OC_JLE",    // if (ra <= rb) pc += rc;
  "OC_JZ",     // if (ra == 0) pc += rb;
  "OC_JNZ",    // if (ra != 0) pc += rb;

  "OC_LOOP1A", // if (rb = 0) { pc += c } else { ra = 0; }
  "OC_LOOP1B", // ra++; if (ra < rb) { pc -= c }

  "OC_DUMP",   // dump ra to console
  "OC_PRINTC", // printf("%s",toString(ra.value));
};

// Compute the GCD of a and b.
// in0 = a
// in1 = b
// r0 = tempA
// r1 = tempB
// out0 = gcd of a and b
Instruction gcd[] = {
  { OC_MOV,  r0, in0 },
  { OC_MOV,  r1, in1 },

  { OC_JZ,   r1, $4 },
  { OC_MOD,  r0, r0, r1 },
  { OC_SWAP, r0, r1 },
  { OC_JMP,  $n3 },

  { OC_MOV,  out0, r0 },
  { OC_RET,  $1 },
};


// Make a table with t[0] = 0, t[1] = 1, ...
// in0 = size
// r0 = table
// r1 = counter
// r2 = compare result
// r3 = trash
// result0 = table
// constant0 = "new"
// constant1 = "table"
// context0 = global
// context1 = types
Instruction range[] = {

  // look up 'new' in globals, store in r0
  { OC_GET,  r0, globals, const0 },
  // look up 'table' in types, store in arg0
  { OC_GET,  arg0, types, const1 },
  // copy size to arg1
  { OC_MOV,  arg1, in0 },
  // call table constructor with 2 arguments
  { OC_CALL, r0, $2, r3 },
  // move result0 to out0
  { OC_MOV,  r0, ret0 },

  // set table items
  { OC_LOOP1A, r0, in0, $3 },
  { OC_SET,    out0, r0, r0 },
  { OC_LOOP1B, r0, in0, $n1 },

  { OC_MOV,  out0, r0 },
  { OC_RET,  $1 },
};


// Return all arguments to the function as a tuple.
Instruction tuplify[] = {
  { OC_TUPLE, r0, in0, r0 },
  { OC_MOV,   out0, r0 },
  { OC_RET,   $1 }
};

// Calls function in in0 with args from tuple at in1
// in0 = function to call
// in1 = function args
// r0 = counter
// r1 = arg length
// r2 = compare result
// r3 = temp value from function args
Instruction apply[] = {
  // get the # arguments
  { OC_LEN,  r1, in1 },

  // Copy args to arg[1...]
  { OC_JZ,   r1, $5 },
  { OC_GET,  r3, in1, r0 },
  { OC_IMOV, arg0, r0, r3, $0 },
  { OC_INC,  r0 },
  { OC_JNE,  r0, r1, $n3 },

  // Or we could do this with a splat...
  //{ OC_SPLAT, arg0, in1 },

  // Call the function.
  { OC_CALL, in0, r1, r3 },

  // Copy results to out[0...]
  { OC_MOV,  r0, $0 },
  { OC_JZ,   r3, $4 },
  { OC_IMOV, out0, r0, ret0, r0 },
  { OC_INC,  r0 },
  { OC_JNE,  r0, r3, $n2 },

  // Or block move
  // { OC_BMOV, out0, ret0, r3 }

  // Done
  { OC_RET, r3 }
};
