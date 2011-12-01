#include "jmath.h"
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>

long long fibonacci(int n)
{
	if (n < 0) {
		return -EDOM;
	} else if (n > 92) {
		return -ERANGE;
	}

	// start conditions
	int x_0 = 0;
	int x_1 = 1;

	long long x_n, x_n1, x_n2; // recursion variables, x_n1, ... is the equivalent of x_{n-1} (LaTeX)

	if (n == 0) { 
		return x_0;
	} else if (n == 1) {
		return x_1;
	}

	x_n1 = x_1;
	x_n2 = x_0;

	int i;
	for (i = 1; i < n; i++) {
		x_n = x_n1 + x_n2; // the recursive equation
		x_n2 = x_n1;
		x_n1 = x_n;
	}

	return x_n;
}

int gcd(int a, int b)
{
	if (a == 0 || b == 0) {
		return -EDOM;
	}

	int m; // modulo
	int n = a; // previous modulo, at the end of the algorithm gcd
	while ((m = a % b) != 0) {
		a = b;
		b = n = m;
	}

	if (n < 0) {
		// gcd() is always positive
		n *= -1;
	}

	return n;
}

long long lcm(int a, int b)
{
	if (a == 0 || b == 0) {
		return -EDOM;
	}

	long long res = ((long long)a*b)/gcd(a, b);
	if (res < 0) {
		// lcm() is always positive
		res *= -1;
	}

	return res;
}

int arerelativeprime(int a, int b)
{
	if (a == 0 || b == 0) {
		return -EDOM;
	}

	return gcd(a, b) == 1;
}

int chinese_remainder(const int *a, const int *m, int neq)
{
	if (a == NULL || m == NULL || neq < 1) {
		return -EINVAL;
	}

	int i;
	// check that every integer in arguments a and m are non-zero
	for (i = 0; i < neq; i++) {
		if (a[i] == 0 || m[i] == 0) {
			return -EDOM;
		}
	}

	// check that m[0]..m[neq-1] are pairwise relative prime
	for (i = 0; i < neq-1; i++){
		if (!arerelativeprime(m[i], m[i+1])) {
			return -EDOM;
		}
	}

	int sm = 1; // solution modulo
	int *M = NULL; // moduli product except m[i]
	int *y = NULL; // factor to M[i] making y[i]M[i] inverse modulo to m[i] 
	int res = 0; // the result, a number (0 \le res < sm) that solves the whole equation system, all other solutions are congruent with x

	if ((M = (int *)malloc(sizeof(int)*neq)) == NULL
		|| (y = (int *)malloc(sizeof(int)*neq)) == NULL) {
		res = -errno;
		goto error;
	}

	for (i = 0; i < neq; i++) {
		sm *= m[i];
	}

	for (i = 0; i < neq; i++) {
		M[i] = sm / m[i];
	}

	for (i = 0; i < neq; i++) {
		int j = 0;
		while(++j < INT_MAX) {
			if ((M[i]*j) % m[i] == 1) {
				y[i] = j;
				break;
			} else if ((M[i]*-j) % m[i] == 1) {
				y[i] = -j;
				break;
			}
		}
		if (j == INT_MAX) {
			return -ERANGE;
		}
	}

	for (i = 0; i < neq; i++) {
		res += M[i]*a[i]*y[i];
	}
	res %= sm;
	if (res < 0) {
		res += sm;
	}

error:
	free(M);
	free(y);
	return res;
}
