/*
 *   File "lmarip.h"
 *   Modular Ariphmetic of Long Integers, Definitions
 *
 *   Implements the operations in a quotent ring Zm = Z/mZ,
 *   where m is an integer number of arbitrary length.
 *
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 */
#ifndef LMARIPH_H
#define LMARIPH_H

#include "lariph.h"

extern void lminit(int nd); /* Get memory for work variables <num.of digits> */
extern void lmterm();       /* Release memory */

extern void lmadd(      /* c:=(a+b)(mod m) */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
);
extern void lmsub(      /* c:=(a-b)(mod m) */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
);
extern void lmmul(      /* c:=(a*b)(mod m) */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
);
extern void lmshl(      /* (a *= 2)(mod m) */
    long_number *a, const long_number *m
);
extern void lmshr(      /* (a /= 2)(mod m) */
    long_number *a, const long_number *m
);
extern int lmdiv(      /* Division (mod m) returns true if b is invertible */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
);
extern int lminv(       /* c := (a)^(-1)(mod m) returns true if invertible */
    const long_number *a, const long_number *m,
    long_number *c
);
extern void lmshl_n(    /* Mult. by 2**n (mod m)*/
    long_number *a, int n,
    const long_number *m
);
extern void lmshr_n(    /* Div. by 2**n (mod m) */
    long_number *a, int n,
    const long_number *m
);
extern void lmpow(      /* (a^(int.)n)(mod m) */
    const long_number *a, int n,
    const long_number *m, long_number *c
);
extern void lmlpow(     /* (a^ long n)(mod m) */
    const long_number *a, const long_number *n,
    const long_number *m, long_number *c
);

#endif /* LMARIPH_H */
