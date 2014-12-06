print("hello world");
stoplexing

implied_int := 3;
implied_float := 4.1;
implied_string := "five";
two : int32 = 2;
pi : float32 = 3.1415926535897932384626433832795028814971;
cheese: string = "delicious";
stoplexing

sum : int32 = ((1 + 1) * (4 + 87));
stoplexing

a : int32 = 1 + 1;
stoplexing;
print(a + 1);


stoplexing

//------------------------------------------------------------------------------
// Tuplit

gcd := function(a : int, b : int) : int {
  while(b) {
    a = a % b;
    swap(a,b);
  }
  result = a;
}

stoplexing

// C

int gcd(int a, int b) {
  int temp;
  while (b != 0) {
    temp = b;
    b = a % b;
    a = temp;
  }
  return a;
}

gcd := function(a : int, b : int) : (a : int) {
  result = loop (a, b) {
    next = b ? (b, a % b) : break;
  }
}

gcd := function(a : int, b : int) : int {
  result = b ? gcd(b, a % b) : a;
}

print(gcd(135, 25))

// ILA

// r0 = a
// r1 = b
// r2 = temp;
loop:
cmp   r3, r1, 0
cjmp  done, r3
mov   r2, r1
mod   r1, r0, r1
mov   r0, r2
jmp   loop
done:
ret   r0

#define OP_CONSTANT(A) (A | 256)

Instruction gcd[] = {
  { OP_CMP,  3, 1, OP_CONSTANT(0) },
  { OP_CJMP, 3, 5 },
  { OP_MOV,  2, 1 },
  { OP_MOD,  1, 0, 1 },
  { OP_MOV,  0, 2 },
  { OP_JMP,  -5 },
  { OP_RET,  2 },
};


------------






data1 := (a : int = 4, b : string = "hello")
data2 := (int = 4, string = "hello")
data3 := (int, string)

print(data3)
> Tuple(? : int = ?, ? : string = ?)


Rectangle := (width : number, height : number)



myRectangle := Rectangle :: (10, 20)
print(myRectangle)
> Tuple(width : number = 10, height : number = 20)

