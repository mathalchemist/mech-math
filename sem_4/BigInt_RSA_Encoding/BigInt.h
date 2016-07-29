//
//   class BigInt
//   Integer of arbitrary lengh
//   Based on "lariph" package by O.Sysoeva, mech-math MSU, 1993
//
#ifndef BIG_INT_H
#define BIG_INT_H

#include "lariph/lariph.h"
#include "lariph/lmariph.h"

// "Long Integer" object allocated in dynamic memory:
// many "BigInt" instances may refer to the same object of this type.
//
typedef struct {
  int num_ref;     // number of referencies
  int num_digits;  // number of macro-digits
  int capacity;    // length of the "digits" array >= num_digits
  int sign;        // sign +1 / -1
  byte2 digits[2]; // variable length array of macro-digits
} _long_integer;

class BigInt {
private:
  _long_integer *number;

public:
  BigInt();
  BigInt(const BigInt &n);
  BigInt(int n);           // convertion int => BigInt
  BigInt(int size,         // size of digits array
         int ndigits,      // if ndigits == 0, then allocate space only
         int sign = 1,     // sign of a number
         byte2 *digits = 0 // array of macro digits
         );
  BigInt(_long_integer *n);
  ~BigInt();

  void ensureCapacity(int n);
  int size() const; // Size of *number in bytes
  void trim();      // Release unnecessary memory

  BigInt &operator=(const BigInt &n);
  BigInt &operator=(int n);

  // Access to private members
  const byte2 *digits() const;
  int num_digits() const;
  int sign() const;

  int numDigits() const; // Real number of macro-digits
                         // (not including leading zeroes)
                         // num. digitts of zero equals 0

  int numBits() const; // Length of binary representation

  int numDecimalDigits() const; // Approximate number of decimal digits

  int defineNumDigits(); // Changes number->num_digits to
                         // real number of digits. Result >= 1

  // Comparings
  int compareTo(const BigInt &n) const; // Returns -1, 0, 1
  bool operator==(const BigInt &n) const;
  bool operator!=(const BigInt &n) const { return !operator==(n); }
  bool operator==(int n) const;
  bool operator!=(int n) const { return !operator==(n); }
  bool operator>(const BigInt &n) const;
  bool operator>=(const BigInt &n) const;
  bool operator<(const BigInt &n) const { return !operator>=(n); }
  bool operator<=(const BigInt &n) const { return !operator>(n); }
  bool operator>(int n) const;
  bool operator>=(int n) const { return (operator==(n) || operator>(n)); }
  bool operator<(int n) const { return !operator>=(n); }
  bool operator<=(int n) const { return !operator>(n); }

  bool even() const;
  bool odd() const { return !even(); }

  // Ariphmetic operations
  BigInt operator+(const BigInt &n) const;
  BigInt operator-(const BigInt &n) const;
  BigInt operator-() const;
  BigInt operator*(const BigInt &n) const;
  BigInt operator/(const BigInt &n) const;
  BigInt operator%(const BigInt &n) const;
  void divide(const BigInt &denominator, BigInt *quotent = 0,
              BigInt *residue = 0) const;

  BigInt operator<<(int n) const;
  BigInt operator>>(int n) const;

  BigInt &operator+=(const BigInt &n);
  BigInt &operator-=(const BigInt &n);
  BigInt &changeSign();
  BigInt &operator*=(const BigInt &n);
  BigInt &operator/=(const BigInt &n);
  BigInt &operator%=(const BigInt &n);
  BigInt &operator<<=(int n);
  BigInt &operator>>=(int n);

  BigInt &operator++();   // Prefix increment operator
  BigInt operator++(int); // Postfix increment operator
  BigInt &operator--();   // Prefix increment operator
  BigInt operator--(int); // Postfix increment operator

  BigInt &shiftl();
  BigInt &shiftr();

  // Convertion to int
  operator int() const;

  // Operations modulo m
  BigInt addMod(const BigInt &n, const BigInt &m) const;
  BigInt subtractMod(const BigInt &n, const BigInt &m) const;
  BigInt multiplyMod(const BigInt &n, const BigInt &m) const;
  BigInt divideMod(const BigInt &n, const BigInt &m,
                   bool *invertible = 0) const;
  BigInt negateMod(const BigInt &m) const;
  BigInt inverseMod(const BigInt &m, bool *invertible = 0) const;

  bool equalsMod(const BigInt &n, const BigInt &m) const;

