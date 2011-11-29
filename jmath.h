#ifndef JMATH_H
#define JMATH_H

/*
 * Returns the nth fibonacci number (0, 1, 1, 3, ...)
 * -EDOM if n < 0, -ERANGE if n > 92 (causes overflow)
 * for more info: http://en.wikipedia.org/wiki/Fibonacci_number
 */
long long fibonacci(int n);

/*
 * Greatest common divisor is the greatest integer which divides both a and b without a remainder
 * if gcd(a,b) = 1 a and b are relative prime
 */
int gcd(int a, int b);

/*
 * Chinese remainder is a mathematical theorem to calculate x in an equation system like this:
 *   x \equiv a_1 % m_1
 *   x \equiv a_2 % m_2
 *      ...
 *   x \equiv a_neq % m_neq
 *
 * m_1..m_neq must be pairwise relative prime (gcd(m_1, m-2) = 1, ...)
 */
int chinese_remainder(const int *a, const int *m, int neq);

#endif /* JMATH_H */
