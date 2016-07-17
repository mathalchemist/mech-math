/*
 *   File "latest.c"
 *   Ariphmetic of Long Integers
 *
 *   Implements the operations with integers of arbitrary length.
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 *
 *   The simple test
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lariph.h"

#define TEXT_MAXLEN 1024

char texta[TEXT_MAXLEN], textb[TEXT_MAXLEN], textc[TEXT_MAXLEN], 
    textd[TEXT_MAXLEN], texte[TEXT_MAXLEN];
int lena = 0, lenb = 0, lenc = 0, lend = 0, lene = 0;

int main() {
    int na, nb, nc, nd, sa, sb, sc, sd, se;
    int i, j, n, ndig;
    long_number a, b, c, d, e;
    byte2 K[1];
    char s;

    lainit(100);

    if (
        (a.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL ||
        (b.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL ||
        (c.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL ||
        (d.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL ||
        (e.digits = (byte2*) malloc(200 * sizeof(byte2))) == NULL) {
        perror("Cannot get memory for digits");
        exit(1);
    }

    Lbeg:
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
    printf("\nInput first  number>");
    scanf("%s", texta);
    if (!isdigit(*texta) && *texta != '+' && *texta != '-') 
        return 0;
    printf(
        "Approx. number of macro digits = %d\n",
        laa2nd(strlen(texta), texta)
    );
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
    printf("\nInput second number>");
    scanf("%s", textb);
    lnatoi(textb, strlen(textb), &b);

    Lrepeat:
    printf(
        "\n Operation? (+, -, *, /, ^, #(gcd), e(Ext.Euclid Alg.),\n"
        "            d(num. decimal digits), b(binary digits)): "
    );
    scanf("\n%c", &s);

    if (s == '+')
        lnadd(&a, &b, &c);
    else if (s == '-')
        lnsub(&a, &b, &c);
    else if (s == '*')
        lnmul(&a, &b, &c);
    else if (s == '/'){
        lndiv(&a, &b, &c, &d);}
    else if (s == '^') {
        if (b.number_of_digits != 1 ||
            b.sign < 0) {
            printf ("\nSorry, long exponent is illegal");

            goto Lrepeat;
        }
        n = b.digits[0];
        lnpow(&a, n, &c);
    }
    else if (s == '#')
        lngcd(&a, &b, &c);
    else if (s == 'e')
        lneuc(&a, &b, &c, &d, &e);
    else if (s == 'd')
        ndig = lnndig10(&a);
    else if (s == 'b')
        ndig = lnndig2(&a);
    else goto Lrepeat;

    /*
    printf("\nResult = %c", c.sign > 0? '+' : '-');
    for (i = c.number_of_digits-1; i >= 0; --i) {
        printf(" %u", c.digits[i]);
    }
    if (s == '/') {
        printf("\nResidue = %c", d.sign > 0? '+' : '-');
        for (i = d.number_of_digits-1; i >= 0; --i) {
            printf(" %u", d.digits[i]);
        }
    }

    */
    if (s != 'd' && s != 'b') {
        lnitoa(&c, TEXT_MAXLEN, textc, &lenc);
        printf("\nResult: ");
        if (s != 'e') {
            printf(textc);
        }
    }

    if (s == '/') {
        lnitoa(&d, TEXT_MAXLEN, textd, &lend);
        printf("\nResidue: ");
        printf(textd);
    } else if (s == 'e') {
        lnitoa(&c, TEXT_MAXLEN, textc, &lenc);
        lnitoa(&d, TEXT_MAXLEN, textd, &lend);
        lnitoa(&e, TEXT_MAXLEN, texte, &lene);
        printf("\ngcd = %s\n", texte);
        printf("u = %s\n", textc);
        printf("v = %s", textd);
    } else if (s == 'd') {
        printf("Approx. number of decimal digits = %d", ndig);
    } else if (s == 'b') {
        printf("Number of binary digits = %d", ndig);
    }

    printf("\n");

    goto Lbeg;
}
