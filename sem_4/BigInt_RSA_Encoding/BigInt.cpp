#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "BigInt.h"

BigInt::BigInt() : number(0) {}

BigInt::BigInt(const BigInt &n) : number(n.number) {
  if (n.number != 0)
    link(n.number);
}

BigInt::BigInt(int n) : number(new _long_integer) {
  int s = 1;
  if (n < 0) {
    s = (-1);
    n = (-n);
  }
  number->num_ref = 1;
  number->capacity = 2;
  number->num_digits = 2;
  number->sign = s;
  number->digits[0] = LA_LOWER_DIGIT(n);
  number->digits[1] = LA_UPPER_DIGIT(n);
  if (number->digits[1] == 0)
    number->num_digits = 1;
}

BigInt::BigInt(int size,           // size of digits array
               int ndigits,        // if ndigits == 0, then allocate space only
               int sign /* = 1 */, // sign of a number
               byte2 *digits /* = 0 */ // array of macro digits
               )
    : number(0) {
  int additionalBytes = (size - 2) * sizeof(byte2);
  if (additionalBytes < 0)
    additionalBytes = 0;
  int len = sizeof(_long_integer) + additionalBytes;
  number = (_long_integer *)new char[len];
  memset(number, 0, len);
  link(number);
  number->sign = sign;
  number->num_digits = ndigits;
  if (ndigits > 0 && digits != 0) {
    memmove(number->digits, digits, ndigits * sizeof(byte2));
  }
}

BigInt::BigInt(_long_integer *n) : number(0) { link(n); }

BigInt::~BigInt() { unlink(); }

void BigInt::link(_long_integer *n) {
  number = n;
  if (n != 0)
    ++(n->num_ref);
}

void BigInt::unlink() {
  if (number != 0) {
    assert(number->num_ref > 0);
    --(number->num_ref);
    if (number->num_ref == 0)
      delete number;
    number = 0;
  }
}

void BigInt::clone() {
  assert(number != 0 && number->num_ref > 1);
  int s = size();
  _long_integer *copy = (_long_integer *)new char[s];
  memmove(copy, number, s);
  unlink();
  copy->num_ref = 0;
  link(copy);
}

int BigInt::size() const {
  if (number == 0)
    return 0;
  int addBytes = (number->capacity - 2) * sizeof(byte2);
  if (addBytes < 0)
    addBytes = 0;
  return sizeof(_long_integer) + addBytes;
}

void BigInt::ensureCapacity(int n) {
  if (n == 0)
    return;
  if (number == 0) {
    int addBytes = (n - 2) * sizeof(byte2);
    if (addBytes < 0)
      addBytes = 0;
    int len = sizeof(_long_integer) + addBytes;
    number = (_long_integer *)new char[len];
    memset(number, 0, len);
    number->sign = 1;
    number->capacity = n;
    if (number->capacity < 2)
      number->capacity = 2;
    number->num_digits = 1; // zero
    link(number);
    return;
  }

  if (number->capacity >= n)
    return;

  int addBytes = (n - number->capacity) * sizeof(byte2);
  assert(addBytes > 0);
  int newSize = size() + addBytes;
  _long_integer *new_number = (_long_integer *)new char[newSize];
  memmove(new_number, number, size());
  new_number->num_ref = 0;
  new_number->capacity = n;
  unlink();
  link(new_number);
}

void BigInt::trim() { // Release unnecessary memory
  if (number == 0)
    return;
  int nd = landig(number->num_digits, number->sign, number->digits);
  if (nd == 0)
    nd = 1;
  if (nd == number->capacity)
    return;
  int addBytes = (nd - 2) * sizeof(byte2);
  if (addBytes < 0)
    return;

  int newSize = sizeof(_long_integer) + addBytes;
  _long_integer *new_number =
      (_long_integer *)new char[sizeof(_long_integer) + addBytes];
  memset(new_number, 0, newSize);
  new_number->num_digits = nd;
  new_number->sign = number->sign;
  memmove(new_number->digits, number->digits, nd * sizeof(byte2));
  unlink();
  link(new_number);
}

