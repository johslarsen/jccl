#include "jmath.h"
#include <math.h>
#include <errno.h>

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
