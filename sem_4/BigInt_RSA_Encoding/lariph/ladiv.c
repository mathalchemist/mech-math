/*
 *   File "ladiv.c"
 *   Ariphmetic of Long Integers, Implementation
 *
 *   Implements the operations with integers of arbitrary length.
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 *
 *   Part 3 of 4: Division, Shifts, Euclid Algorithm, Ext. Euclid Alg.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lariph.h"

/* Work memory */
static int number_size = 0;
byte2 *la_workmem = (byte2 *)0;
static byte2 *p = (byte2 *)0, *a = (byte2 *)0, *c = (byte2 *)0,
             *denominator = (byte2 *)0, *residue = (byte2 *)0,
             *quotient = (byte2 *)0;

/*------------ Work memory for Euclid's algorithm -------*/
static long_number u2, v2, y, q, r, t;
static int eu_ndigits = 0;
static byte2 *eu_workmem = (byte2 *)0;

static void eu_init(int nd);

void lainit(int number_of_digits) {
  byte2 *oldmem;
  size_t oldp, olda, oldc, oldresidue, olddenominator, oldquotient;
  size_t size, offset;
  size_t old_size = number_size;

  if (number_of_digits <= 0)
    number_of_digits = LA_MIN_DIGITS;
  if (number_size < number_of_digits + 4) {
    number_size = max(number_of_digits + 4, number_size + LA_MIN_DIGITS);
    /* Get memory */
    oldmem = la_workmem;
    la_workmem = (byte2 *)realloc(la_workmem, sizeof(byte2) * number_size * 6);
    if (la_workmem == NULL) {
      perror("Cannot get memory for temporary arrays");
      exit(1);
    }
    oldp = p - oldmem;
    olda = a - oldmem;
    oldc = c - oldmem;
    olddenominator = denominator - oldmem;
    oldresidue = residue - oldmem;
    oldquotient = quotient - oldmem;

    p = la_workmem;
    a = p + number_size;
    c = a + number_size;
    denominator = c + number_size;
    residue = denominator + number_size;
    quotient = residue + number_size;

    size = sizeof(byte2) * old_size;
    if (size > 0) {
      memmove(quotient, la_workmem + oldquotient, size);
      memmove(residue, la_workmem + oldresidue, size);
      memmove(denominator, la_workmem + olddenominator, size);
      memmove(c, la_workmem + oldc, size);
      memmove(a, la_workmem + olda, size);
      memmove(p, la_workmem + oldp, size);
    }
  }
}

void laterm() {
  if (la_workmem != (byte2 *)0)
    free(la_workmem);
  number_size = 0;
  if (eu_workmem != (byte2 *)0)
    free(eu_workmem);
  eu_ndigits = 0;
}

