#ifndef DMATH_H
#define DMATH_H

/* Discrete mathematics */

/* Returns the nth fibonacci number (0, 1, 1, 3, ...)
 * -EDOM if n < 0, -ERANGE if n > 92 (causes overflow)
 * for more info: http://en.wikipedia.org/wiki/Fibonacci_number */
long long fibonacci(int n);

/* Greatest common divisor is the greatest integer which divides both a and b without a remainder */
int gcd(int a, int b);

/* Least common multiple is the smallest positive integer which is divisable by both a and b without a remainder */
long long lcm(int a, int b);

/* Integers are relative prime if gcd(a,b) = 1, so no common prime factors */
int arerelativeprime(int a, int b);


/* Chinese remainder is a mathematical theorem to calculate x in an equation system like this:
 *   x \equiv a_1 % m_1
 *   x \equiv a_2 % m_2
 *      ...
 *   x \equiv a_neq % m_neq
 *
 * m_1..m_neq must be pairwise relative prime (gcd(m_1, m-2) = 1, ...) */
int chinese_remainder(const int *a, const int *m, int neq);

int isprime(unsigned int n);

#endif /* DMATH_H */
