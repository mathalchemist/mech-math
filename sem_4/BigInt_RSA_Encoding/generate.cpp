// =====================================
// Veeeeeery biiiiig number generator :-)
// Needed For Rumley-Shamir-Aldeman encoding
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "BigInt.h"

void prn(BigInt *x, FILE *f) {
  int l = x->numDecimalDigits() + 16;
  char *s = new char[l + 1];
  x->toString(s, l);
  fprintf(f, "%s\n", s);
  delete s;
}

int main() {
  srand(time(0));

  BigInt One(1);
  BigInt p;
  BigInt q;
  FILE *f = fopen("numbers.txt", "w+");
  int k = 16;
  int b = k * 8 + 2;

  p.generateRandomPrime(b, 100);
  q.generateRandomPrime(b, 100);
  prn(&p, f);
  prn(&q, f);
  BigInt m = p * q;
  prn(&m, f);

  BigInt fi((p - One) * (q - One));

  BigInt e;
  int bits = fi.numBits() / 2;
  e.generateRandomPrime(bits, 100);

  prn(&fi, f);
  prn(&e, f);
  bool res;
  BigInt d = e.inverseMod(fi, &res);
  if (!res) {
    printf("bad number E!!!\n");
    return 0;
  }
  d = d % fi;
  prn(&d, f);
  fclose(f);

  return 0;
}
