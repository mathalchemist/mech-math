/*
 *   File "lmtest.c"
 *   Modular Ariphmetic of Long Integers
 *
 *   Implements the operations in a quotent ring Zm = Z/mZ,
 *   where m is an integer number of arbitrary length.
 *
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 *
 *   The simple test
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lariph.h"
#include "lmariph.h"

char texta[1000], textb[1000], textc[1000], textd[1000],  textm[1000];
int lena, lenb, lenc, lend;

int main() {
    int na, nb, nc, nd, sa, sb, sc, sd;
    int i, j, n;
    long_number a, b, c, m;
    byte2 K[1];
    char s;

    lminit(100);

    if (
        (a.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL ||
        (b.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL ||
        (c.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL ||
        (m.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL
    ) {
        perror("Cannot get memory for digits");
        exit(1);
    }

    Lbeg: ;

    /*
    printf("\nNumber of digits in the first number: ");
    scanf("%d", &(a.number_of_digits));
    if (a.number_of_digits == 0) {
        laterm();
        return 0;
    }
    printf("Sign of the first number (+1/-1): ");
    scanf("%d", &(a.sign));
    printf("Give digits of the first number: ");
    for (i = a.number_of_digits-1; i >= 0; --i) {
        scanf("%ud", &(a.digits[i]));
    }
    */

    printf("\nInput first number>");
    scanf("%s", texta);
    if (!isdigit(*texta) && *texta != '+' && *texta != '-')
        return 0;
    lnatoi(texta, strlen(texta), &a);

    /*
    printf("\nNumber of digits in the second number: ");
    scanf("%d", &(b.number_of_digits));
    printf("Sign of the second number (+1/-1): ");
    scanf("%d", &(b.sign));
    printf("Give digits of the second number: ");
    for (i = b.number_of_digits-1; i >= 0; --i) {
        scanf("%ud", &(b.digits[i]));
    }
    */

    printf("Input second number>");
    scanf("%s", textb);
    lnatoi(textb, strlen(textb), &b);

    printf("Input modul        >");
    scanf("%s", textm);
    lnatoi(textm, strlen(textm), &m);

    Lrepeat: ;
    printf("\nOperation? (+, -, *, /, ^ ): ");
    scanf("\n%c", &s);

    if (s == '+')
        lmadd(&a, &b, &m, &c);
    else if (s == '-')
        lmsub(&a, &b, &m, &c);
    else if (s == '*')
        lmmul(&a, &b, &m, &c);
    else if (s == '/')
        lmdiv(&a, &b, &m, &c);
    else if (s == '^') {
        lmlpow(&a, &b, &m, &c);
    }
    else goto Lrepeat;

    /*
    printf("\nResult = %c", c.sign > 0? '+' : '-');
    for (i = c.number_of_digits-1; i >= 0; --i) {
        printf(" %u", c.digits[i]);
    }
    */

    lnitoa(&c, 1000, textc, &lenc);
    printf("\nResult: ");
    printf(textc);
    printf("\n");

    goto Lbeg;
}
