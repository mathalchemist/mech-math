/*
 *   File "lariph.h"
 *   Ariphmetic of Long Integers, Definitions
 *
 *   Implements the operations with integers of arbitrary length.
 *   Author: Olga Sysoeva, MSU, mech-math dept., 3 course, 1993
 */
#ifndef LARIPH_H
#define LARIPH_H

typedef unsigned short byte2;   /* Digit == 2 bytes */
typedef unsigned int byte4;

#define LA_BASE 65536   /* Base of number system (must be 2^LA_SIZE_OF_DIGIT) */
#define LA_BASE2 (LA_BASE/2)
#define LA_DEC_POWER 10000   /* Maximal power of 10 < B     */
#define LA_DEC_DIGITS 4      /* LA_DEC_POWER = 10 ^ LA_DEC_DIGITS */
#define LA_SIZE_OF_DIGIT 16  /* Number of bits in a digit   */
#define LA_BYTES_IN_DIGIT 2  /* Number of bytes in a digit  */
#define LA_HIGH_BIT 0x8000   /* High bit in a digit         */
#define LA_DIGIT_MASK 0xffff /* Mask for single digit       */

/*--------------------------------------------------
 * Long number is represented by
 * number of digits  (1,2,...)  int nd ,
 * sign      (+1 / -1)          int s  ,
 * array of digits              byte2 a[nd] ,
 * a[i] - digit for B^i .
 * Example:
 *           1234 == 1*B^3 + 2*B^2 + 3*B^1 + 4*B^0
 *           nd = 4,
 *            s = 1
 *           a[0]=4, a[1]=3, a[2]=2, a[3]=1.
 *-------------------------------------------------*/
/*
 *      Representation of long numbers as structures
 *      and corresponding functions:
 */
typedef struct {
    int number_of_digits;
    int sign;
    byte2 *digits;
} long_number;

extern void lnadd(              /* c := a+b */
    const long_number *a, const long_number *b, long_number *c
);
extern void lnsub(              /* c := a-b */
    const long_number *a, const long_number *b, long_number *c
);
extern void lncopy(             /* b := a */
    const long_number *a, long_number *b
);
extern void lnmul(              /* c := a*b */
    const long_number *a, const long_number *b, long_number *c
);
extern void lnnegate(           /* a = (-a) */
    long_number *a
);
extern void lnshl(              /* a *= 2 */
    long_number *a
);
extern void lnshr(              /* a /= 2 */
    long_number *a
);
extern void lnshl_n(            /* Mult. by 2^n */
    long_number *a, int n
);
extern void lnshr_n(            /* Division by 2^n */
    long_number *a, int n
);
extern void lndiv(              /* Division */
    const long_number *a, const long_number *b,
    long_number *quotent, long_number *residue
);
extern void lnitoa(             /* Convertion to text */
    const long_number *n,       /* in decimal system  */
    int maxlen, char *text, int *length
);
extern void lnatoi(                     /* Convertion         */
    const char *text, int textlen,      /* text->long number  */
    long_number *n
);
extern int lnndig(                      /* Real number of macro digits */
    const long_number *n
);
extern int lnndig2(                     /* Number of binary digits */
    const long_number *n
);
extern int lnndig10( /* Approximate number of decimal digits (upper bound) */
    const long_number *n
);
extern int lntextl( /* Approximate length of text representation (upper bound) */
    const long_number *n        /* including the terminating zero character */
);

extern void lnpow(                      /* c := a^n */
    const long_number *a, int  n, long_number *c
);

/* Euclid Algorithm */
extern void lngcd(                      /* c := gcd(a,b) */
    const long_number *a, const long_number *b,
    long_number *c
);

/* Extended Euclid Algorithm */
extern void lneuc(                      /* a*u + b*v = d = gcd(a,b) */
    const long_number *a, const long_number *b,
    long_number *u, long_number *v,
    long_number *d
);

/* Comparing */
extern int lncmp(                       /* Returns -1, 0, 1 */
    const long_number *a, const long_number *b
);

/* Conditions */
#ifndef __cplusplus
/*  Definitions for C-language */
#   define lnge(a,b) (lncmp((a),(b)) >= 0)
#   define lngt(a,b) (lncmp((a),(b)) >  0)
#   define lnle(a,b) (lncmp((a),(b)) <= 0)
#   define lnlt(a,b) (lncmp((a),(b)) <  0)
#   define lneq(a,b) (lncmp((a),(b)) == 0)
#   define lnne(a,b) (lncmp((a),(b)) != 0)
#   define lneven(a) (((a)->digits[0] & 1) == 0)
#   define lnodd(a)  (((a)->digits[0] & 1) != 0)
#else
/*  Definitions for C++ language */
inline int lnge(const long_number *a, const long_number *b) {
    return (lncmp(a, b) >= 0);
}
inline int lngt(const long_number *a, const long_number *b) {
    return (lncmp(a, b) > 0);
}
inline int lnle(const long_number *a, const long_number *b) {
    return (lncmp(a, b) <= 0);
}
inline int lnlt(const long_number *a, const long_number *b) {
    return (lncmp(a, b) < 0);
}
inline int lneq(const long_number *a, const long_number *b) {
    return (lncmp(a, b) == 0);
}
inline int lnne(const long_number *a, const long_number *b) {
    return (lncmp(a, b) != 0);
}
inline int lneven(const long_number *a) {
    return ((a->digits[0] & 1) == 0);
}
inline int lnodd(const long_number *a) {
    return ((a->digits[0] & 1) != 0);
}
#endif /* __cplusplus */