BigInt &BigInt::operator=(const BigInt &n) {
  if (this == &n) // Copy to itself
    return *this;

  if (n.number == 0) {
    unlink();
    number = 0;
    return *this;
  }
  if (number == 0) {
    link(n.number);
    return *this;
  }
  int nd = n.numDigits();
  if (nd <= 0)
    nd = 1;
  if (nd > number->capacity || number->num_ref > 1) {
    unlink();
    link(n.number);
  } else {
    // Real copy
    number->num_digits = nd;
    number->sign = n.number->sign;
    memmove(number->digits, n.number->digits, nd * sizeof(byte2));
  }
  return *this;
}

BigInt &BigInt::operator=(int n) {
  if (number->num_ref > 1)
    unlink();

  if (number == 0) {
    number = new _long_integer;
    number->capacity = 2;
    number->num_ref = 1;
  }

  int s = 1;
  if (n < 0) {
    s = (-1);
    n = (-n);
  }
  number->num_digits = 2;
  number->sign = s;
  number->digits[0] = LA_LOWER_DIGIT(n);
  number->digits[1] = LA_UPPER_DIGIT(n);
  if (number->digits[1] == 0)
    number->num_digits = 1;
  return *this;
}

const byte2 *BigInt::digits() const {
  if (number == 0)
    return 0;
  else
    return number->digits;
}

int BigInt::num_digits() const {
  if (number == 0)
    return 0;
  else
    return number->num_digits;
}

int BigInt::sign() const {
  if (number == 0)
    return 1;
  else
    return number->sign;
}

// Comparings
bool BigInt::operator==(const BigInt &n) const {
  if (number == 0 && n.number == 0)
    return true;
  if (number == 0 || n.number == 0)
    return false;
  return (lacmp(number->num_digits, n.number->num_digits, number->sign,
                n.number->sign, number->digits, n.number->digits) == 0);
}

bool BigInt::operator==(int n) const {
  if (number == 0)
    return false;
  int nd = numDigits();
  if (nd == 0 && n == 0)
    return true;
  int digitsInInt = sizeof(int) / LA_BYTES_IN_DIGIT;
  if (nd > digitsInInt)
    return false;
  else
    return (((int)*this) == n);
}

int BigInt::compareTo(const BigInt &n) const {
  assert(number != 0 && n.number != 0);
  return lacmp(number->num_digits, n.number->num_digits, number->sign,
               n.number->sign, number->digits, n.number->digits);
}

bool BigInt::operator>(const BigInt &n) const { return (compareTo(n) > 0); }

bool BigInt::operator>=(const BigInt &n) const { return (compareTo(n) >= 0); }

bool BigInt::operator>(int n) const {
  assert(number != 0);
  int nd = numDigits();

  if (nd == 0 && n == 0)
    return false; // Both zero

  int digitsInInt = sizeof(int) / LA_BYTES_IN_DIGIT;
  if (nd > digitsInInt)
    return (number->sign > 0);

  // Consider separately case when high bit is set
  if ((number->digits[digitsInInt] & LA_HIGH_BIT) != 0)
    return (number->sign > 0);

  return (((int)*this) > n);
}

bool BigInt::even() const {
  assert(number != 0);
  return (number->num_digits == 0 || // Should not be so...
          ((number->digits[0] & 1) == 0));
}

BigInt BigInt::operator+(const BigInt &n) const {
  assert(number != 0 && n.number != 0);

  BigInt res;
  add(*this, n, res);
  return res;
}

BigInt BigInt::operator-(const BigInt &n) const {
  assert(number != 0 && n.number != 0);

  BigInt res;
  subtract(*this, n, res);
  return res;
}

// Real number of macro-digits
// (not including leading zeroes)
// num. digitts of zero equals 0
int BigInt::numDigits() const {
  if (number == 0)
    return 0;
  int nd = number->num_digits - 1; // High digit
  while (nd >= 0 && number->digits[nd] == 0)
    --nd;
  return nd + 1;
}

int BigInt::defineNumDigits() {
  assert(number != 0);
  number->num_digits = landig(number->num_digits, number->sign, number->digits);
  if (number->num_digits == 0)
    number->num_digits = 1;
  return number->num_digits;
}

