#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bigint.h"
#include "../unittest.h"

void bigint_test_creation_and_hexstring()
{
	UNITTEST(bigint_from_msb_first_hexstring(NULL, 0) == NULL);
	UNITTEST(bigint_from_msb_first_hexstring(NULL, 1337) == NULL);
	UNITTEST(bigint_to_msb_first_hexstring(NULL) == NULL);

	const unsigned long n_uint = (1UL<<31) - 1;
	const char const *n_hexstring =          "000000007fffffff";

	struct bigint *bns[3];
	UNITTEST((bns[0] = bigint_from_long(n_uint)) != NULL);
	UNITTEST((bns[1] = bigint_from_msb_first_hexstring(n_hexstring, 16)) != NULL);
	UNITTEST((bns[2] = bigint_from_msb_first_hexstring(n_hexstring, 0)) != NULL);

	int i;
	for (i = 0; i < 3; i++) {
		char *res = bigint_to_msb_first_hexstring(bns[i]);
		UNITTEST(strcmp(res, n_hexstring) == 0);
		free (res);
		bigint_destroy(bns[i]);
	}

	const char const *multi_long_hexstring =                    "effffffffffffffff";
	const char const *multi_long_hexstring_res = "fffffffffffffffeffffffffffffffff";

	UNITTEST((bns[0] = bigint_from_msb_first_hexstring(multi_long_hexstring, 17)) != NULL);
	UNITTEST((bns[1] = bigint_from_msb_first_hexstring(multi_long_hexstring, 0)) != NULL);

	for (i = 0; i < 2; i++) {
		char *res = bigint_to_msb_first_hexstring(bns[i]);
		UNITTEST(strcmp(res, multi_long_hexstring_res) == 0);
		free(res);
		bigint_destroy(bns[i]);
	}
}


void bigint_test_bitwise_operators(void)
{
	enum {
		NHEXSTRING=7,
	};

	enum {                       A,
	                             B,
	                             NOT_A,
	                             NOT_B,
	                             A_AND_B,
	                             A_OR_B,
	                             A_XOR_B,
	};
	const char *hexstrings[] = {
		"0000000000007fff0000000000007fff",
		                "ffffffffffff1337",
		"ffffffffffff8000ffffffffffff8000",
		                "000000000000ecc8",
		"0000000000007fff0000000000001337",
		                "ffffffffffff7fff",
		"ffffffffffff8000ffffffffffff6cc8",
	};

	struct bigint *bns[NHEXSTRING];
	bns[A] = bigint_from_msb_first_hexstring(hexstrings[A], 0);
	bns[B] = bigint_from_msb_first_hexstring(hexstrings[B], 0);
	bns[NOT_A] = bigint_not(bns[A]);
	bns[NOT_B] = bigint_not(bns[B]);
	bns[A_AND_B] = bigint_and(bns[A], bns[B]);
	bns[A_OR_B] = bigint_or(bns[A], bns[B]);
	bns[A_XOR_B] = bigint_xor(bns[A], bns[B]);

	int i;
	for (i = 0; i < NHEXSTRING; i++) {
		char *res = bigint_to_msb_first_hexstring(bns[i]);
		UNITTEST(strcmp(hexstrings[i], res) == 0);
		free(res);
		bigint_destroy(bns[i]);
	}
}


void bigint_test_arithmetic_operators(void)
{
	enum {
		NHEXSTRING = 3,
	};

	enum {
		A,
		B,
		A_ADD_B,
	};
	const char *hexstrings[] = {
		"7fffffffffffffff",
		"ffffffffffffffff",
		"0000000000000000fffffffffffffffe",
	};

	struct bigint *bns[NHEXSTRING];
	bns[A] = bigint_from_msb_first_hexstring(hexstrings[A], 0);
	bns[B] = bigint_from_msb_first_hexstring(hexstrings[B], 0);
	bns[A_ADD_B] = bigint_add(bns[A], bns[B]);

	int i;
	for (i = 0; i < NHEXSTRING; i++) {
		char *res = bigint_to_msb_first_hexstring(bns[i]);
		printf("%2d, i:%s\n    o:%s\n", i, hexstrings[i], res);
		UNITTEST(strcmp(hexstrings[i], res) == 0);
		free(res);
		bigint_destroy(bns[i]);
	}
}

int main(void)
{
	bigint_test_creation_and_hexstring();
	bigint_test_bitwise_operators();
	bigint_test_arithmetic_operators();
	return 0;
}
