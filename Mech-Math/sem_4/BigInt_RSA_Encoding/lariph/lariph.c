/*
 *   File "lariph.c"
 *   Ariphmetic of Long Integers, Implementation
 *
 *   Implements the operations with integers of arbitrary length.
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 *
 *   Part 1 of 4: Operations +, -, *, comparings, etc.
 */
#include <stdio.h>
#include <string.h>
#include "lariph.h"

/* Local functions */
static void lacompl(int nd, byte2 a[]); /* Complement */

void laadd(                             /* Addition */
    int nd1, int nd2, int *nd3,
    int s1,  int s2,  int *s3,
    const byte2 a1[], const byte2 a2[], byte2 a3[]
) {
    int i;
    int ndmax, c, s;
    byte4 sum, d1, d2;
    const byte2 *a;
    int nd, n3;

    int nd33, s33;
    byte2 *a33;
    int copy = 0;

    if (a1 == a3 || a2 == a3) {
        lainit(nd1+nd2+2);
        a33 = la_workmem+1;
        lacopy(*nd3, &nd33, *s3, &s33, a3, a33);
        copy = 1;
    } else {
        a33 = a3;
    }

    ndmax = (nd1 >= nd2? nd1 : nd2);
    if (s1 < 0) {                                   /* a1 < 0 => */
        lacopy(nd1, &nd33, s1, &s33, a1, a33);      /* a3 := a1  */
        for (i = nd1; i <= ndmax; i++) a33[i]=0;
        nd = nd2; a = a2;   /* Second argument */   /* a := a2   */
        s = s2;
    } else {                                        /* a1 >= 0 => */
        lacopy(nd2, &nd33, s2, &s33, a2, a33);      /* a3 := a2   */
        for (i = nd2; i <= ndmax; i++) a33[i]=0;
        nd = nd1; a = a1;   /* Second argument */   /* a := a1   */
        s = s1;
    }
    if (s33 < 0 && s > 0) {                         /* a3 < 0 && a1 > 0 => */
        lacompl(ndmax+1, a33);                      /* complement a3       */
        s33 = 1;

        /*
        printf("\nComplement representation:");
        for (i=ndmax; i>=0; --i) printf(" %u", a33[i]);
        */
    }
    /*
     * Assertion: (a3 >= 0 && a >= 0) &&
     *            (a3 < 0  && a <  0) **
     *            (a3 + a == a1 + a2)
     */
    n3 = 0;       /* Number of digits in the sum */
    c = 0;
    for (i = 0; i <= ndmax; i++) { /* a3 := a3 + a */
        d1 = a33[i];
        d2 = i < nd? a[i] : 0;

        /*
        printf("\nComputing %ld + %ld", d1, d2);
        */

        sum = d1 + d2 + c;
        if ((a33[i] = (byte2) sum) != 0)
            n3 = i+1; /* Count a number of digits */
        /*
        c = 0;
        if (sum >= LA_BASE) c = 1;
        */
        c = LA_UPPER_DIGIT(sum);

        /*
        printf("\nsum = %ld, c = %d, a3[%d] = %u", sum, c, i, a3[i]);
        */
    }

    if (n3 == 0) {                          /* Zero sum */
        s33 = 1; nd33 = 1;
    } else if (a33[ndmax] >1 && c==0) {     /* Negative sum */
        s33 = (-1);
        lacompl(ndmax+1, a33);
        i = ndmax+1;
        while (--i >= 0 && a33[i] == 0);    /* Count a number of digits */
        nd33 = i+1;
    } else {                                /* Positive sum or             */
        nd33 = n3;                          /* sum of two negative numbers */
    }

    /* Copy result from work memory */
    *nd3 = nd33; *s3 = s33;
    if (copy) 
        lacopy(nd33, nd3, s33, s3, a33, a3);

#   ifdef DEBUG_PRINT
    printf("\nAddition:");                               /*+++*/
    for (i=*nd3-1; i>=0; --i) printf(" %u", a3[i]);      /*+++*/
#   endif

}

void lasub(                     /* Subtraction: a3:= a1-a2 */
    int nd1, int nd2, int *nd3,
    int s1,  int s2,  int *s3,
    const byte2 a1[], const byte2 a2[], byte2 a3[]
) {

# ifdef DEBUG_PRINT
  printf("\nSubtraction:");                            /*+++*/
# endif

    laadd(nd1, nd2, nd3, s1, (-s2), s3, a1, a2, a3);
}