/* Constants: */
extern const long_number LN_ZERO;
extern const long_number LN_ONE;
extern const long_number LN_MINUS_ONE;

extern void lainit(int number_of_digits); /* Initialize <In: initial number */
                                          /* of digits in temporary arrays> */

extern void laterm();                     /* Release used memory */

/*
 * Basic functions that operate with long numbers defined as arrays
 */
extern void laadd(                      /* Addition */
    int nd1, int nd2, int *nd3,
    int s1,  int s2,  int *s3,
    const byte2 a1[], const byte2 a2[], byte2 a3[]
);

extern void lasub(                      /* Subtraction */
    int nd1, int nd2, int *nd3,
    int s1,  int s2,  int *s3,
    const byte2 a1[], const byte2 a2[], byte2 a3[]
);
extern void lacopy(                     /* Copy numbers a2 := a1 */
    int nd1, int *nd2, int s1, int *s2,
    const byte2 a1[], byte2 a2[]
);
extern void lamul(                      /* Multiplication */
    int nd1, int nd2, int *nd3,
    int s1,  int s2,  int *s3,
    const byte2 a1[], const byte2 a2[], byte2 a3[]
);
extern void lashl(                      /* Multiplication by 2 */
    int *nd, int *s, byte2 *a
);
extern void lashr(                      /* Division by 2 */
    int *nd, int *s, byte2 *a
);
extern void lashl_n(                    /* Mult. by 2^n */
    int *nd, int *s, byte2 *a, int n
);
extern void lashr_n(                    /* Division by 2^n */
    int *nd, int *s, byte2 *a, int n
);
extern void ladiv(                      /* Division */
    int nd1, int nd2, int *nd3, int *nd4,
    int s1,  int s2,  int *s3,  int *s4,
    const byte2 a1[], const byte2 a2[], byte2 a3[], byte2 a4[]
);
extern int lacmp(                       /* Comparing: Returns -1, 0, 1 */
    int nd1, int nd2,
    int s1,  int s2,
    const byte2 a1[], const byte2 a2[]
);
extern void laitoa(                     /* Convertion to text */
    int nd, int s, const byte2 d[],     /* in decimal system  */
    int maxlen, char *text, int *length
);
extern void laatoi(                     /* Convertion         */
    const char *text, int textlen,      /* text->long number  */
    int *nd, int *s,
    byte2 digits[]
);
extern int landig(                      /* Real length in macro-digits */
    int nd, int s, const byte2 a[]
);
extern int landig2(                     /* Number of binary digits */
    int nd, int s, const byte2 d[]
);
extern int landig10( /* Approximate number of decimal digits (upper bound) */
    int nd, int s, const byte2 d[]
);
/* Text representation -> approx. number of macro digits (upper bound) */
extern int laa2nd(
    int len, const char *text
);
extern int lalen2nd( /* Number of decimal digits -> number of macro digits */
    int len
);
extern int latextl(  /* Approximate length of text representation (upper bound) */
    int nd, int s, const byte2 d[]      /* including the terminating zero */
);

/*
typedef union {
    byte4  longword;
    struct {byte2 lowerword; byte2 upperword;} digits;
} twodigits;
*/
/* #define UP_DIGIT(x) (((twodigits*)&(x))->digits.upperword) */

/* Upper and lower digits of 4-byte integer */
#ifndef __cplusplus
#   define LA_UP_DIGIT(x) ((byte2)(((byte4)(x))>>LA_SIZE_OF_DIGIT))
#   define LA_LO_DIGIT(x) ((byte2)(x))
#else
    inline byte2 LA_UPPER_DIGIT(byte4 x) {
        return (byte2) (((byte4)(x))>>LA_SIZE_OF_DIGIT);
    }
    inline byte2 LA_LOWER_DIGIT(byte4 x) {
        return (byte2) x;
    }
#endif  /* __cplusplus */

#define LA_MIN_DIGITS 50        /* Initial number of digits for temp. arrays */
extern byte2 *la_workmem;       /* Pointer to the work memory */

/* #define DEBUG_PRINT */       /* Turn On debug printing */

#ifndef __cplusplus
#   ifndef max
#       define max(x,y) ( ((x) > (y))? (x) : (y) )
#   endif
#   ifndef min
#       define min(x,y) ( ((x) < (y))? (x) : (y) )
#   endif
#else
    inline int max(int x, int y) { return (x >= y)? x : y; }
    inline int min(int x, int y) { return (x <= y)? x : y; }
#endif  /* __cplusplus */

#endif /* LARIPH_H */
