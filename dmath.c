#include "dmath.h"
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>


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
	while ((m = a % b) != 0) {
		a = b;
		b = m;
	}

	if (b < 0) {
		// gcd() is always positive
		b *= -1;
	}

	return b;
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
	return gcd(a, b) == 1;
}


int extended_gcd(int a, int b, int *s, int *t) {
	if (a == 0 || b == 0) {
		*s = 0, *t = 0;
		return -EDOM;
	}

	*s = 0, *t = 1;
	int sprev = 1, tprev = 0;
	int m; // modulo
	while ((m = a % b) != 0) {
		int q = a / b; // quotient
		a = b;
		b = m;

		int stmp = *s, ttmp = *t;
		*s = sprev - *s*q;
		*t = tprev - *t*q;
		sprev = stmp;
		tprev = ttmp;
	}

	if (b < 0) {
		b *= -1;
		*s *= -1;
		*t *= -1;
	}

	return b;
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
		int tmp;
		if (extended_gcd(M[i], m[i], &y[i], &tmp) != 1) {
			res = -EDOM;
			goto error;
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


int isprime(long long unsigned int n)
{
	if (n < 2) {
		return 0;
	}

	long long unsigned int i, sqrtn = sqrt(n);
	for (i = 2; i <= sqrtn; i++) {
		if (n % i == 0) {
			return 0;
		}
	}

	return 1;
}


int isprime_cached(unsigned int n)
{
	enum {
		Maxn = 4294967295, // 2^32-1, if some computer have UINT_MAX > 2^32-1, do not calculate primes above this
#if (UINT_MAX == 65535)
		Nprime = 54, // number of primes upto sqrt(2^16)
#else
		Nprime = 6542, // number of primes upto sqrt(2^32)
#endif
	};

	if (n < 2 || n > Maxn) {
		return 0;
	}

	static unsigned int primes[Nprime];
	static int nprime = 0;

	if (nprime == 0) {
		primes[nprime++] = 2;
	}

	unsigned int sqrtn = sqrt(n);

	unsigned int i;
	for (i = 0; i < nprime && primes[i] <= sqrtn; i++) {
		if (n % primes[i] == 0) {
			return 0;
		}
	}

	for (i = primes[nprime-1]; i <= sqrtn; i++) {
		if (isprime_cached(i) && n % i == 0) {
			return 0;
		}
	}

	if (n > primes[nprime-1] && n < sqrt(UINT_MAX)) {
		// only cache unique primes
		// and primes above sqrt(UINT_MAX) will never be tested, so do not cahce them
		primes[nprime++] = n;
	}

	return 1;
}
