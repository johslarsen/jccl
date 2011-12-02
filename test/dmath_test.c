#include "dmath_test.h"
#include "../dmath.h"
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
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
		assert(fibonacci(i) == fibonacci_answer[i]);
	}
	assert(fibonacci(-1) == -EDOM);
}

void gcd_lcm_arerelativeprime_test(void)
{
	enum {
		Ngcd = 8
	};

	const int a[Ngcd] =			{0,		1,		5, 92928,	-92928,		92928,		23, 49};
	const int b[Ngcd] =			{1,		0,		5, 123552,	123552,		-123552,	17, 64};
	const int gcd_ans[Ngcd] =	{-EDOM, -EDOM,	5, 1056,	1056,		1056,		1,	1};
	const int lcm_ans[Ngcd] =	{-EDOM, -EDOM,	5, 10872576,10872576,	10872576,	391,3136};
	const int rp_ans[Ngcd] =	{-EDOM, -EDOM,	0, 0,		0,			0,			1,	1};


	int i;
	for (i = 0; i < Ngcd; i++) {
		assert(gcd(a[i], b[i]) == gcd_ans[i]);
		assert(lcm(a[i], b[i]) == lcm_ans[i]);
		assert(arerelativeprime(a[i], b[i]) == rp_ans[i]);
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
	assert(chinese_remainder(a19, m19, Neq19) == 323);

	const int a21[Neq21] = {7, 4, 16};
	const int m21[Neq21] = {9, 12, 21};
	assert(chinese_remainder(a21, m21, Neq21) == -EDOM);

	assert(chinese_remainder(NULL, m21, Neq21) == -EINVAL);
	assert(chinese_remainder(a21, NULL, Neq21) == -EINVAL);
	assert(chinese_remainder(a21, m21, 0) == -EINVAL);
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
		assert(isprime(a[i]) == ans[i]);
	}
}
