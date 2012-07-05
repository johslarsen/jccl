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
		NHEXSTRING=1,
	};

	enum {                       A,
	                             B,
	                             NOT_A,
	                             NOT_B,
	                             A_AND_B,
	                             A_OR_B,
	                             A_XOR_B};
	const char *hexstrings[7] = {"0000000000007fff0000000000007fff",
	                                             "ffffffffffff1337",
	                             "ffffffffffff8000ffffffffffff8000",
	                                             "000000000000ecc8",
	                             "0000000000007fff0000000000001337",
	                                             "ffffffffffff7fff",
	                             "ffffffffffff8000ffffffffffff6cc8"};

	struct bigint *a = bigint_from_msb_first_hexstring(hexstrings[A], 0);
	struct bigint *b = bigint_from_msb_first_hexstring(hexstrings[B], 0);
	struct bigint *not_a = bigint_not(a);
	struct bigint *not_b = bigint_not(b);
	struct bigint *a_and_b = bigint_and(a, b);
	struct bigint *a_or_b = bigint_or(a, b);
	struct bigint *a_xor_b = bigint_xor(a, b);

	char *res[7];
	res[A] = bigint_to_msb_first_hexstring(a);
	res[B] = bigint_to_msb_first_hexstring(b);
	res[NOT_A] = bigint_to_msb_first_hexstring(not_a);
	res[NOT_B] = bigint_to_msb_first_hexstring(not_b);
	res[A_AND_B] = bigint_to_msb_first_hexstring(a_and_b);
	res[A_OR_B] = bigint_to_msb_first_hexstring(a_or_b);
	res[A_XOR_B] = bigint_to_msb_first_hexstring(a_xor_b);

	UNITTEST(strcmp(hexstrings[A], res[A]) == 0);
	UNITTEST(strcmp(hexstrings[B], res[B]) == 0);
	UNITTEST(strcmp(hexstrings[NOT_A], res[NOT_A]) == 0);
	UNITTEST(strcmp(hexstrings[NOT_B], res[NOT_B]) == 0);
	UNITTEST(strcmp(hexstrings[A_AND_B], res[A_AND_B]) == 0);
	UNITTEST(strcmp(hexstrings[A_OR_B], res[A_OR_B]) == 0);
	UNITTEST(strcmp(hexstrings[A_XOR_B], res[A_XOR_B]) == 0);

	int i;
	for (i = 0; i < 7; i++) {
		free(res[i]);
	}

	bigint_destroy(a);
	bigint_destroy(b);
	bigint_destroy(not_a);
	bigint_destroy(not_b);
	bigint_destroy(a_and_b);
	bigint_destroy(a_or_b);
	bigint_destroy(a_xor_b);
}

int main(void)
{
	bigint_test_creation_and_hexstring();
	bigint_test_bitwise_operators();
	return 0;
}