void lacopy(                            /* Copy numbers a2 := a1 */
    int nd1, int *nd2, int s1, int *s2,
    const byte2 a1[], byte2 a2[]
) {
    const byte2 *p; byte2 *q;
    *nd2 = nd1; *s2 = s1;
    memmove(a2, a1, nd1 * sizeof(byte2));
}

static void lacompl(int nd, byte2 *a) { /* Complement */
    int c;
    byte4 sum;
    c = 1;
    while (--nd >= 0) {
        sum = LA_BASE-1;
        sum -= *a;
        sum += c;
        *a++ = (byte2)sum;
        /* c = (sum >= LA_BASE); */
        c = LA_UPPER_DIGIT(sum);
    }
}

int landig(     /* Real number of macro digits */
    int nd, int s, const byte2 a[]
) {
    int i = nd - 1;
    while (i >= 0 && a[i] == 0)
        --i;
    return (i + 1);
}

int lacmp(      /* Comparing: returns -1, 0, 1 */
    int nd1, int nd2,
    int s1,  int s2,
    const byte2 a1[], const byte2 a2[]
) {
    int na, nb;
    int s;

    if (s1 < 0 && s2 > 0) 
        return (-1);
    if (s1 > 0 && s2 < 0) 
        return 1;
    na = landig(nd1, s1, a1);
    if (na == 0)
        na = 1;
    nb = landig(nd2, s2, a2);
    if (nb == 0)
        nb = 1;

    if (s1 > 0) {
        if (s2 < 0)
            return 1;
        /* Both positive */
        s = 1;
    } else {
        if (s2 > 0)
            return (-1);
        /* Both negative */
        s = (-1);
    }

    if (na > nb) 
        return s;
    else if (na < nb) 
        return (-s);

    /* Same number of digits, same sign of both numbers */

    /* Skip equal digits */
    --na;       /* Index of leading digit */
    while (na > 0 && a1[na] == a2[na])
        --na;

    if (a1[na] > a2[na]) 
        return s;
    else if (a1[na] == a2[na]) 
        return 0;
    else 
        return (-s);
}

void lnadd(                     /* c := a+b */
    const long_number *a, const long_number *b, long_number *c
) {
    laadd(
        a->number_of_digits, b->number_of_digits, &(c->number_of_digits),
        a->sign, b->sign, &(c->sign),
        a->digits, b->digits, c->digits
    );
}

void lnsub(                     /* c := a-b */
    const long_number *a, const long_number *b, long_number *c
) {
    lasub(
        a->number_of_digits, b->number_of_digits, &(c->number_of_digits),
        a->sign, b->sign, &(c->sign),
        a->digits, b->digits, c->digits
    );
}

void lnmul(                     /* c := a*b */
    const long_number *a, const long_number *b, long_number *c
) {
    lamul(
        a->number_of_digits, b->number_of_digits, &(c->number_of_digits),
        a->sign, b->sign, &(c->sign),
        a->digits, b->digits, c->digits
    );
}

void lndiv(                     /* Division */
    const long_number *a, const long_number *b,
    long_number *c, long_number *d
) {
    int* c_ndig = 0;
    int* c_sign = 0;
    byte2* c_digits = 0;
    int* d_ndig = 0;
    int* d_sign = 0;
    byte2* d_digits = 0;

    if (c != 0) {
        c_ndig = &(c->number_of_digits);
        c_sign = &(c->sign);
        c_digits = c->digits;
    }
    if (d != 0) {
        d_ndig = &(d->number_of_digits);
        d_sign = &(d->sign);
        d_digits = d->digits;
    }

    ladiv(
        a->number_of_digits,    b->number_of_digits,
        c_ndig, d_ndig,
        a->sign, b->sign, c_sign, d_sign,
        a->digits, b->digits, c_digits, d_digits
    );
}

int lncmp(      /* Comparing: returns -1, 0, 1 */
    const long_number *a, const long_number *b
) {
    return lacmp(
        a->number_of_digits, b->number_of_digits,
        a->sign, b->sign,
        a->digits, b->digits
    );
}

void lncopy(
    const long_number *from, long_number *to
) {
    lacopy(
         from->number_of_digits, &(to->number_of_digits),
         from->sign, &(to->sign),
         from->digits, to->digits
    );
}

void lnnegate(long_number *a) {         /* a = (-a) */
    a->sign = (-(a->sign));
}