void ladiv(/* Division */
           int nd1, int nd2, int *nd3, int *nd4, int s1, int s2, int *s3,
           int *s4, const byte2 a1[], const byte2 a2[], byte2 a3[],
           byte2 a4[]) {
  byte2 G[4], D[4], H[4], K[4];
  byte2 dig;
  int g, i, j, l, nG, sG, np, sp, na, sa, nc, sc, n, d, s;
  int deg;
  int nd22, s22, nd33, s33, nd44, s44;

  /* Compute the real number of digits in the second number */
  for (i = nd2 - 1; i >= 0 && a2[i] == 0; --i)
    ;
  if (i < 0) {
    perror("ladiv: zero division");
    exit(1);
  }
  nd2 = i + 1;
  if (nd1 < nd2) {
    if (nd3 != NULL && s3 != NULL && a3 != NULL) {
      *nd3 = 1;
      *s3 = 1;
      a3[0] = 0;
    }
    if (nd4 != NULL && s4 != NULL && a4 != NULL) {
      *nd4 = nd1;
      for (i = 0; i < *nd4; i++)
        a4[i] = a1[i];
      *s4 = s1;
    }
    return;
  }

  int new_size = max(nd1, nd2) + 2;
  if (new_size > number_size) {
    lainit(new_size);
  }

  lacopy(nd2, &nd22, s2, &s22, a2, denominator);
  lacopy(nd1, &nd44, s1, &s44, a1, residue);

  s33 = s44 * s22;

  /*
  deg = 0;
  while(denominator[nd22 - 1] < LA_BASE2) {
      deg++;
      lashl(&nd22,  &s22,  denominator);
      lashl(&nd44, &s44, residue);
  }
  */
  deg = 0;
  dig = LA_BASE2;
  while ((denominator[nd22 - 1] & dig) == 0) {
    dig >>= 1;
    deg++;
  }
  lashl_n(&nd22, &s22, denominator, deg);
  lashl_n(&nd44, &s44, residue, deg);

  nd33 = nd44 - nd22 + 1;
  residue[nd44] = 0;

  for (j = nd33 - 1, l = nd44 - 1; j >= 0; j--, l--) {

    /* 1. Guess the digit of quotient */

    if (residue[l + 1] == denominator[nd22 - 1])
      g = (LA_BASE - 1);
    else {
      g = ((byte4)residue[l + 1] * LA_BASE + (byte4)residue[l]) /
          denominator[nd22 - 1];
    }

#ifdef DEBUG_PRINT
    printf("\n g = %u", g); /*+++++++*/
#endif

    /* 2. Test it: Multiply by two leading digits of denominator */

    if (nd2 > 1) {
      G[2] = residue[l + 1];
      G[1] = residue[l];
      G[0] = residue[l - 1];
      nG = 3;
      H[1] = denominator[nd22 - 1];
      H[0] = denominator[nd22 - 2];

      K[0] = g;
      lamul(1, 2, &n, 1, 1, &s, K, H, D);
      lasub(3, n, &np, 1, 1, &sp, G, D, p);

      while (sp < 0) {
        /* Correct the digit */
        g--;

#ifdef DEBUG_PRINT
        printf("\n Correction 1: g = %u", g); /*+++++++*/
#endif

        lacopy(np, &nG, sp, &sG, p, G);
        laadd(nG, 2, &np, sG, 1, &sp, G, H, p);
      }
    }

    /* 3. Subtract the product of digit and denominator */
    G[0] = g;
    lamul(nd22, 1, &np, 1, 1, &sp, denominator, G, p);

    for (i = 0; i <= nd22; i++)
      a[i] = residue[j + i];
    lasub(nd22 + 1, np, &nc, 1, 1, &sc, a, p, c);
    lacopy(nc, &na, sc, &sa, c, a);

    while (sa < 0) {
      /* Result of subtraction is negative => correct the digit */
      g--;

#ifdef DEBUG_PRINT
      printf("\n Correction 2: g = %u", g); /*+++++++*/
#endif

      laadd(na, nd22, &nc, sa, 1, &sc, a, denominator, c);
      lacopy(nc, &na, sc, &sa, c, a);
    }
    for (i = 0; i < na; i++)
      residue[j + i] = a[i];
    for (; i < nd22; i++)
      residue[j + i] = 0;

    quotient[j] = g;
  }

  nd44 = nd22;
  /*
  while (--deg >= 0) {
      lashr(&nd44, &s44, residue);
      lashr(&nd2,  &s2,  a2);
  }
  */
  lashr_n(&nd44, &s44, residue, deg);
  /*... lashr_n(&nd22, &s2,  denominator, deg); ...not used below ...*/

  /* Number of digits in result */
  for (i = nd33 - 1; i > 0 && quotient[i] == 0; --i)
    ;
  nd33 = i + 1;
  if (i == 0 && quotient[0] == 0)
    s33 = 1; /* Zero */

  /* Number of digits and sign of residue */
  for (i = nd44 - 1; i > 0 && residue[i] == 0; --i)
    ;
  nd44 = i + 1; /*??? s44 = s33; ???*/
  if (i == 0 && residue[0] == 0)
    s44 = 1; /* Zero residue */

  /* Output quotient */
  if (nd3 != NULL && s3 != NULL && a3 != NULL) {
    *nd3 = nd33;
    *s3 = s33;
    for (i = 0; i < nd33; i++)
      a3[i] = quotient[i];
  }

  /* Output residue */
  if (nd4 != NULL && s4 != NULL && a4 != NULL) {
    *nd4 = nd44;
    *s4 = s44;
    for (i = 0; i < nd44; i++)
      a4[i] = residue[i];
  }
}

void lashl(int *nd, int *s, byte2 *a) { /* Mult. by 2 */
  register int c, i, c1;
  c = 0;
  for (i = 0; i < *nd; i++) {
    c1 = ((LA_BASE2)&a[i]) ? 1 : 0;
    a[i] <<= 1;
    a[i] |= c;
    c = c1;
  }
  if (c) {
    a[(*nd)++] = 1;
  }
  /* s is not used */
}

void lashr(int *nd, int *s, byte2 *q) { /* Division by 2 */
  register int c, i, c1;
  c = 0;
  for (i = *nd - 1; i >= 0; --i) {
    c1 = (q[i] & 1) ? LA_BASE2 : 0;
    q[i] >>= 1;
    q[i] |= c;
    c = c1;
  }
  if (q[*nd - 1] == 0) {
    if (*nd > 1)
      --(*nd);
    else { /* Zero */
      *s = 1;
    }
  }
}

