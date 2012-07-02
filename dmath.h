#ifndef DMATH_H
#define DMATH_H

/* Discrete mathematics */

/* Returns the nth fibonacci number (0, 1, 1, 3, ...)
 * -EDOM if n < 0, -ERANGE if n > 92 (causes overflow)
 * for more info: http://en.wikipedia.org/wiki/Fibonacci_number */
long fibonacci(int n);

/* Greatest common divisor is the greatest integer which divides both a and b without a remainder */
int gcd(int a, int b);

/* Least common multiple is the smallest positive integer which is divisable by both a and b without a remainder */
long lcm(int a, int b);

/* Integers are coprime if gcd(a,b) = 1, so no common prime factors */
int arecoprime(int a, int b);

/* sa + ts = gcd(a,b) */
int extended_gcd(int a, int b, int *s, int *t);

/* Chinese remainder is a mathematical theorem to calculate x in an equation system like this:
 *   x \equiv a_1 % m_1
 *   x \equiv a_2 % m_2
 *      ...
 *   x \equiv a_neq % m_neq
 *
 * m_1..m_neq must be pairwise coprime (gcd(m_1, m-2) = 1, ...) */
int chinese_remainder(const int *a, const int *m, int neq);

int isprime(long unsigned int n);
int isprime_cached(unsigned int n);

/* integer operations to find the floor value of the base logaritm of n 
 * in this library it is used to compute the length-1 needed for a conversion from n to a basestring
 * returns -EDOM if n == 0 */
int logbi(long unsigned int n, int base);

/* convert the integer n into any base
 * the result is stored as a string of characters in bs, assuming bs is large enough
 * the alphabet of the basestring is {0..9}{a..z}
 * returns -ERANGE if base is out of the alphabet range (2-36) */
int uint_to_basestring(long unsigned int n, int base, char *bs);

/* the alphabet used for basestrings */
extern const char *basestring_alphabet;

/* conversion from a basestring character to the integer it represent
 * using character types, so it is much faster than locating the character in basestring_alphabet*/
int basestring_char_to_int(char c);

/* modular exponentiation is a type of exponentiation performed over a modulus
 * b^e % m */
unsigned int modular_exponentiation(unsigned int b, unsigned int e, unsigned m);

#endif /* DMATH_H */