int BigInt::numBits() const { // Length of binary representation
  assert(number != 0);
  return landig2(number->num_digits, number->sign, number->digits);
}

// Approximate length of decimal representation (upper estimation)
int BigInt::numDecimalDigits() const {
  assert(number != 0);
  return landig10(number->num_digits, number->sign, number->digits);
}

BigInt BigInt::operator*(const BigInt &n) const {
  assert(number != 0 && n.number != 0);

  BigInt res;
  multiply(*this, n, res);
  return res;
}

BigInt BigInt::operator/(const BigInt &n) const {
  assert(number != 0 && n.number != 0);

  BigInt q;
  divide(*this, n, &q, 0);
  return q;
}

BigInt BigInt::operator%(const BigInt &n) const {
  assert(number != 0 && n.number != 0);

  BigInt r;
  divide(*this, n, 0, &r);
  return r;
}

void BigInt::divide(const BigInt &denominator, BigInt *quotent,
                    BigInt *residue /* = 0 */
                    ) const {
  divide(*this, denominator, quotent, residue);
}

BigInt BigInt::operator-() const { // Unary minus
  assert(number != 0);
  BigInt res;
  res.ensureCapacity(number->capacity);
  memmove(res.number, number, size());
  res.number->num_ref = 1;
  res.number->sign = (-(number->sign));
  return res;
}

BigInt BigInt::operator<<(int n) const { // Multiply by 2^n
  if (n < 0)
    return operator>>(-n);

  assert(number != 0);
  int ndig2 = landig2( // Number of binary digits
      number->num_digits, number->sign, number->digits);
  int ndig2_new = ndig2 + n;
  int new_capacity = ndig2_new / LA_SIZE_OF_DIGIT;
  if (ndig2_new % LA_SIZE_OF_DIGIT > 0)
    new_capacity++;

  BigInt res;
  res.ensureCapacity(new_capacity);
  lacopy(number->num_digits, &(res.number->num_digits), number->sign,
         &(res.number->sign), number->digits, res.number->digits);
  lashl_n(&(res.number->num_digits), &(res.number->sign), number->digits, n);
  res.defineNumDigits();
  return res;
}

BigInt BigInt::operator>>(int n) const { // Divide by 2^n
  if (n < 0)
    return operator<<(-n);

  assert(number != 0);
  int ndig = landig(number->num_digits, number->sign, number->digits);
  if (ndig <= 0)
    ndig = 1;

  BigInt res;
  res.ensureCapacity(ndig);
  lacopy(ndig, &(res.number->num_digits), number->sign, &(res.number->sign),
         number->digits, res.number->digits);
  lashr_n(&(res.number->num_digits), &(res.number->sign), number->digits, n);
  res.trim();
  return res;
}

// Constants
static _long_integer number_zero = {1, 1, 2, 1, {0, 0}};
static _long_integer number_one = {1, 1, 2, 1, {1, 0}};
static _long_integer number_two = {1, 1, 2, 1, {2, 0}};
static _long_integer number_minus_one = {1, 1, 2, -1, {1, 0}};

static const BigInt bigint_zero(&number_zero);
static const BigInt bigint_one(&number_one);
static const BigInt bigint_two(&number_two);
static const BigInt bigint_minus_one(&number_minus_one);

const BigInt &BigInt::zero() { return bigint_zero; }

const BigInt &BigInt::one() { return bigint_one; }

const BigInt &BigInt::two() { return bigint_two; }

const BigInt &BigInt::minusOne() { return bigint_minus_one; }

char *BigInt::toString(char *str, int maxlen, int *len /* = 0 */) const {
  assert(number != 0);
  assert(number->digits != 0);
  int l;
  laitoa(number->num_digits, number->sign, number->digits, maxlen, str, &l);
  if (len != 0)
    *len = l;
  return str;
}

char *BigInt::itoa(const BigInt n, char *str, int maxlen) { // static method
  n.toString(str, maxlen);
  return str;
}

