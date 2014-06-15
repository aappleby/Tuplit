#include<stdio.h>

range := function args := (?, min, max, step = 1), block {
  eval (cursor = args.min) {
    if (cursor >= args.max) return;
    eval (param[0].name = cursor) block;
    repeat (cursor + step);
  }
}

int main (int argc, char **argv)
{
  int iter = 50;
  size := atoi(argv[1]);

  range (y, 0, size) {
    range (x, 0, size) {

      Cr := (2.0*x/w - 1.5);
      Ci := (2.0*y/h - 1.0);

      reps = eval (rep : int = 0, r : double = 0, i : double = 0) {
        if (r * r + i * i >= 4) return rep;
        if (rep == iter) return iter;
        r2 = 2.0 * r * i + Ci;
        i2 = r * r - i * i + Cr
        repeat (rep + 1, r2, i2);
      }

      putc reps;
    }
  }	
}

//-----------------------------------------------------------------------------

// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
//
// contributed by Ian Osgood

A := (i : double, j : double) : double
{
  return 1/((i+j)*(i+j+1)/2+i+1);
}

Au := function(u : array<double>) : array<double> {
  concat (i, u.length) : double {
    sum (j, u.length) : double {
      A(i, j) * u[j];
    }
  }
}

Atu := function(u : array<double>) : array<double> {
  concat (i, u.length) {
    sum (j, u.length) {
      A(j, i) * u[j];
    }
  }
}

function AtAu(u,v,w) {
  w = Au(u);
  v = Atu(w);
}

function spectralnorm(n) {
  u = concat(n) 1;
  v = concat(n) 0;
  w = concat(n) 0;

  var i, u=[], v=[], w=[], vv=0, vBv=0;
  for (i=0; i<n; ++i) {
    u[i] = 1; v[i] = w[i] = 0; 
  }
  for (i=0; i<10; ++i) {
    AtAu(u,v,w);
    AtAu(v,u,w);
  }
  for (i=0; i<n; ++i) {
    vBv += u[i]*v[i];
    vv  += v[i]*v[i];
  }
  return Math.sqrt(vBv/vv);
}

print(spectralnorm(arguments[0]).toFixed(9));


for = function args block {
  context = args[0];
  condition = args[1];
  step = args[2];

  loop context {
    if (eval condition == false) return;
    eval context block;
    next = eval context step;
  }
}

//-----------------------------------------------------------------------------


print cut_left (a : int = 1, b : float = 2.0, c : string = "three") (a : int = 4, b : float = 5.0)
  > (c : string = "three")



curry := function f : function<argtype, rettype>, args1 => (function<cut_left(argtype, type(args1)), rettype>)
{
  return function args2 : cut_left(argtype, type(args1)) => rettype {
    return f cons(args1, args2);
  }
}


//-----------------------------------------------------------------------------

meta := function args, code : block<int, int> => block<int, nil> 
{
}

print (eval (eval block 10))



//-----------------------------------------------------------------------------

objective_c_style := function firstarg 'somestring' secondarg => nil
{
}


objective_c_style 10 somestring 20;

//-----------------------------------------------------------------------------

counter := generator(n : int, start : int = 0) => int
{
  if (start == n) return;
  yield start;
  repeat n, start + 1;
}


print_generator := function count : generator<int> {
  loop () {
    if (count.done) return;
    print count();
    repeat;
  }
}

main := function {
  my_counter := counter 10;
  print_generator my_counter;
}

fibonacci := generator(a : int = 1, b : int = 0) => int
{
  yield a;
  repeat a + b, a;
}



first_fib_greater_than_1000 := function(a : int 1, b : int = 0) => int
{
  if (a > 1000) return a;
  repeat a + b, a;
}


mandelbrot := function (cr : double, ci : double,
                        r = 0.0, i = 0.0,
                        rep = 0, max = 50) => int
{
  if (rep == max || (r * r + i * i > 4)) return rep;
  r2 = 2.0 * r * i + ci;
  i2 = r * r - i * i + cr
  repeat (cr, ci, r2, i2, rep + 1);
}

//-----------------------------------------------------------------------------
// list comprehensions

S = [x**2 for x in range(10)]

S = array (x, 10) { x**2; }
V = array (i, 13) { 2**i; }

// array specialized for generator types?
my_counter = counter(10);
S = array my_counter;



// generator that returns lines in a file

