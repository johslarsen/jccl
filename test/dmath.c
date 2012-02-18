#include "../dmath.h"
#include "../unittest.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


void fibonacci_test(void)
{
	enum {
		Nfibonacci = 94,
	};

	const long long fibonacci_answer[Nfibonacci] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34,
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
	for (i = 0; i < Nfibonacci; i++) {
		UNITTEST(fibonacci(i) == fibonacci_answer[i]);
	}
	UNITTEST(fibonacci(-1) == -EDOM);
}


void gcd_lcm_arecoprime_test(void)
{
	enum {
		Ngcd = 8
	};

	const int a[Ngcd] =			{0,		1,		5, 92928,	-92928,		92928,		23, 49};
	const int b[Ngcd] =			{1,		0,		5, 123552,	123552,		-123552,	17, 64};
	const int gcd_ans[Ngcd] =	{-EDOM, -EDOM,	5, 1056,	1056,		1056,		1,	1};
	const int lcm_ans[Ngcd] =	{-EDOM, -EDOM,	5, 10872576,10872576,	10872576,	391,3136};
	const int cp_ans[Ngcd] =	{0,		0,		0, 0,		0,			0,			1,	1};


	int i;
	for (i = 0; i < Ngcd; i++) {
		UNITTEST(gcd(a[i], b[i]) == gcd_ans[i]);
		UNITTEST(lcm(a[i], b[i]) == lcm_ans[i]);
		UNITTEST(arecoprime(a[i], b[i]) == cp_ans[i]);
	}
}


void extended_gcd_test(void)
{
	enum {
		Negcd = 6,
	};

	const int a[Negcd] =		{0,		1,		8348,	92928,	-92928,	92928};
	const int b[Negcd] =		{1,		0,		5359,	123552,	123552,	-123552};
	const int s_ans[Negcd] =	{0,		0,		1264,	4,		-4,		4};
	const int t_ans[Negcd] =	{0,		0,		-1969,	-3,		-3,		3};
	const int gcd_ans[Negcd] =	{-EDOM, -EDOM,	1,		1056,	1056,	1056};

	int s, t, i;
	for (i = 0; i < Negcd; i++) {
		UNITTEST(extended_gcd(a[i], b[i], &s, &t) == gcd_ans[i]);
		UNITTEST(s == s_ans[i]);
		UNITTEST(t == t_ans[i]);
	}
}


void chinese_remainder_test(void)
{
	enum {
		Neq19 = 4,
		Neq21 = 3,
	};

	const int a19[Neq19] = {1,2,3,4};
	const int m19[Neq19] = {2,3,5,11};
	UNITTEST(chinese_remainder(a19, m19, Neq19) == 323);

	const int a21[Neq21] = {7, 4, 16};
	const int m21[Neq21] = {9, 12, 21};
	UNITTEST(chinese_remainder(a21, m21, Neq21) == -EDOM);

	UNITTEST(chinese_remainder(NULL, m21, Neq21) == -EINVAL);
	UNITTEST(chinese_remainder(a21, NULL, Neq21) == -EINVAL);
	UNITTEST(chinese_remainder(a21, m21, 0) == -EINVAL);
}


void isprime_test(void)
{
	enum {
		Na = 5,
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
	for (i = 0; i < Na; i++) {
		UNITTEST(isprime_cached(a[i]) == ans[i]);
		UNITTEST(isprime(a[i]) == ans[i]);
	}
}


void logbi_uint_to_basestring_test(void)
{
	enum {
		Nuint = 6,
	};

	long long unsigned int n[Nuint] =	{0,		16,	127,	128,	UINT_MAX,	ULONG_MAX};
	int log2i_ans[Nuint] =				{-EDOM, 4,	6,		7,		31,			63};
	int base[Nuint] =					{3,		3,	5,		7,		11,			36};
	int logbi_ans[Nuint] =				{-EDOM, 2,	3,		2,		9,			12};
	char *uint_to_base_2_string_ans[Nuint] = {	"0",
												"10000",
												"1111111",
												"10000000",
												"11111111111111111111111111111111",
												"1111111111111111111111111111111111111111111111111111111111111111"
	};
	char *uint_to_base_b_string_ans[Nuint] = {	"0",
												"121",
												"1002",
												"242",
												"1904440553",
												"3w5e11264sgsf"
	};

	char bs[65];

	int i;
	for (i = 0; i < Nuint; i++) {
		UNITTEST(logbi(n[i], 2) == log2i_ans[i]);
		UNITTEST(logbi(n[i], base[i]) == logbi_ans[i]);

		int len;
		UNITTEST((len = uint_to_basestring(n[i], 2, bs)) == (n[i] == 0 ? 1 : log2i_ans[i] + 1));
		UNITTEST(strcmp(bs, uint_to_base_2_string_ans[i]) == 0);
		
		UNITTEST((len = uint_to_basestring(n[i], base[i], bs)) == (n[i] == 0 ? 1 : logbi_ans[i] + 1));
		UNITTEST(strcmp(bs, uint_to_base_b_string_ans[i]) == 0);

		char *bsp;
		for (bsp = bs; *bsp != '\0'; bsp++) {
			UNITTEST(basestring_alphabet[basestring_char_to_int(*bsp)] == *bsp);
		}
	}

	UNITTEST(uint_to_basestring(0, 1, bs) == -ERANGE);
	UNITTEST(uint_to_basestring(0, 37, bs) == -ERANGE);
}


void modular_exponentiation_test(void)
{
	enum {
		Nme = 3,
	};

	int b[Nme] =	{3,		7,	3};
	int e[Nme] =	{644,	644,2003};
	int m[Nme] =	{645,	645,99};
	int ans[Nme] =	{36,	436,27};

	int i;
	for (i = 0; i < Nme; i++) {
		UNITTEST(modular_exponentiation(b[i], e[i], m[i]) == ans[i]);
	}
}


int main(void)
{
	fibonacci_test();
	gcd_lcm_arecoprime_test();
	extended_gcd_test();
	chinese_remainder_test();
	isprime_test();
	logbi_uint_to_basestring_test();
	modular_exponentiation_test();

	return 0;
}
