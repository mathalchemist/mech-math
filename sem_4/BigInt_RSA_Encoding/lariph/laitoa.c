/*
 *   File "laitoa.c"
 *   Ariphmetic of Long Integers, Implementation
 *
 *   Implements the operations with integers of arbitrary length.
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 *
 *   Part 4 of 4: Convertion long number <--> text
 */
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "lariph.h"

static const byte2 decbase[2] = {LA_DEC_POWER, 0};
static const long_number dec_base = {1, 1, (byte2 *)decbase}; /* 10000 */

void laitoa(/* Convertion to text in decimal system */
            int nd, int s, const byte2 d[], int maxlen, char *text,
            int *length) {
  long_number number, res;
  byte2 res_digits[4];
  register byte2 tetrada;
  register char *t;
  char *p;
  register int i;

  number.digits = (byte2 *)malloc((nd + 2) * sizeof(byte2));
  res.digits = res_digits;

  lacopy(nd, &(number.number_of_digits), s, &(number.sign), d, number.digits);

  t = text;
  if (number.number_of_digits <= 1 && number.digits[0] == 0) {
    if ((t - text) < (maxlen - 1))
      *t++ = '0';
  } else {
    while (number.number_of_digits > 1 || number.digits[0] != 0) {
      lndiv(&number, &dec_base, &number, &res);
      tetrada = res.digits[0];
      for (i = 0; i < 4; i++) {
        if ((t - text) < (maxlen - 1))
          *t++ = tetrada % 10 + '0';
        tetrada /= 10;
      }
    }
  }

  while (t > text && *(--t) == '0')
    ; /* Delete leading zeros */
  t++;

  if (s < 0) {
    if ((t - text) < (maxlen - 1))
      *t++ = '-';
  }
  *t = 0;
  *length = t - text;
  /* Invert a text */
  for (--t, p = text; p < t;) {
    register char c;
    c = *p;
    *p++ = *t;
    *t-- = c;
  }
  free(number.digits);
}

void laatoi(/* Convertion text => long number */
            const char *text, int textlen, int *nd, int *s, byte2 digits[]) {
  byte2 coeff[2];
  byte2 tetrada;
  const char *t;
  const char *p;
  int i;
  int n, sign;

  t = text;
  while ((t - text) < textlen && *t != 0 && isspace(*t))
    t++;

  sign = 1;
  if ((t - text) < textlen && *t != 0 && *t == '-')
    sign = (-1);

  if ((t - text) < textlen && *t != 0 && (*t == '-' || *t == '+'))
    t++;
  while ((t - text) < textlen && *t != 0 && isspace(*t))
    t++;

  p = t;
  while ((p - text) < textlen && isdigit(*p))
    p++;
  n = (p - t) % 4;
  if (n == 0)
    n = 4;

  *nd = 1;
  digits[0] = 0;

  while (t < p) {
    tetrada = 0;
    for (i = n; --i >= 0;) {
      tetrada *= 10;
      tetrada += (*t++ - '0');
    }
    n = 4;

    coeff[0] = tetrada;
    lamul(*nd, 1, nd, *s, 1, s, digits, decbase, digits);
    laadd(*nd, 1, nd, *s, 1, s, digits, coeff, digits);
  }
  *s = sign;
}

void lnatoi(/* Convertion text => long number */
            const char *text, int textlen, long_number *n) {
  laatoi(text, textlen, &(n->number_of_digits), &(n->sign), n->digits);
}

void lnitoa(/* Convertion to text in decimal system */
            const long_number *n, int maxlen, char *text, int *length) {
  laitoa(n->number_of_digits, n->sign, n->digits, maxlen, text, length);
}

int lnndig(/* Real number of macro digits */
           const long_number *n) {
  return landig(n->number_of_digits, n->sign, n->digits);
}

int lnndig2(/* Number of binary digits */
            const long_number *n) {
  return landig2(n->number_of_digits, n->sign, n->digits);
}

int lnndig10(/* Number of binary digits */
             const long_number *n) {
  return landig10(n->number_of_digits, n->sign, n->digits);
}

int lntextl(/* Approximate length of text representation (upper bound) */
            const long_number *n /* including the terminating zero character */
            ) {
  return latextl(n->number_of_digits, n->sign, n->digits);
}

static double log2_10 = 0.;

int landig2(/* Number of binary digits */
            int nd, int s, const byte2 d[]) {
  int k, n;
  int hidigit, bit;

  /* Compute the real number of byte2-digits */
  k = landig(nd, s, d);
  if (k == 0) {
    return 1; /* Zero */
  }

  hidigit = d[k - 1];
  n = LA_SIZE_OF_DIGIT * (k - 1); /* Size of all digits except the high */
  bit = 1;
  while (bit <= hidigit) {
    bit <<= 1;
    n++;
  }
  return n;
}

int landig10(/* Approximate number of decimal digits, upper bound */
             int nd, int s, const byte2 d[]) {
  int ndig2 = landig2(nd, s, d);

  if (log2_10 == 0.)
    log2_10 = 1. / log10(2.);

  return (int)((double)(ndig2 + 1) / log2_10 + 0.7);
}

/* Text representation -> approx. number of macro digits (upper bound) */
int laa2nd(int len, const char *text) {
  int i, n, n2;

  i = 0;
  while (i < len && isspace(text[i]))
    i++;
  if (i < len && (text[i] == '-' || text[i] == '+'))
    i++;
  while (i < len && isspace(text[i]))
    i++;

  n = 0;
  while (isdigit(text[i])) {
    i++;
    n++;
  }

  return lalen2nd(n);
}

int lalen2nd(/* Number of decimal digits -> number of macro digits */
             int len) {
  int n, n2;

  /* Upper bound of a number is N = 10^len  */
  /* Number of binary digits = log_2(N) + 1= len * log_2(10) + 1 */

  if (log2_10 == 0.)
    log2_10 = 1. / log10(2.);

  /* Number of binary digits */
  n2 = (int)((double)len * log2_10 + 1.);
  n = n2 / LA_SIZE_OF_DIGIT;
  if (n2 % LA_SIZE_OF_DIGIT != 0)
    n++;
  return n;
}

int latextl(                               /* Length of text representation  */
            int nd, int s, const byte2 d[] /* including the terminating zero */
            ) {
  int len = landig10(nd, s, d) + 1;
  if (s < 0) /* Negative sigh */
    len++;
  return len;
}