BigInt &BigInt::fromString(const char *str, int len /* = (-1) */) {
  if (len < 0)
    len = strlen(str);
  int ndig = laa2nd(len, str);
  ensureCapacity(ndig);
  laatoi(str, len, &(number->num_digits), &(number->sign), number->digits);
  return *this;
}

BigInt BigInt::atoi(const char *str, int len /* = (-1) */) { // static method
  BigInt res;
  return res.fromString(str, len);
}

BigInt &BigInt::operator+=(const BigInt &n) {
  assert(number != 0 && n.number != 0);
  add(*this, n, *this);
  return *this;
}

BigInt &BigInt::operator-=(const BigInt &n) {
  assert(number != 0 && n.number != 0);
  subtract(*this, n, *this);
  return *this;
}

BigInt &BigInt::changeSign() {
  assert(number != 0);
  if (number->num_ref > 1)
    clone();

  // Consider zero separately
  int nd = numDigits();
  if (nd <= 0) {
    // Zero
    assert(number->digits[0] == 0);
    number->sign = 1;
    return *this;
  }
  number->sign = (-(number->sign));
  number->num_digits = nd;
  return *this;
}

BigInt &BigInt::operator*=(const BigInt &n) {
  assert(number != 0 && n.number != 0);
  multiply(*this, n, *this);
  return *this;
}

BigInt &BigInt::operator/=(const BigInt &n) {
  assert(number != 0 && n.number != 0);
  divide(*this, n, this, 0);
  return *this;
}

BigInt &BigInt::operator%=(const BigInt &n) {
  assert(number != 0 && n.number != 0);
  divide(*this, n, 0, this);
  return *this;
}

BigInt &BigInt::operator<<=(int n) { // Multiply by 2^n
  if (n < 0)
    return operator>>=(-n);

  assert(number != 0);
  int ndig2 = landig2( // Number of binary digits
      number->num_digits, number->sign, number->digits);
  int ndig2_new = ndig2 + n;
  int new_capacity = ndig2_new / LA_SIZE_OF_DIGIT;
  if (ndig2_new % LA_SIZE_OF_DIGIT > 0)
    new_capacity++;
  ensureCapacity(new_capacity);
  if (number->num_ref > 1)
    clone();
  lashl_n(&(number->num_digits), &(number->sign), number->digits, n);
  defineNumDigits();
  return *this;
}

BigInt &BigInt::operator>>=(int n) { // Divide by 2^n
  if (n < 0)
    return operator<<=(-n);

  assert(number != 0);
  if (number->num_ref > 1)
    clone();
  lashr_n(&(number->num_digits), &(number->sign), number->digits, n);
  defineNumDigits();
  return *this;
}

BigInt &BigInt::shiftl() {
  assert(number != 0);

  int ndig2 = landig2( // Number of binary digits
      number->num_digits, number->sign, number->digits);
  int ndig2_new = ndig2 + 1;
  int new_capacity = ndig2_new / LA_SIZE_OF_DIGIT;
  if (ndig2_new % LA_SIZE_OF_DIGIT > 0)
    new_capacity++;
  ensureCapacity(new_capacity);
  if (number->num_ref > 1)
    clone();

  lashl(&(number->num_digits), &(number->sign), number->digits);

  defineNumDigits();
  return *this;
}

BigInt &BigInt::shiftr() {
  assert(number != 0);
  if (number->num_ref > 1)
    clone();
  lashr(&(number->num_digits), &(number->sign), number->digits);
  defineNumDigits();
  return *this;
}

BigInt::operator int() const {
  assert(number != 0);
  if (number == 0) {
    return 0;
  } else {
    int nd = numDigits();
    if (nd <= 0)
      return 0;
    int totalBytes = nd * LA_BYTES_IN_DIGIT;
    if (totalBytes > sizeof(int))
      totalBytes = sizeof(int);
    nd = totalBytes / LA_BYTES_IN_DIGIT;
    int res = 0;
    for (int i = 0; i < nd; i++) {
      res |= (number->digits[i] << (i * LA_SIZE_OF_DIGIT));
    }
    if (number->sign < 0)
      res = (-res);
    return res;
  }
}

