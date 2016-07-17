/*
 *   File "lamult.c"
 *   Ariphmetic of Long Integers, Implementation
 *
 *   Implements the operations with integers of arbitrary length.
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 *
 *   Part 2 of 4: Multiplication, fast power
 */
#include <stdlib.h>
#include <stdio.h>
#include "lariph.h"

static const byte2 d_zero[] = {0};
static const byte2 d_one[]  = {1};
const long_number LN_ZERO = {1, 1, (byte2*) d_zero};
const long_number LN_ONE  = {1, 1, (byte2*) d_one };
const long_number LN_MINUS_ONE = {1, -1, (byte2*) d_one };

void lamul(                     /* Multiplication */
    int nd1, int nd2, int *nd3,
    int s1,  int s2,  int *s3,
    const byte2 a1[], const byte2 a2[], byte2 a3[]
) {
    byte4 t;
    byte2 k;
    int i,j;
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

    nd33 = nd2 + nd1;
    for(j = 0; j < nd2+nd1; j++) 
        a33[j] = 0;
    for (j = 0; j <= nd2 - 1; j++) {
        if (a2[j]) {
            k = 0;
            for (i = 0; i <= nd1 - 1; i++) {
                t = (byte4)a1[i]*a2[j] + a33[i+j] + k;
                a33[i+j] = LA_LOWER_DIGIT(t);
                /*+++
                printf("\na3[%d] = %d ",i+j,a33[i+j]);
                +++*/
                k = LA_UPPER_DIGIT(t);
                /*+++
                printf("k = %u", k);
                +++*/
            }
            a33[nd1+j] += k;
        }
    }
    while (nd33 > 1 && a33[nd33-1] == 0) 
        --nd33;
    s33 = s1*s2;
    if (nd33 == 1 && a33[0] == 0) 
        s33 = 1;

    /* Copy result from work memory */
    *nd3 = nd33; *s3 = s33;
    if (copy) 
        lacopy(nd33, nd3, s33, s3, a33, a3);

#   ifdef DEBUG_PRINT
    printf("\nMultiplication:");
    for(i = *nd3-1; i >= 0; --i){
        printf(" %u",a3[i]);
    }
#   endif
}

void lnpow(             /* c := (a^n) */
    const long_number *a, int n, long_number *c
){
    long_number f;
    int k;

    /* Get memory */
    f.digits = (byte2 *) malloc(
        ((a->number_of_digits + 1) * n + 2) * sizeof(byte2)
    );
    if (f.digits == (byte2*) 0) {
        perror("lnpow: Cannot get memory");
        exit(1);
    }

    lncopy(&LN_ONE, c);
    lncopy(a, &f);

    k = n;
    while (k != 0) {
        /* Invariant: f^k * c = const */
        if ((k & 1) != 0) {
            /* Odd k */
            lnmul(c, &f, c);    /* c *= f */
            k--;
        } else {
            /* Even k */
            lnmul(&f, &f, &f);  /* f *= f */
            k /= 2;
        }
    }

    /* Release memory */
    free(f.digits);
}
