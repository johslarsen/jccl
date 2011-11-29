#ifndef JMATH_H
#define JMATH_H

/*
 * Returns the nth fibonacci number (0, 1, 1, 3, ...)
 * -EDOM if n < 0, -ERANGE if n > 92 (causes overflow)
 * for more info: http://en.wikipedia.org/wiki/Fibonacci_number
 */
long long fibonacci(int n);

#endif /* JMATH_H */
