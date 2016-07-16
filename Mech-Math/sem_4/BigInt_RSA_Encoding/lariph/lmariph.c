/*
 *   File "lmarip.c"
 *   Modular Ariphmetic of Long Integers, Implementation
 *
 *   Implements the operations in a quotent ring Zm = Z/mZ,
 *   where m is an integer number of arbitrary length.
 *
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lariph.h"
#include "lmariph.h"

/* Temporary variables */
static long_number d = {
    0, 1, NULL
};
static long_number f = {
    0, 1, NULL
};
static long_number g = {
    0, 1, NULL
};

static int lm_ndigits = 0;
static byte2 *lm_workmem = NULL;

#define LM_MIN_DIGITS 50

void lminit(int nd) {
    int old_ndigits = lm_ndigits;
    size_t f_old = f.digits - lm_workmem;
    size_t g_old = g.digits - lm_workmem;

    if (nd <= lm_ndigits || nd <= 0) return;
    lm_ndigits = max(nd+4, lm_ndigits + LM_MIN_DIGITS);
    lm_workmem = (byte2*) realloc(lm_workmem, (lm_ndigits+2)*3);
    if (lm_workmem == 0) {
        perror("lminit: Cannot get memory for work variables");
        exit(1);
    }

    g.digits = lm_workmem + 2*lm_ndigits + 1;
    if (old_ndigits != 0)
        memcpy(g.digits, lm_workmem + g_old, sizeof(byte2) * old_ndigits);

    f.digits = lm_workmem + lm_ndigits + 1;
    if (old_ndigits != 0)
        memcpy(f.digits, lm_workmem + f_old, sizeof(byte2) * old_ndigits);

    d.digits = lm_workmem;
}

void lmterm() {
    free(lm_workmem);
    lm_workmem = NULL;
    lm_ndigits = 0;
}

void lmadd(                             /* c := (a+b)(mod m) */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
) {
    lnadd(a, b, c);
    if (lnge(c, m))
        lndiv(c, m, NULL, c);
}

void lmsub(                             /* c := (a-b)(mod m) */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
) {
    lnsub(a, b, c);
    if (c->sign < 0) 
        lndiv(c, m, NULL, c);
}

void lmmul(                             /* c := (a*b)(mod m) */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
) {
    lminit(2 * max(a->number_of_digits + 1, b->number_of_digits + 1));
    lnmul(a, b, &d);
    lndiv(&d, m, NULL, c);
}

void lmpow(                             /* c := (a^n)(mod m) */
    const long_number *a, int n,
    const long_number *m, long_number *c
) {
    int k;

    /* Get memory */
    lminit(2 * max(a->number_of_digits, m->number_of_digits) + 2);

    lncopy(&LN_ONE, c);
    lncopy(a, &f);

    k = n;
    while (k) {
        /* Invariant: f^k * c = const */
        if ((k & 1) != 0) {
            /* Odd k */
            lmmul(c, &f, m, c);     /* c *= f (mod m) */
            k--;
        } else {
            /* Even k */
            lmmul(&f, &f, m, &f);   /* f *= f (mod m) */
            k /= 2;
        }
    }
}

void lmshl(                             /* (a *= 2)(mod m) */
    long_number *a, const long_number *m 
){
    lnshl(a);
    if (lnge(a, m))
        lnsub(a, m, a);
}

void lmshr(                             /* (a /= 2)(mod m) */
    long_number *a, const long_number *m
) {
    if (lneven(a)) {
        Leven:
        lnshr (a);
    } else if (lnodd(m)) {
        lnadd(a, m, a);
        lnshr(a);
    } else {
        /* a odd, m even => incorrect input */
        goto Leven;
    }
}

void lmlpow(                            /* c := (a^n)(mod m) */
    const long_number *a, const long_number *n,
    const long_number *m, long_number *c
) {
    /* Get memory */
    lminit(
        2 * (max(max(a->number_of_digits, m->number_of_digits),
        n->number_of_digits) + 2)
    );

    lncopy(&LN_ONE, c);
    lncopy(a, &f);
    lncopy(n, &g);

    while (g.number_of_digits > 1 || g.digits[0]!= 0) {
        /* Invariant: f^k * c = const */
        if (lnodd(&g)){
            lmmul(c, &f, m, c);     /* c *= f (mod m) */
            lnsub(&g, &LN_ONE, &g); /* g-- */
        } else {
            lmmul(&f, &f, m, &f);   /*  f *= f (mod m) */
            lnshr(&g);              /*  g /= 2 */
        }
    }
}

int lminv(                          /* c := (a)^(-1)(mod m) */
    const long_number *a, const long_number *m,
    long_number *c
) {
    const long_number *mm = m;

    if (lneq(a, &LN_ZERO) || lneq(m, &LN_ZERO)) {
        lncopy(&LN_ZERO, c);    /* Incorrect input */
        return 0;               /* return false */
    }

    lminit(2 * max(a->number_of_digits, m->number_of_digits) + 2);
    lneuc(a, m, &f, &g, &d);
    if (d.sign < 0) {
        lnnegate(&d);
        lnnegate(&f);
    }
    if (lnne(&d, &LN_ONE)) {
        lncopy(&LN_ZERO, c);    /* a is a zero divisor */
        return 0;               /* return false */
    }

    /* We need m to be positive */
    if (m->sign < 0) {
        lncopy(m, &g);
        lnnegate(&g);
        mm = &g;
    }
    if (f.sign >= 0) {
        if (lngt(&f, mm))
            lndiv(&f, mm, NULL, &f);
    } else {
        lnadd(&f, mm, &f);
        if (f.sign < 0) {
            lndiv(&f, mm, NULL, &f);
            lnadd(&f, mm, &f);
        }
    }
    lncopy(&f, c);

    return 1;                   /* return true */
}

int lmdiv(      /* Division (mod m) returns true if b is invertible */
    const long_number *a, const long_number *b,
    const long_number *m, long_number *c
) {
    if (!lminv(b, m, c))
        return 0;
    lmmul(a, c, m, c);
    return 1;                   /* return true */
}