void lashl_n(/* Mult. by 2^n */
             int *nd, int *s, byte2 *q, int n) {
  int shift, nzeros;
  byte2 c;
  byte4 d;
  register int i;
  int k;

  if (n == 0)
    return;
  else if (n < 0) {
    lashr_n(nd, s, q, (-n));
    return;
  }

  nzeros = n / LA_SIZE_OF_DIGIT;
  shift = n % LA_SIZE_OF_DIGIT;

  c = 0;
  k = *nd;
  for (i = 0; i < k; i++) {
    d = q[i];
    d <<= shift;
    q[i] = (LA_LOWER_DIGIT(d) | c);
    c = LA_UPPER_DIGIT(d);
  }
  if (c) {
    q[(*nd)++] = c;
  }

  if (nzeros == 0)
    return;
  *nd += nzeros;
  for (i = (*nd - 1); i >= nzeros; --i) {
    q[i] = q[i - nzeros];
  }
  while (i >= 0)
    q[i--] = 0;

  /* s is not used */
}

void lashr_n(/* Division by 2^n */
             int *nd, int *s, byte2 *q, int n) {
  int shift, nzeros;
  byte2 c;
  byte4 d;
  register int i;
  int k;

  if (n == 0)
    return;
  else if (n < 0) {
    lashl_n(nd, s, q, (-n));
    return;
  }

  nzeros = n / LA_SIZE_OF_DIGIT;
  shift = n % LA_SIZE_OF_DIGIT;
  shift = LA_SIZE_OF_DIGIT - shift; /* Replace right shift by the left one */

  k = *nd;
  if (nzeros < k) {
    c = 0;
    for (i = k - 1; i >= 0; --i) {
      d = q[i];
      d <<= shift;
      q[i] = (c | LA_UPPER_DIGIT(d));
      c = LA_LOWER_DIGIT(d);
    }
  }

  if (nzeros == 0)
    return;
  k -= nzeros;
  if (k < 0)
    k = 0;

  for (i = 0; i < k; i++)
    q[i] = q[i + nzeros];
  for (k = *nd; i < k; i++)
    q[i] = 0;

  i = (*nd - 1) - nzeros;
  while (i >= 0 && q[i] == 0)
    --i;
  if (i < 0) {
    *s = 1;
    *nd = 1; /* Zero */
    return;
  }
  *nd = i + 1;
}

void lnshl(long_number *a) { /* a *= 2 */
  lashl(&(a->number_of_digits), &(a->sign), a->digits);
}

void lnshr(long_number *a) { /* a /= 2 */
  lashr(&(a->number_of_digits), &(a->sign), a->digits);
}

void lnshl_n(long_number *a, int n) { /* a *= 2^n */
  lashl_n(&(a->number_of_digits), &(a->sign), a->digits, n);
}

void lnshr_n(long_number *a, int n) { /* a /= 2^n */
  lashr_n(&(a->number_of_digits), &(a->sign), a->digits, n);
}

static void eu_init(int nd) {
  if (eu_ndigits < nd + 4) {
    eu_ndigits = max(nd + 4, eu_ndigits + LA_MIN_DIGITS);

    /* Get memory */
    eu_workmem = (byte2 *)realloc(eu_workmem, sizeof(byte2) * eu_ndigits * 6);
    if (eu_workmem == NULL) {
      perror("eu_init(): Cannot get memory for temporary arrays");
      exit(1);
    }

    u2.digits = eu_workmem;
    v2.digits = u2.digits + eu_ndigits;
    y.digits = v2.digits + eu_ndigits;
    q.digits = y.digits + eu_ndigits;
    r.digits = q.digits + eu_ndigits;
    t.digits = r.digits + eu_ndigits;
  }
}

void lngcd(/* c := gcd(a,b) */
           const long_number *a, const long_number *b, long_number *c) {
  eu_init(max(a->number_of_digits, b->number_of_digits) + 2);
  lncopy(a, c);
  lncopy(b, &y);
  while (y.number_of_digits > 1 || y.digits[0] != 0) {
    lndiv(c, &y, &t, &r);
    lncopy(&y, c);
    lncopy(&r, &y);
  }
}

/* Extended Euclid Algorithm: a*u + b*v = d = gcd(a,b) */
void lneuc(const long_number *a, const long_number *b, long_number *u,
           long_number *v, long_number *d) {
  eu_init(a->number_of_digits + b->number_of_digits + 2);

  lncopy(&LN_ONE, u);
  lncopy(&LN_ZERO, &u2);
  lncopy(&LN_ZERO, v);
  lncopy(&LN_ONE, &v2);
  lncopy(a, d);
  lncopy(b, &y);
  while (y.number_of_digits > 1 || y.digits[0] != 0) {
    lndiv(d, &y, &q, &r);
    lncopy(u, &t);
    lncopy(&u2, u);
    lnmul(&q, &u2, &u2);
    lnsub(&t, &u2, &u2);
    lncopy(v, &t);
    lncopy(&v2, v);
    lnmul(&q, &v2, &v2);
    lnsub(&t, &v2, &v2);
    lncopy(&y, d);
    lncopy(&r, &y);
  }
}
