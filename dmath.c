#include "CuTest/CuTest.h"
#include "dmath.h"
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


unsigned long fibonacci(int n)
{
	if (n < 0) {
		return -EDOM;
	} else if (n > 92) {
		return -ERANGE;
	}

	// start conditions
	unsigned long x_0 = 0;
	unsigned long x_1 = 1;


	if (n == 0) {
		return x_0;
	} else if (n == 1) {
		return x_1;
	}

	unsigned long x_n; // recursion variables, x_n1, ... is the equivalent of x_{n-1} (LaTeX)
	unsigned long x_n1 = x_1;
	unsigned long x_n2 = x_0;

	int i;
	for (i = 1; i < n; i++) {
		x_n = x_n1 + x_n2; // the recursive equation
		x_n2 = x_n1;
		x_n1 = x_n;
	}

	return x_n;
}
void TestFibonacci(CuTest *tc)
{
	enum {
		NFIBONACCI = 94,
	};

	CuAssertTrue(tc, fibonacci(-1) == -EDOM);

	const unsigned long fibonacci_answer[NFIBONACCI] = {
		0, 1, 1, 2, 3, 5, 8, 13, 21, 34,
		55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181,
		6765, 10946, 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229,
		832040, 1346269, 2178309, 3524578, 5702887, 9227465, 14930352, 24157817, 39088169, 63245986,
		102334155, 165580141, 267914296, 433494437, 701408733, 1134903170, 1836311903, 2971215073, 4807526976, 7778742049,
		12586269025, 20365011074, 32951280099, 53316291173, 86267571272, 139583862445, 225851433717, 365435296162, 591286729879, 956722026041,
		1548008755920, 2504730781961, 4052739537881, 6557470319842, 10610209857723, 17167680177565, 27777890035288, 44945570212853, 72723460248141, 117669030460994,
		190392490709135, 308061521170129 ,498454011879264, 806515533049393, 1304969544928657, 2111485077978050, 3416454622906707, 5527939700884757, 8944394323791464, 14472334024676221,
		23416728348467685, 37889062373143906, 61305790721611591, 99194853094755497, 160500643816367088, 259695496911122585, 420196140727489673, 679891637638612258, 1100087778366101931, 1779979416004714189,
		2880067194370816120, 4660046610375530309, 7540113804746346429, -ERANGE
	};

	int i;
	for (i = 0; i < NFIBONACCI; i++) {
		CuAssertIntEquals(tc, fibonacci(i), fibonacci_answer[i]);
	}
}


long gcd(long a, long b)
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


long lcm(int a, int b)
{
	if (a == 0 || b == 0) {
		return -EDOM;
	}

	long res = ((long)a*b)/gcd(a, b);
	if (res < 0) {
		// lcm() is always positive
		res *= -1;
	}

	return res;
}


int arecoprime(long a, long b)
{
	return gcd(a, b) == 1;
}
void TestGcdLcmArecoprime(CuTest *tc)
{
	enum {
		NELEM = 8
	};

	const int a[NELEM] =       {0,     1,        5, 92928,    -92928,   92928,    23,  49};
	const int b[NELEM] =       {1,     0,        5, 123552,   123552,   -123552,  17,  64};
	const int gcd_ans[NELEM] = {-EDOM, -EDOM,    5, 1056,     1056,     1056,     1,   1};
	const int lcm_ans[NELEM] = {-EDOM, -EDOM,    5, 10872576, 10872576, 10872576, 391, 3136};
	const int cp_ans[NELEM] =  {0,     0,        0, 0,        0,        0,        1,   1};


	int i;
	for (i = 0; i < NELEM; i++) {
		CuAssertIntEquals(tc, gcd(a[i], b[i]), gcd_ans[i]);
		CuAssertIntEquals(tc, lcm(a[i], b[i]), lcm_ans[i]);
		CuAssertIntEquals(tc, arecoprime(a[i], b[i]), cp_ans[i]);
	}
}


int arecoprime_pairwise(const long *a, int na)
{
	const long *p, *eoa = a+na;
	for( ; a<eoa; a++) {
		for (p = a+1; p<eoa; p++) {
			if (*a == *p || arecoprime(*a, *p) == 0) {
				return 0;
			}
		}
	}
	return 1;
}


