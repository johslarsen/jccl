#ifndef DMATH_H
#define DMATH_H

/* library for some discrete mathematics functions */


/*
 * the fibonacci sequence is a well known sequence of integers. see
 * http://en.wikipedia.org/wiki/Fibonacci_number for more information.
 *
 * returns:
 *   n < 0  --> -EDOM
 *   n > 92 --> -ERANGE (causes overflow on 64-bit) -->
 *
 *   n-th fibonacci number (0, 1, 1, 3, ...)
 */
extern unsigned long fibonacci(int n);


/*
 * greatest common divisor is the greatest integer which divides both a and b
 * without a remainder.
 *
 * returns:
 *   a == 0 || b == 0 --> -EDOM
 *   --> gcd(a, b)
 */
extern long gcd(long a, long b);


/*
 * least common multiple is the smallest positive integer which is divisable by
 * both a and b without a remainder.
 *
 * returns:
 *   a == 0 || b == 0 --> -EDOM
 *   --> lcm(a, b)
 */
extern long lcm(int a, int b);


/*
 * integers are coprime if gcd(a,b) = 1, i.e. if a and b have no common prime
 * factors.
 *
 * returns:
 *   --> gcd(a, b) == 1
 */
extern int arecoprime(long a, long b);


/*
 * pairwise coprime means that every pair of integers in a set of integers (a)
 * must be coprime with each other.
 *
 * returns:
 *   a[0..na, i] == a[(i+1)..na] --> 0
 *   --> arecoprime(a[0..na, i], a[(i+1)..na]) == 1
 */
extern int arecoprime_pairwise(const long *a, int na);

/*
 * extended gcd is an algorithm to calculate s and b in equation like this
 * sa + tb = gcd(a,b).
 *
 * the values s and t from the equation is stored in respectively *s and *s
 *
 * returns:
 *   a == 0 || b == 0 --> -EDOM
 *   --> gcd(a, b)
 */
extern long extended_gcd(long a, long b, long *s, long *t);

/*
 * Chinese remainder is a mathematical theorem to calculate x in an equation
 * system like this:
 *   x \equiv a_1 % m_1
 *   x \equiv a_2 % m_2
 *      ...
 *   x \equiv a_neq % m_neq
 *
 * returns:
 *   a == NULL || m == NULL || neq == 0  --> -EINVAL
 *
 *   a[0..neq] == 0 || m[0..neq] == 0    --> -EDOM
 *   arecoprime_pairwise(m, neq) == 0    --> -EDOM
 *
 *   --> x (from equation system)
 */
extern int chinese_remainder(const long *a, const long *m, int neq);

/*
 * check if n is a prime number
 *
 * returns:
 *   --> prime ? 1 : 0;
 */
extern int isprime(long unsigned int n);
/* cached version. generating cache as primes are tested.*/
extern int isprime_cached(unsigned int n);


/*
 * integer operations to find the floor value of the base logaritm of n.
 * returns:
 *   base < 2 --> -EDOM
 *   n == 0 --> -EDOM
 *   --> log_{base}(n)
 */
extern int dmath_ilogb(int base, long unsigned int n);
/* more efficient version when base == 2 */
extern int dmath_ilog2(long unsigned int n);


/*
 * can n be written as base^a, a \in Z
 *
 * returns:
 *   base < 2 --> 0
 *   n == 0 --> 0
 *   --> base^a == n
 */
extern int is_power_of(int base, unsigned long n);
/* more efficient version when base == 2 */
extern int is_power_of_2(unsigned long n);


/*
 * the alphabet used for basestrings {0..1}{a..z}
 */
extern const char *basestring_alphabet;


/*
 * convert a basestring character to the integer it represents.
 *
 * returns:
 *   c not in basestring_alphabet --> -ERANGE
 *   --> index to character in basestring_alphabet
 */
extern int basestring_char_to_int(char c);

/*
 * convert the integer n into a string of any base.
 *
 * resulting string is stored in *basestring, assuming this is large enough
 *
 * returns:
 *   basestring == NULL --> -EINVAL
 *   base < 2 || base > 36 --> -ERANGE
 *   --> strlen(basestring)
 */
extern int basestring_from_long(char *basestring, int base, long unsigned int n);


/*
 * modular exponentiation is a type of exponentiation performed over a modulus.
 *
 * returns:
 *   m < 1 --> -EDOM
 *   --> b^e % m 
 */
extern unsigned int modular_exponentiation(unsigned int b, unsigned int e, unsigned m);

#endif /* DMATH_H */
