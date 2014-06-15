//-----------------------------------------------------------------------------
/* The Computer Language Benchmarks Game
 * http://benchmarksgame.alioth.debian.org/
 *
 * Contributed by Sebastien Loisel
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double eval_A(int i, int j) {
  return 1.0 / ((i + j) * (i + j + 1) / 2 + i + 1);
}

void eval_A_times_u(int N, const double u[], double Au[]) {
  for (int i=0; i < N; i++) {
    Au[i] = 0;
    for (int j = 0; j < N; j++) {
      Au[i] += eval_A(i, j) * u[j];
    }
  }
}

void eval_At_times_u(int N, const double u[], double Au[]) {
  for (int i = 0; i < N; i++) {
    Au[i] = 0;
    for (int j = 0; j < N; j++) {
      Au[i] += eval_A(j, i) * u[j];
    }
  }
}

void eval_AtA_times_u(int N, const double u[], double AtAu[]) {
  double v[N];
  eval_A_times_u(N,u,v);
  eval_At_times_u(N,v,AtAu);
}

int main(int argc, char *argv[])
{
  const int N = ((argc == 2) ? atoi(argv[1]) : 2000);

  double u[N];
  double v[N];
  for (int i = 0; i < N; i++) {
    u[i] = 1;
  }

  for (int i = 0; i < 10; i++) {
    eval_AtA_times_u(N,u,v);
    eval_AtA_times_u(N,v,u);
  }

  double vBv = 0;
  double vv = 0;
  for (int i = 0; i < N; i++) {
    vBv += u[i] * v[i];
    vv += v[i] * v[i];
  }
  printf("%0.9f\n", sqrt(vBv / vv));
  return 0;
}

//-----------------------------------------------------------------------------

// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
//
// contributed by Ian Osgood
// modified by Isaac Gouy

A := function(i : double, j : double) : double
{
  result = 1/((i+j)*(i+j+1)/2+i+1);
}

Au := function(u : in array(double),
               v : out array(double))
{
  repeat (i : int, len(u)) {
  }

  loop (i := 0) {
    /*
    t : double = 0;
    for j in range(u) {
      t += A(i,j) * u[j];
    }
    v[i] = t;
    */
    v[i] = (sum) :: loop (j := 0, sum := 0.0) {
      if (j == len(u)) {
        next := break;
      } else {
        next := (j + 1, sum + { A(i,j) * u[j]; });
      }
    }
    next := (i < len(u)) ? i + 1 : break;
  }
}


ASDFASDFASDFASDF

expansion and stuff. value expressions on the left are evaluated in the context from the right
a := (=x, =x*x, =x*x*x, =sqrt(x*x + y*y)) :: (x := 10.2, y := 11.1, z := 13.4);

ASDFASDFASDFASDF



Atu := function(u : array(double),
                v : array(double))
{
  t : double;
  for i in range(u) {
    t = 0;
    for j in range(u) {
      t += A(j,i) * u[j];
    }
    v[i] = t;
  }
}

AtAu := function(u : array(double),
                 v : array(double),
                 w : array(double))
{
  Au(u,w);
  Atu(w,v);
}

spectralnorm := function(n : int) : double {
  u := array(double)(n, 1);
  v := array(double(n, 0);
  w := array(double)(n, 0);

  for i in range(10) {
    AtAu(u,v,w);
    AtAu(v,u,w);
  }

  vv := 0.;
  vBv := 0.;

  for i in range(n) {
    vBv += u[i]*v[i];
    vv  += v[i]*v[i];
  }

  return sqrt(vBv/vv);
}

print(spectralnorm(+arguments[0]).toFixed(9));