long extended_gcd(long a, long b, long *s, long *t) {
	if (a == 0 || b == 0) {
		*s = 0, *t = 0;
		return -EDOM;
	}

	*s = 0, *t = 1;
	long sprev = 1, tprev = 0;
	long m; // modulo
	while ((m = a % b) != 0) {
		long q = a / b; // quotient
		a = b;
		b = m;

		long stmp = *s, ttmp = *t;
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
void TestExtended_gcd(CuTest *tc)
{
	enum {
		NEGCD = 6,
	};

	const long a[NEGCD] =       {0,        1,     8348,    92928,  -92928, 92928};
	const long b[NEGCD] =       {1,        0,     5359,    123552, 123552, -123552};
	const long s_ans[NEGCD] =   {0,        0,     1264,    4,      -4,     4};
	const long t_ans[NEGCD] =   {0,        0,     -1969,   -3,     -3,     3};
	const long gcd_ans[NEGCD] = {-EDOM,    -EDOM, 1,       1056,   1056,   1056};

	long s, t, i;
	for (i = 0; i < NEGCD; i++) {
		CuAssertIntEquals(tc, extended_gcd(a[i], b[i], &s, &t), gcd_ans[i]);
		CuAssertIntEquals(tc, s, s_ans[i]);
		CuAssertIntEquals(tc, t, t_ans[i]);
	}

}


int chinese_remainder(const long *a, const long *m, int neq)
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

	arecoprime_pairwise(m, neq);

	long sm = 1; // solution modulo
	long *M = NULL; // moduli product except m[i]
	long *y = NULL; // factor to M[i] making y[i]M[i] inverse modulo to m[i] 
	long res = 0; // the result, a number (0 \le res < sm) that solves the whole equation system, all other solutions are congruent with x

	if ((M = (long *)malloc(sizeof(*M)*neq)) == NULL
		|| (y = (long *)malloc(sizeof(*y)*neq)) == NULL) {
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
		long tmp;
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
void TestChinese_remainder(CuTest *tc)
{
	enum {
		NWITHOUT_COPRIME = 4,
		NWITH_COPRIME = 3,
	};

	const long a_with_coprime[NWITH_COPRIME] = {7, 4, 16};
	const long m_with_coprime[NWITH_COPRIME] = {9, 12, 21};
	CuAssertIntEquals(tc, chinese_remainder(a_with_coprime, m_with_coprime, NWITH_COPRIME), -EDOM);

	const long a_without_coprime[NWITHOUT_COPRIME] = {1,2,3,4};
	const long m_without_coprime[NWITHOUT_COPRIME] = {2,3,5,11};
	CuAssertIntEquals(tc, chinese_remainder(a_without_coprime, m_without_coprime, NWITHOUT_COPRIME), 323);

	CuAssertIntEquals(tc, chinese_remainder(NULL, a_without_coprime, NWITHOUT_COPRIME), -EINVAL);
	CuAssertIntEquals(tc, chinese_remainder(a_without_coprime, NULL, NWITHOUT_COPRIME), -EINVAL);
	CuAssertIntEquals(tc, chinese_remainder(a_without_coprime, m_without_coprime, 0), -EINVAL);
}


int isprime(long unsigned int n)
{
	if (n < 2) {
		return 0;
	}

	long unsigned int i, sqrtn = sqrt(n);
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
		MAXN = 4294967295, // 2^32-1, if some computer have UINT_MAX > 2^32-1, do not calculate primes above this, because there are approximately 2*10^8 primes  up to sqrt(2^64)==2^32
#if (UINT_MAX == 65535)
		NPRIME = 54, // number of primes upto sqrt(2^16)
#else
		NPRIME = 6542, // number of primes upto sqrt(2^32)
#endif
	};

	if (n < 2 || n > MAXN) {
		return 0;
	}

	static unsigned int primes[NPRIME];
	static int nprime = 0;

	if (nprime == 0) {
		primes[nprime++] = 2;
	}

	unsigned int sqrtn = sqrt(n)+1;

	unsigned int i;
	for (i = 0; i < nprime && primes[i] <= sqrtn; i++) {
		if (n % primes[i] == 0) {
			return n == primes[i] ? 1 : 0;
		}
	}

	for (i = primes[nprime-1]; i <= sqrtn; i++) {
		if (isprime_cached(i) && (n % i) == 0) {
			return 0;
		}
	}

	if (nprime < NPRIME && n > primes[nprime-1]) {
		// only cache unique primes
		primes[nprime++] = n;
	}

	return 1;
}
void TestIsprime(CuTest *tc)
{
	enum {
		NA = 5,
	};

	unsigned int a[] = 	{2,	3,	4,	25,
#if (UINT_MAX == 65535)
		65521		// highest int before 2^16
#else
		4294967291	// highest int before 2^32
#endif
	};
	int ans[] = 		{1,	1,	0,	0,	1};

	int i;
	for (i = 0; i < NA; i++) {
		CuAssertIntEquals(tc, ans[i], isprime(a[i]));
		CuAssertIntEquals(tc, ans[i], isprime_cached(a[i]));
	}
}


int is_power_of_2(unsigned long n)
{
	if (n == 0) {
		return 0;
	}

	while ((n&1) == 0) {
		n >>= 1;
	}

	return n == 1;
}


int is_power_of(int base, unsigned long n)
{
	if (base < 0) {
		return 0;
	} else if (base == 0) {
		return n == 0;
	} else if (base == 1) {
		return n == 1;
	}

	if (n == 0) {
		return 0;
	} else if (n == 1) {
		return 1;
	}

	int cannot_be_power_of = 0;
	while (n > 0) {
		if (n == base) {
			return !cannot_be_power_of;
		}

		cannot_be_power_of |= n%base;
		n /= base;
	}

	return 0;
}
void TestIs_power_of(CuTest *tc)
{
	enum {
		NBASE = 3,
		NELEM = 3,
	};

	CuAssertIntEquals(tc, 0, is_power_of_2(0));
	CuAssertIntEquals(tc, 0, is_power_of(2, 0));
	CuAssertIntEquals(tc, 0, is_power_of(5, 0));

	int i;
	for (i = 1; i > 0; i<<=1) { // escapes on overflow
		CuAssertIntEquals(tc, 1, is_power_of_2(i));
		CuAssertIntEquals(tc, 1, is_power_of(2, i));
	}

	unsigned int base[NBASE] = {3, 5, 7};
	unsigned int n[8][NELEM] = {
		{},
		{},
		{},
		{3, 9, 27},
		{},
		{5,25,125},
		{},
		{7,49,343},
	};

	for(i = 0; i < NBASE; i++) {

		unsigned int *p = n[base[i]];
		unsigned int *eon = p+NELEM;
		for ( ; p < eon; p++) {

			int j;
			for (j = 0; j < NBASE; j++) {
				CuAssertIntEquals(tc, i == j, is_power_of(base[j], *p));
			}

			CuAssertIntEquals(tc, 0, is_power_of_2(*p));
			CuAssertIntEquals(tc, 0, is_power_of(2, *p));
		}
	}

}


int dmath_ilog2(unsigned long n)
{
	if (n == 0) {
		return -EDOM;
	}

	int i;
	for (i = -1; n > 0; i++) { // -1 compensates for checking when 0<n<2
		n >>= 1;
	}

	return i;
}


int dmath_ilogb(int base, long unsigned int n)
{
	if (base == 0) {
		return n == 0 ? -EDOM : 0;
	} else if (base == 1) {
		return -EDOM;
	}

	if (n == 0) {
		return -EDOM;
	}

	int i;
	for (i = -1; n > 0; i++) { // -1 compensates for checking when 0<n<b
		n /= base;
	}

	return i;
}


const char *basestring_alphabet = "0123456789abcdefghijklmnopqrstuvwxyz";
int basestring_char_to_int(char c)
{
	if (isdigit(c)) {
		return c - '0';
	} else if (islower(c)) {
		return c - 'a' + 10;
	} else {
		return -ERANGE;
	}
}
void TestBasestring_char_to_int(CuTest *tc)
{
	CuAssertIntEquals(tc, basestring_char_to_int('0'-1), -ERANGE);
	CuAssertIntEquals(tc, basestring_char_to_int('z'+1), -ERANGE);

	int i, basestring_alphabet_len = strlen(basestring_alphabet);
	for (i = 0; i < basestring_alphabet_len; i++) {
		CuAssertIntEquals(tc, basestring_char_to_int(basestring_alphabet[i]), i);
	}
}


int basestring_from_long(char *basestring, int base, long unsigned int n)
{
	if (base < 2 || base > 36) {
		return -ERANGE;
	}

	if (n == 0) { // special case, normal algorithm does not work with 0
		basestring[0] = basestring_alphabet[0];
		basestring[1] = '\0';
		return 1; // strlen(basestring)
	}

	int i, max;
	max = dmath_ilogb(base, n);
	for (i = max; i >= 0; n /= base) {
		basestring[i--] = basestring_alphabet[n % base];
	}
	basestring[max+1] = '\0';
	return max+1; // strlen(basestring)
}
void TestLogBasestring_from_log(CuTest *tc)
{
	enum {
		NELEM = 6,
		LONGEST_BASESTRING = 64+1,
	};

	long unsigned int n[NELEM] =    {0,     16, 127, 128, UINT_MAX, ULONG_MAX};
	int dmath_ilog2_ans[NELEM] =    {-EDOM, 4,  6,   7,   31,       63};
	int base[NELEM] =               {3,     3,  5,   7,   11,       36};
	int dmath_ilogb_ans[NELEM] =    {-EDOM, 2,  3,   2,   9,        12};
	char *base_2_string[NELEM] = {
		"0",
		"10000",
		"1111111",
		"10000000",
		"11111111111111111111111111111111",
		"1111111111111111111111111111111111111111111111111111111111111111",
	};
	char *base_b_string[NELEM] = {
		"0",
		"121",
		"1002",
		"242",
		"1904440553",
		"3w5e11264sgsf",
	};

	CuAssertIntEquals(tc, dmath_ilogb(0, 0), -EDOM);
	CuAssertIntEquals(tc, dmath_ilogb(0, 1337), 0);
	CuAssertIntEquals(tc, dmath_ilogb(1, 0), -EDOM);
	CuAssertIntEquals(tc, dmath_ilogb(1, 1337), -EDOM);

	char bs[LONGEST_BASESTRING];
	CuAssertIntEquals(tc, basestring_from_long(bs, 1, 1337), -ERANGE);
	CuAssertIntEquals(tc, basestring_from_long(bs, 37, 1337), -ERANGE);

	int i;
	for (i = 0; i < NELEM; i++) {
		CuAssertIntEquals(tc, dmath_ilog2_ans[i], dmath_ilog2(n[i]));
		CuAssertIntEquals(tc, dmath_ilog2_ans[i], dmath_ilogb(2, n[i]));
		CuAssertIntEquals(tc, dmath_ilogb_ans[i], dmath_ilogb(base[i], n[i]));

		CuAssertIntEquals(tc, strlen(base_2_string[i]), basestring_from_long(bs, 2, n[i]));
		CuAssertStrEquals(tc, base_2_string[i], bs);

		CuAssertIntEquals(tc, strlen(base_b_string[i]), basestring_from_long(bs, base[i], n[i]));
		CuAssertStrEquals(tc, base_b_string[i], bs);

		char *bsp;
		for (bsp = bs; *bsp != '\0'; bsp++) {
			CuAssertIntEquals(tc, basestring_alphabet[basestring_char_to_int(*bsp)], *bsp);
		}
	}
}


unsigned int modular_exponentiation(unsigned int b, unsigned int e, unsigned int m)
{
	if (m < 1) {
		return -EDOM;
	}

	long unsigned result = 1;
	while (e > 0) {
		b = b % m;
		if (e & 1) {
			result *= b;
		}

		e >>= 1;
		b = b * b;
	}

	return result % m;
}
void TestModular_exponentiation(CuTest *tc)
{
	enum {
		NELEM = 3,
	};

	int b[NELEM] =	{3,		7,	3};
	int e[NELEM] =	{644,	644,2003};
	int m[NELEM] =	{645,	645,99};
	int ans[NELEM] =	{36,	436,27};

	int i;
	for (i = 0; i < NELEM; i++) {
		CuAssertIntEquals(tc, modular_exponentiation(b[i], e[i], m[i]), ans[i]);
	}
}