BigInt &BigInt::addMod( // static method
    const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c) {
  add(a, b, c);
  c %= m;
  return c;
}

BigInt BigInt::addMod(const BigInt &n, const BigInt &m) const {
  BigInt res;
  addMod(*this, n, m, res);
  return res;
}

BigInt &BigInt::subtractMod( // static method
    const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c) {
  subtract(a, b, c);
  c %= m;
  return c;
}

BigInt BigInt::subtractMod(const BigInt &n, const BigInt &m) const {
  BigInt res;
  subtractMod(*this, n, m, res);
  return res;
}

BigInt &BigInt::multiplyMod( // static method
    const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c) {
  multiply(a, b, c);
  c %= m;
  return c;
}

BigInt BigInt::multiplyMod(const BigInt &n, const BigInt &m) const {
  BigInt res;
  multiplyMod(*this, n, m, res);
  return res;
}

bool BigInt::divideMod( // static method
    const BigInt &a, const BigInt &b, const BigInt &m, BigInt &c) {
  if (!inverseMod(b, m, c))
    return false;
  multiplyMod(a, c, m, c);
  return true;
}

BigInt BigInt::divideMod(const BigInt &n, const BigInt &m,
                         bool *invertible /* = 0 */
                         ) const {
  BigInt res;
  bool inv = divideMod(*this, n, m, res);
  if (invertible != 0)
    *invertible = inv;
  return res;
}

BigInt &BigInt::negateMod( // static method
    const BigInt &a, const BigInt &m, BigInt &c) {
  c = a;
  int k = lacmp( // compare to abs(m)
      c.number->num_digits, m.number->num_digits, c.number->sign, 1,
      c.number->digits, m.number->digits);
  if (k >= 0) {
    // c >= abs(m)
    c %= m;
    subtract(m, c, c);
    return c;
  }
  k = lacmp( // compare to -abs(m)
      c.number->num_digits, m.number->num_digits, c.number->sign, (-1),
      c.number->digits, m.number->digits);
  if (k <= 0) {
    // c < -abs(m)
    c %= m;
    c.changeSign();
    return c;
  }

  // assert: -abs(m) < c < abs(m)
  c.changeSign();
  if (c.number->sign < 0)
    c += m;
  return c;
}

BigInt BigInt::negateMod(const BigInt &m) const {
  BigInt res;
  negateMod(*this, m, res);
  return res;
}

BigInt &BigInt::changeSignMod(const BigInt &m) {
  negateMod(*this, m, *this);
  return *this;
}

bool BigInt::inverseMod( // static method
    const BigInt &a, const BigInt &m, BigInt &b) {
  assert(a.number != 0 && m.number != 0);
  b.ensureCapacity(m.number->num_digits);

  long_number n, mm, inv;
  n.number_of_digits = a.number->num_digits;
  n.sign = a.number->sign;
  n.digits = a.number->digits;

  mm.number_of_digits = m.number->num_digits;
  mm.sign = m.number->sign;
  mm.digits = m.number->digits;

  inv.digits = b.number->digits;

  int inverted = lminv(&n, &mm, &inv);
  if (!inverted) {
    b = BigInt::zero();
    return false;
  }

  b.number->sign = inv.sign;
  b.number->num_digits = inv.number_of_digits;
  b.defineNumDigits();
  return true;
}

BigInt BigInt::inverseMod(const BigInt &m, bool *invertible /* = 0 */
                          ) const {
  BigInt res;
  bool inv = inverseMod(*this, m, res);
  if (invertible != 0)
    *invertible = inv;
  return res;
}

bool BigInt::equalsMod(const BigInt &n, const BigInt &m) const {
  if (number == 0 || n.number == 0 || m.number == 0)
    return false;
  if (n == m)
    return true;
  else
    return ((*this - n) % m == BigInt::zero());
}

BigInt &BigInt::incrementModBy(const BigInt &n, const BigInt &m) {
  *this += n;
  *this %= m;
  return *this;
}

BigInt &BigInt::incrementMod(const BigInt &m) {
  return incrementModBy(BigInt::one(), m);
}