getline := function(handle : FILE*) => (ok : bool = false, line : string = nil) {
  // array concats the yielded values and propagates the generator's terminator
  // to produce (array<char>, error)
  (line, error) := array() {
    (c, error) = getc(handle);
    if (error) return error;
    yield c;
    // how does a return in an if() clause distinguish between the current block and
    // the parent block?
    /*
    if (c == '\n') {
      return;
    } else {
      repeat;
    }
    */

    // predicated expression?
    c == '\n' ? return;
    repeat;
  }
};


lines := function(filename : string) => generator<string> {
  handle := fopen(filename, "r");
  return generator(handle : FILE* = handle) {
    (ok, line) := getline(handle);
    if (!ok) return;
    yield line;
    repeat;
  }
}



(define (leap-year? year)
 (or (and (zero? (remainder year 4))
 (not (zero? (remainder year 100))))
 (zero? (remainder year 400))))

leap-year := function year => bool
{
  if (year % 400 == 0) return true;
  if (year % 100 == 0) return false;
  return (year % 4 == 0);
}



if := function condition 'then' block
{
}

if := function condition 'then' block1 'else' block2
{
}

add := function a @to b
{
  return a + b;
}

add := function (a : int) (? : symbol = 'to') (b : int)
{
  return a + b;
}

x := add 10 to 16;

=> x := add (? : int = 10) (? : symbol = 'to') (? : int = 16)


unresolved identifiers produce (? : symbol = 'to') atoms which participate
in function lookup.

just pick some unused prefix character for the shorthand identifier syntax
@to => (? : symbol = 'to')


map
reduce
foldr
foldl


foldr := function<type> (data : array<type>, f : function<(type, type), type>, accum : type = 0, index : int = 0)
{
  if (index == data.length) return accum;
  repeat f(accum, data[index]), index + 1;
}

foldl := function<type1, type2> (data : array<type>, f : function<(type2, type1), type2>, accum : type2 = 0, index : int = 0)
{
  if (index == data.length) return accum;
  repeat accum = f(accum, data[data.length - index - 1]), index = index + 1;
}

// transform releases the source array
my_array = [1, 2, 3];
new_array = transform (my_array, x) { return x**2; }
// my_array is now dead, its storage has been moved to new_array


//-----------------------------------------------------------------------------
// loops are referenced by the names associated with them?

blah = function(x : int, y : int) => int
{
  outer := range(i = 0, max = x, sum1 = 0) {
    inner := range(j = 0, max = y, sum2 = 0) {
      if (j & 1) {
        repeat sum2 + 10;
      } else {
        repeat sum2 + 20;
      }
    }
    repeat sum1 + inner;
  }
  return outer;
}

int blah(int x, int y) {
  int sum;
  for (int i = 0; i < x; i++) {
    int sum2;
    for(int j = 0; j < x; j++) {
      sum2 += j;
      if (j > 10) {
        sum2 = 0;
        break;
      }
    }
    sum += sum2;
  }
}

//-----------------------------------------------------------------------------
// establishing a context? defining a context requirement for a function?

bool Mammal::nap(int length) {
}

nap := function this : Mammal :: (length : number) => (stillSleepy : boolean, stinky : boolean)
{
  this.yawn();
  wait(length);
  this.sleepDebt -= length;
  this.stretch();
  return stillSleepy = this.sleepDebt > 1, stinky = length > 12;
}

dance := function Austin : Human, Joanna : Human
         :: danceName : string
         => StepsTaken : number
{
  if (danceName == "samba") {
    Austin.moveRight();
    Joanna.moveLeft();
    return StepsTaken = 2;
  } else {
    Joanna.jump();
    Austin.shimmy();
    Joanna.spin();
    return StepsTaken = 3;
  }
}

main := function()
{
  Austin = new Human();
  Joanna = new Human();
  (Austin, Joanna) dance "Samba";
  Austin.nap(5);
  result = Joanna.nap(3);
  if (result.stinky) {
    Joanna shower;
  }
}

//-----------------------------------------------------------------------------
// How do we express sorting an array of things in a strict-ownership world?

// Distinguish 'function' 'modifier' 'mutator'
function - no side effects.
modifier - inputs are read-only, outputs are write-only, aliasing is forbidden (somehow).
mutator - read/write state.

//-----------------------------------------------------------------------------

function types consist of
  name
  context tuple
  (delimiter?)
  arg tuple
    support 'hard' strings for "add X to Y" syntax?
  (delimiter?)
  result tuple
  (delimiter?)
  block

//-----------------------------------------------------------------------------
automatic delegation?


