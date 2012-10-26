#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../bigint.h"
#include "../unittest.h"

void bigint_test_conversion()
{
	unsigned long somelong;
	struct bigint *somebn;

	UNITTEST(bigint_to_msb_first_hexstring(NULL) == NULL);
	UNITTEST(bigint_from_msb_first_hexstring(NULL, 0) == NULL);
	UNITTEST(bigint_from_msb_first_hexstring(NULL, 1337) == NULL);
	UNITTEST(bigint_to_long(NULL, NULL) == EINVAL);
	UNITTEST(bigint_to_long(NULL, &somelong) == EINVAL);
	UNITTEST(bigint_to_long(somebn, NULL) == EINVAL);

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

		unsigned long lres;
		UNITTEST(bigint_to_long(bns[i], &lres) == 0);
		UNITTEST(lres == n_uint);
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

		unsigned long lres;
		UNITTEST(bigint_to_long(bns[i], &lres) == ERANGE);
		free(res);
		bigint_destroy(bns[i]);
	}
}


void bigint_test_bitwise_operators(void)
{
	enum {
		NHEXSTRING=7,
	};

	enum {
		A,
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


void bigint_test_bitshift(void)
{
	enum {
		NHEXSTRING = 9,
	};

	enum {
		A,
		B,
		MINUS_B,
		A_LEFT_SHIFT_B,
		A_RIGHT_SHIFT_MINUS_B,
		A_RIGHT_SHIFT_B,
		A_LEFT_SHIFT_MINUS_B,
		C,
		C_RIGHT_SHIFT_B,
	};
	const char *hexstrings[] = {
		"bfffffffffffefffffffffffffffefff",
		"0000000000000043",
		"ffffffffffffffbd",
		"fffffffffffffffdffffffffffff7fffffffffffffff7ff80000000000000000",
		"fffffffffffffffdffffffffffff7fffffffffffffff7ff80000000000000000",
		"f7fffffffffffdff",
		"f7fffffffffffdff",
		"8fffffffffffffff",
		"ffffffffffffffff",
	};

	struct bigint *bns[NHEXSTRING];
	bns[A] = bigint_from_msb_first_hexstring(hexstrings[A],0);
	bns[B] = bigint_from_msb_first_hexstring(hexstrings[B],0);
	bns[MINUS_B] = bigint_negate(bns[B]);
	bns[A_LEFT_SHIFT_B] = bigint_shift_left(bns[A], bns[B]);
	bns[A_RIGHT_SHIFT_MINUS_B] = bigint_shift_right(bns[A], bns[MINUS_B]);
	bns[A_RIGHT_SHIFT_B] = bigint_shift_right(bns[A], bns[B]);
	bns[A_LEFT_SHIFT_MINUS_B] = bigint_shift_left(bns[A], bns[MINUS_B]);
	bns[C] = bigint_from_msb_first_hexstring(hexstrings[C], 0);
	bns[C_RIGHT_SHIFT_B] = bigint_shift_right(bns[C], bns[B]);

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
		NHEXSTRING = 10,
	};

	enum {
		A,
		B,
		MINUS_A,
		MINUS_B,
		ABS_A,
		ABS_MINUS_A,
		ABS_B,
		ABS_MINUS_B,
		A_ADD_B,
		A_MINUS_B,
	};
	const char *hexstrings[] = {
		"00000000000000010000000000000000",
		"ffffffffffffffff",
		"ffffffffffffffff0000000000000000",
		"0000000000000001",
		"00000000000000010000000000000000",
		"00000000000000010000000000000000",
		"0000000000000001",
		"0000000000000001",
		"0000000000000000ffffffffffffffff",
		"00000000000000010000000000000001",
	};

	struct bigint *bns[NHEXSTRING];
	bns[A] = bigint_from_msb_first_hexstring(hexstrings[A], 0);
	bns[B] = bigint_from_msb_first_hexstring(hexstrings[B], 0);
	bns[MINUS_A] = bigint_negate(bns[A]);
	bns[MINUS_B] = bigint_negate(bns[B]);
	bns[ABS_A] = bigint_abs(bns[A]);
	bns[ABS_MINUS_A] = bigint_abs(bns[MINUS_A]);
	bns[ABS_B] = bigint_abs(bns[B]);
	bns[ABS_MINUS_B] = bigint_abs(bns[MINUS_B]);
	bns[A_ADD_B] = bigint_add(bns[A], bns[B]);
	bns[A_MINUS_B] = bigint_subtract(bns[A], bns[B]);

	int i;
	for (i = 0; i < NHEXSTRING; i++) {
		char *res = bigint_to_msb_first_hexstring(bns[i]);
		UNITTEST(strcmp(hexstrings[i], res) == 0);
		free(res);
		bigint_destroy(bns[i]);
	}
}


void bigint_test_compare_and_copy(void)
{
	enum {
		NBNS = 4,
		NCOMPARE = 16,
	};

	enum {
		A,
		B,
		MINUS_B,
		MINUS_A,
	};
	struct bigint *bns[4];
	bns[A] = bigint_from_msb_first_hexstring("10000000000000001", 0);
	bns[B] = bigint_from_msb_first_hexstring("10000000000000000", 0);
	bns[MINUS_B] = bigint_negate(bns[B]);
	bns[MINUS_A] = bigint_negate(bns[A]);

	int compare[NBNS][NBNS] = {
		{0,  1,  1,  1},
		{-1, 0,  1,  1},
		{-1, -1, 0,  1},
		{-1, -1, -1, 0}
	};

	int i;
	for (i = 0; i < NBNS; i++) {
		int j;
		for(j = 0; j < NBNS; j++) {
			UNITTEST(bigint_compare(bns[i], bns[j]) == compare[i][j]);
		}

		struct bigint *duplicate = bigint_copy(bns[i]);
		UNITTEST(bigint_compare(bns[i], duplicate) == 0);
		bigint_destroy(duplicate);
	}

	UNITTEST(bigint_compare(NULL, NULL) == 0);
	UNITTEST(bigint_compare(bns[0], NULL) == 1);
	UNITTEST(bigint_compare(NULL, bns[0]) == -1);

	for (i = 0; i < NBNS; i++) {
		bigint_destroy(bns[i]);
	}
}


int main(void)
{
	bigint_test_conversion();
	bigint_test_bitwise_operators();
	bigint_test_bitshift();
	bigint_test_arithmetic_operators();
	bigint_test_compare_and_copy();
	return 0;
}