BigInt &BigInt::decrementModBy(const BigInt &n, const BigInt &m) {
  *this -= n;
  *this %= m;
  return *this;
}

BigInt &BigInt::decrementMod(const BigInt &m) {
  return decrementModBy(BigInt::one(), m);
}

BigInt &BigInt::multiplyModBy(const BigInt &n, const BigInt &m) {
  *this *= n;
  *this %= m;
  return *this;
}

BigInt &BigInt::divideModBy(const BigInt &n, const BigInt &m) {
  *this /= n;
  *this %= m;
  return *this;
}

BigInt BigInt::power(int n) const {
  assert(number != 0);
  assert(n >= 0);

  BigInt b = *this;
  b.raise(n);
  return b;
}

BigInt &BigInt::raise(int n) {
  assert(number != 0);
  assert(n >= 0);

  int nd = numDigits();
  if (nd == 0) {
    *this = BigInt::zero();
    return *this;
  }

  BigInt b = *this;
  *this = BigInt::one();

  while (n > 0) {
    if ((n & 1) == 0) {
      n /= 2;
      b *= b;
    } else {
      --n;
      (*this) *= b;
    }
  }
  return *this;
}

BigInt BigInt::powerMod(const BigInt &n, const BigInt &m) const {
  assert(number != 0);
  assert(n.number != 0);
  assert(m.number != 0);

  BigInt b = *this;
  b.raiseMod(n, m);
  return b;
}

BigInt &BigInt::raiseMod(const BigInt &n, const BigInt &m) {
  assert(number != 0);
  assert(n.number != 0);

  int nd = numDigits();
  if (nd == 0) {
    *this = BigInt::zero();
    return *this;
  }

  BigInt b = *this;
  BigInt k = n;
  *this = BigInt::one();

  while (k != BigInt::zero()) {
    if (k.even()) {
      k.shiftr();
      b.multiplyModBy(b, m);
    } else {
      --k;
      multiplyModBy(b, m);
    }
  }
  return *this;
}

BigInt &BigInt::operator++() { // Prefix increment operator
  *this += BigInt::one();
  return *this;
}

BigInt BigInt::operator++(int) { // Postfix increment operator
  BigInt tmp = *this;
  operator++();
  return tmp;
}

BigInt &BigInt::operator--() { // Prefix decrement operator
  *this += BigInt::minusOne();
  return *this;
}

BigInt BigInt::operator--(int) { // Postfix decrement operator
  BigInt tmp = *this;
  operator--();
  return tmp;
}

// Euclid's algoritm
BigInt BigInt::gcd(const BigInt &a, const BigInt &b) {
  /*...
  BigInt m(a);
  BigInt n(b);

  while (n != BigInt::zero()) {
      BigInt r = m % n;
      m = n;
      n = r;
  }
  if (m.number->sign < 0) {
      m = (-m);
  }
  return m;
  ...*/

  assert(a.number != 0);
  assert(b.number != 0);

  int nda = a.numDigits();
  if (nda <= 0)
    nda = 1;
  int ndb = b.numDigits();
  if (ndb <= 0)
    ndb = 1;

  BigInt d;
  d.ensureCapacity(max(nda, ndb));

  long_number n_a;
  n_a.number_of_digits = nda;
  n_a.sign = a.number->sign;
  n_a.digits = a.number->digits;

  long_number n_b;
  n_b.number_of_digits = ndb;
  n_b.sign = b.number->sign;
  n_b.digits = b.number->digits;

  long_number n_d;
  n_d.digits = d.number->digits;

  lngcd(&n_a, &n_b, &n_d);

  d.number->num_digits = n_d.number_of_digits;
  d.number->sign = n_d.sign;
  d.defineNumDigits();

  return d;
}