  BigInt &incrementMod(const BigInt &m);
  BigInt &decrementMod(const BigInt &m);
  BigInt &incrementModBy(const BigInt &n, const BigInt &m);
  BigInt &decrementModBy(const BigInt &n, const BigInt &m);
  BigInt &multiplyModBy(const BigInt &n, const BigInt &m);
  BigInt &divideModBy(const BigInt &n, const BigInt &m);
  BigInt &changeSignMod(const BigInt &m);

  // Constants
  static const BigInt &zero();
  static const BigInt &one();
  static const BigInt &two();
  static const BigInt &minusOne();

  // Text convertion
  char *toString(char *str, int maxlen, int *len = 0) const;
  BigInt &fromString(const char *str, int len = (-1));
  static char *itoa(const BigInt n, char *str, int maxlen);
  static BigInt atoi(const char *str, int len = (-1));

  // Fast power
  BigInt power(int n) const;
  BigInt &raise(int n);
  BigInt powerMod(const BigInt &n, const BigInt &m) const;
  BigInt &raiseMod(const BigInt &n, const BigInt &m);

  // Euclid's algoritm
  static BigInt gcd(const BigInt &a, const BigInt &b);
  static BigInt &gcd(const BigInt &a, const BigInt &b, BigInt &d);

  // Extended Euclid's algoritm:
  // d = gcd(a, b) = u*a + v*b
  static BigInt &extEucl( // Returns a reference to d
      const BigInt &a, const BigInt &b, BigInt &u, BigInt &v, BigInt &d);

  // Static methods:
  // Ariphmetic operations
  static BigInt &add( // c = a + b
      const BigInt &a, const BigInt &b, BigInt &c);
  static BigInt &subtract( // c = a - b
      const BigInt &a, const BigInt &b, BigInt &c);
  static BigInt &negate( // b = -a
      const BigInt &a, BigInt &b);
  static BigInt &multiply( // c = a * b
      const BigInt &a, const BigInt &b, BigInt &c);
  static void divide( // a / b
      const BigInt &a, const BigInt &b, BigInt *quotent,
      BigInt *residue = 0 // May be zero pointers
      );
  static BigInt &quotent( // c = a / b
      const BigInt &a, const BigInt &b, BigInt &q) {
    divide(a, b, &q, 0);
    return q;
  }
  static BigInt &residue( // c = a / b
      const BigInt &a, const BigInt &b, BigInt &r) {
    divide(a, b, 0, &r);
    return r;
  }
  // Fast power
  static BigInt &power( // b = a^n
      const BigInt &a, int n, BigInt &b);

  // Ariphmetic operations modulo m
  static BigInt &addMod( // c = a + b (mod m)
      const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c);
  static BigInt &subtractMod( // c = a - b (mod m)
      const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c);
  static BigInt &negateMod( // b = -a (mod m), result is always positive
      const BigInt &a, const BigInt &m, BigInt &b);
  static BigInt &changeSignMod( // a = -a (mod m), result is always positive
      const BigInt &a, const BigInt &m);
  static BigInt &multiplyMod( // c = a * b (mod m)
      const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c);
  static bool divideMod( // c = a/b (mod m), returns true if b is invertible
      const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c);
  static bool inverseMod( // inv=a^(-1)(mod m), returns true if b is invertible
      const BigInt &a, const BigInt &m, BigInt &inv);
  static BigInt &powerMod( // c = a^b (mod m)
      const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c);

  // Applications in Number Theory
  //
  // Rabin's test:
  //     if test returns false, then the number is composite
  //     if true, then the number is prime with probability
  //              no less than (1. - 0.25^numTests)
  bool isProbablePrime(int numTests = 20) const;

  // Rabin's test
  //     If it returns false, then the number is composite;
  //     if true, then the number is prime with probability 0.75.
  // Input: base used in Small Ferma Theorem
  //     If base = 0, then the program selects a random base
  bool primeTest(const BigInt *base = 0) const;

  // Generate random big integer with given length
  // of its binary representation
  void generateRandom(int numBits);
  bool generateRandomPrime(int numBits, int numTests = 20); // true if success

private:
  void unlink();
  void link(_long_integer *);
  void clone();
};

inline bool operator==(int n, const BigInt &m) { return m.operator==(n); }
inline bool operator!=(int n, const BigInt &m) { return m.operator!=(n); }
inline bool operator<=(int n, const BigInt &m) { return m.operator>(n); }
inline bool operator<(int n, const BigInt &m) { return m.operator>=(n); }
inline bool operator>(int n, const BigInt &m) { return m.operator<=(n); }
inline bool operator>=(int n, const BigInt &m) { return m.operator<(n); }

#endif