// Extended Euclid's algoritm:
// d = gcd(a, b) = u*a + v*b
BigInt &BigInt::extEucl(const BigInt &a, const BigInt &b, BigInt &u, BigInt &v,
                        BigInt &d) {
  assert(a.number != 0 && b.number != 0);
  u = BigInt::one();
  v = BigInt::zero();
  BigInt u2 = BigInt::zero();
  BigInt v2 = BigInt::one();
  d = a;
  BigInt m = b;
  BigInt q, r, tmp;

  m.defineNumDigits();
  while (m.number->num_digits > 1 || m.number->digits[0] != 0) {
    d.divide(m, &q, &r);
    d = m;
    m = r;
    m.defineNumDigits();

    tmp = u;
    u = u2; // u2 = tmp - q * u2;
    u2 *= q;
    tmp -= u2;
    u2 = tmp;

    tmp = v;
    v = v2; // v2 = tmp - q * v2;
    v2 *= q;
    tmp -= v2;
    v2 = tmp;
  }
  return d;
}

BigInt &BigInt::add( // c = a + b
    const BigInt &a, const BigInt &b, BigInt &c) {
  assert(a.number != 0 && b.number != 0);
  int nd = max(a.number->num_digits, b.number->num_digits) + 1;
  c.ensureCapacity(nd);
  if (c.number->num_ref > 1)
    c.clone();
  laadd(a.number->num_digits, b.number->num_digits, &(c.number->num_digits),
        a.number->sign, b.number->sign, &(c.number->sign), a.number->digits,
        b.number->digits, c.number->digits);
  c.defineNumDigits();
  return c;
}

BigInt &BigInt::subtract( // c = a - b
    const BigInt &a, const BigInt &b, BigInt &c) {
  assert(a.number != 0 && b.number != 0);
  int nd = max(a.number->num_digits, b.number->num_digits) + 1;
  c.ensureCapacity(nd);
  if (c.number->num_ref > 1)
    c.clone();
  lasub(a.number->num_digits, b.number->num_digits, &(c.number->num_digits),
        a.number->sign, b.number->sign, &(c.number->sign), a.number->digits,
        b.number->digits, c.number->digits);
  c.defineNumDigits();
  return c;
}

BigInt &BigInt::multiply( // c = a * b
    const BigInt &a, const BigInt &b, BigInt &c) {
  assert(a.number != 0 && b.number != 0);
  int nd = a.number->num_digits + b.number->num_digits;
  c.ensureCapacity(nd);
  if (c.number->num_ref > 1)
    c.clone();
  lamul(a.number->num_digits, b.number->num_digits, &(c.number->num_digits),
        a.number->sign, b.number->sign, &(c.number->sign), a.number->digits,
        b.number->digits, c.number->digits);
  c.defineNumDigits();
  return c;
}

void BigInt::divide(const BigInt &a, const BigInt &b, BigInt *quotent,
                    BigInt *residue /* = 0 */
                    ) {
  assert(a.number != 0 && b.number != 0);

  int *qnd = 0, *qs = 0;
  byte2 *qd = 0;

  int *rnd = 0, *rs = 0;
  byte2 *rd = 0;

  if (quotent != 0) {
    quotent->ensureCapacity(a.number->num_digits);
    if (quotent->number->num_ref > 1)
      quotent->clone();
    qnd = &(quotent->number->num_digits);
    qs = &(quotent->number->sign);
    qd = quotent->number->digits;
  }
  if (residue != 0) {
    residue->ensureCapacity(b.number->num_digits);
    if (residue->number->num_ref > 1)
      residue->clone();
    rnd = &(residue->number->num_digits);
    rs = &(residue->number->sign);
    rd = residue->number->digits;
  }

  ladiv(a.number->num_digits, b.number->num_digits, qnd, rnd,

        a.number->sign, b.number->sign, qs, rs,

        a.number->digits, b.number->digits, qd, rd);

  if (quotent != 0)
    quotent->defineNumDigits();
  if (residue != 0)
    residue->defineNumDigits();
}

//
// Rabin's test:
//     if test returns false, then the number is composite
//     if true, then the number is prime with probability
//              no less than (1. - 0.25^numTests)
bool BigInt::isProbablePrime(int numTests /* = 20 */) const {
  assert(number != 0);
  if (numTests <= 0)
    numTests = 20;
  int length = numBits();
  BigInt b;
  bool res = true;
  int i;

  if (*this == BigInt::zero() || *this == BigInt::one())
    return false;

  for (i = 0; res && (i < numTests); i++) {
    if (length <= 3) {
      b = BigInt::two();
    } else {
      // Generate a random base
      do {
        b.generateRandom(length);
        b %= *this;
      } while (b == BigInt::zero() || b == BigInt::one());
    }
    res = primeTest(&b);
  }
  return res;
}

static char textline[1024]; // For debugging

// Rabin's test
//     If it returns false, then the number is composite;
//     if true, then the number is prime with probability >= 0.75.
// Input: a base used in Little Fermat Theorem
//     If base = 0, then the program selects a random base.
bool BigInt::primeTest(const BigInt *base /* = 0 */) const {
  assert(number != 0);
  BigInt m(*this);

  if (m.number->sign < 0) {
    m.changeSign();
  }

  int c = m.compareTo(BigInt::two());
  if (c < 0) { // number < 2
    return false;
  } else if (c == 0) { // number == 2
    return true;
  }

  if (even()) { // even, not 2 => composite
    return false;
  }

  int t = 0;
  BigInt m_minus_1 = m - BigInt::one();
  BigInt deg(m_minus_1);
  while (deg.even()) {
    deg /= BigInt::two();
    ++t;
  }

  BigInt b;
  if (base != 0) {
    b = *base;
  } else {
    int length = numBits();
    do {
      b.generateRandom(length);
      b %= m;
    } while (b == BigInt::zero() || b == BigInt::one());
  }

  //+++ Debug output
  // m.toString(textline, 1022);
  // printf("Prime test:\nInput number: %s\n", textline);
  // b.toString(textline, 1022);
  // printf("Random base: %s\n", textline);
  // printf("Rabin\'s sequence:\n");
  //+++

  b.raiseMod(deg, m);

  //+++ Debug output
  // b.toString(textline, 1022);
  // printf("    %s\n", textline);
  //+++

  if (b == BigInt::one())
    return true;

  bool was_minus_one = (b == m_minus_1);
  while (!was_minus_one && t > 0) {
    --t;
    BigInt::multiplyMod(b, b, m, b);

    //+++ Debug output
    // b.toString(textline, 1022);
    // printf("    %s\n", textline);
    //+++

    if (b == BigInt::one())
      return false; // We have found the square root of 1 that is not -1
                    // => m is composite, because, if m were prime,
                    // then Zm has been a field, but in a field there are
                    // only 2 square roots of 1: 1 and -1.

    was_minus_one = (b == m_minus_1);
  }

  return was_minus_one;
}

// Generate random big integer with given length
// of its binary representation
void BigInt::generateRandom(int numBits) {
  int nd = (numBits - 1) / LA_SIZE_OF_DIGIT + 1;
  ensureCapacity(nd);
  if (number->num_ref > 1)
    clone();

  int i;
  for (i = 0; i < nd; i++) {
    int x = rand();
    number->digits[i] = (byte2)x;
  }

  number->num_digits = nd;

  // Make the binary length to be exactly "numBits"
  int bitsInHighDigit = numBits % LA_SIZE_OF_DIGIT;
  if (bitsInHighDigit == 0)
    bitsInHighDigit = LA_SIZE_OF_DIGIT;
  byte2 &highDigit = number->digits[nd - 1];
  unsigned int mask = (1 << (bitsInHighDigit - 1));

  highDigit |= mask; // Set the high bit
  mask <<= 1;

  // Clear bits in upper digits to the left of high bit
  while (mask <= LA_HIGH_BIT) {
    highDigit &= (~mask); // Clear bit
    mask <<= 1;
  }

  defineNumDigits();
}

static const int MAX_GENERATE_PRIME_ATTEMPTS = 1000;

bool BigInt::generateRandomPrime(int numBits, int numTests /* = 20 */
                                 ) {
  if (numBits <= 1) {
    *this = BigInt::two();
    return false;
  }

  for (int i = 0; i < MAX_GENERATE_PRIME_ATTEMPTS; i++) {
    generateRandom(numBits);
    number->digits[0] |= 1; // Make odd
    if (isProbablePrime(numTests))
      return true;
  }
  return false;
}
