#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigint.h"
#include "CuTest/CuTest.h"

enum {
	NIBBLE_BIT = 4,
	NIBBLE_MASK = (1<<4)-1,

#ifndef LONG_BIT
	LONG_BIT = sizeof(long) * CHAR_BIT,
#endif /* LONG_BIT */
	NNIBBLE_IN_LONG = LONG_BIT/NIBBLE_BIT,

	LONG_MSB = LONG_BIT-1,
	LONG_MSB_MASK = 1UL<<LONG_MSB,

	NIBBLE_MSB = NIBBLE_BIT-1,
	NIBBLE_MSB_MASK = 1<<NIBBLE_MSB,
};

struct bigint {
	int nchunk;
	unsigned long pad_chunk;
	unsigned long chunks[];
};
#define BIGINT_SIZE(nchunk) sizeof(struct bigint) + sizeof(*((struct bigint *)NULL)->chunks) * (nchunk)

const char *bigint_hex_charset = "0123456789abcdef";
enum pad_chunks {
	PAD_CHUNK_POSITIVE = 0,
	PAD_CHUNK_NEGATIVE = ULONG_MAX,
};
static const unsigned long pad_chunks[] = {PAD_CHUNK_POSITIVE, PAD_CHUNK_NEGATIVE}; // indexed by MSB
static const struct bigint bigint_one = {.nchunk = 1, .pad_chunk = PAD_CHUNK_POSITIVE, .chunks = {1} };


static struct bigint *bigint_init(int nchunk) {
	assert(nchunk > 0);
	struct bigint *n = (struct bigint *)calloc(1, BIGINT_SIZE(nchunk));
	if (n == NULL) {
		return NULL;
	}

	n->nchunk = nchunk;

	return n;
}
void TestBigint_init(CuTest *tc) {
	for (int i = 1; i <= 10; i++) {
		struct bigint *n = bigint_init(i);
		CuAssertPtrNotNull(tc, n);
		CuAssertIntEquals(tc, i, n->nchunk);
		CuAssertPtrNotNull(tc, n->chunks);

		bigint_destroy(n);
	}
}


static inline unsigned long bigint_index_with_padding(const struct bigint *n, int i) {
	assert(n != NULL);
	return i >= 0 && i < n->nchunk ? n->chunks[i] : n->pad_chunk;
}
void TestBigint_index_with_padding(CuTest *tc) {
	struct bigint *positive = bigint_from_long(pad_chunks[0]);
	CuAssertPtrNotNull(tc, positive);
	struct bigint *negative = bigint_from_long(pad_chunks[1]);
	CuAssertPtrNotNull(tc, negative);

	CuAssertIntEquals(tc, pad_chunks[0], bigint_index_with_padding(positive, 0));
	CuAssertIntEquals(tc, pad_chunks[0], bigint_index_with_padding(positive, 1));
	CuAssertIntEquals(tc, pad_chunks[0], bigint_index_with_padding(positive, -1));
	CuAssertIntEquals(tc, pad_chunks[1], bigint_index_with_padding(negative, 0));
	CuAssertIntEquals(tc, pad_chunks[1], bigint_index_with_padding(negative, 1));
	CuAssertIntEquals(tc, pad_chunks[1], bigint_index_with_padding(negative, -1));

	bigint_destroy(positive);
	bigint_destroy(negative);
}


static inline int bigint_msb(const struct bigint *n, int i) {
	assert(n != NULL);
	return bigint_index_with_padding(n, i) >> LONG_MSB;
}
void TestBigint_msb(CuTest *tc) {
	struct bigint *positive = bigint_from_long(LONG_MAX);
	CuAssertPtrNotNull(tc, positive);
	struct bigint *negative = bigint_from_long(LONG_MIN);
	CuAssertPtrNotNull(tc, negative);

	CuAssertIntEquals(tc, 0, bigint_msb(positive, 0));
	CuAssertIntEquals(tc, 0, bigint_msb(positive, INT_MAX));
	CuAssertIntEquals(tc, 1, bigint_msb(negative, 0));
	CuAssertIntEquals(tc, 1, bigint_msb(negative, INT_MAX));

	bigint_destroy(positive);
	bigint_destroy(negative);

}


void bigint_destroy(struct bigint *n) {
	if (n != NULL) {
		free(n);
	}
}


struct bigint *bigint_copy(const struct bigint *n) {
	if (n == NULL) {
		return NULL;
	}

	struct bigint *res = bigint_init(n->nchunk);
	if (res == NULL) {
		return NULL;
	}

	res->pad_chunk = n->pad_chunk;
	memmove(res->chunks, n->chunks, n->nchunk*sizeof(*n->chunks));

	return res;
}
void TestBigint_copy(CuTest *tc) {
	struct bigint *single_chunk_n = bigint_from_long(pad_chunks[1]);
	CuAssertPtrNotNull(tc, single_chunk_n);
	struct bigint *multi_chunk_n = bigint_from_msb_first_hexstring("0ffffffffffffffffffffffffffffffffffff", 0);
	CuAssertPtrNotNull(tc, multi_chunk_n);

	struct bigint *single_chunk_copy = bigint_copy(single_chunk_n);
	CuAssertPtrNotNull(tc, single_chunk_copy);
	struct bigint *multi_chunk_copy = bigint_copy(multi_chunk_n);
	CuAssertPtrNotNull(tc, multi_chunk_copy);

	CuAssertIntEquals(tc, 0, bigint_compare(single_chunk_n, single_chunk_copy));
	CuAssertIntEquals(tc, 0, bigint_compare(multi_chunk_n, multi_chunk_copy));

	bigint_destroy(single_chunk_n);
	bigint_destroy(single_chunk_copy);
	bigint_destroy(multi_chunk_n);
	bigint_destroy(multi_chunk_copy);
}


static void bigint_identify_pad_chunk_and_trim(struct bigint *n) {
	if (n == NULL) {
		return;
	}

	int msb = bigint_msb(n, n->nchunk-1);
	n->pad_chunk = pad_chunks[msb];

	int i;
	for (i = n->nchunk-1; i > 0; i--) {
		if (n->chunks[i] != n->pad_chunk) {
			break;
		}
	}

	if (bigint_msb(n, i) != msb) {
		i++;
	}

	int new_nchunk = i+1;
	if (new_nchunk != n->nchunk) {
		assert(new_nchunk < n->nchunk);
		n->nchunk = new_nchunk;
		struct bigint *new_n = realloc(n, BIGINT_SIZE(new_nchunk));
		assert(new_n == n);
	}
}
void TestBigint_identify_pad_chunk_and_trim(CuTest *tc) {
	struct bigint *untrimable_positive = bigint_init(2);
	CuAssertPtrNotNull(tc, untrimable_positive);
	struct bigint *untrimable_negative = bigint_init(2);
	CuAssertPtrNotNull(tc, untrimable_negative);
	struct bigint *trimable_positive = bigint_init(2);
	CuAssertPtrNotNull(tc, trimable_positive);
	struct bigint *trimable_negative = bigint_init(2);
	CuAssertPtrNotNull(tc, trimable_negative);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"

	untrimable_positive->chunks[0] = pad_chunks[1];
	untrimable_positive->chunks[1] = pad_chunks[0]+1;
	untrimable_negative->chunks[0] = pad_chunks[0];
	untrimable_negative->chunks[1] = pad_chunks[1]-1;
	trimable_positive->chunks[0] = pad_chunks[0];
	trimable_positive->chunks[1] = pad_chunks[0];
	trimable_negative->chunks[0] = pad_chunks[1];
	trimable_negative->chunks[1] = pad_chunks[1];

	bigint_identify_pad_chunk_and_trim(untrimable_positive);
	bigint_identify_pad_chunk_and_trim(untrimable_negative);
	bigint_identify_pad_chunk_and_trim(trimable_positive);
	bigint_identify_pad_chunk_and_trim(trimable_negative);

	CuAssertIntEquals(tc, pad_chunks[0], untrimable_positive->pad_chunk);
	CuAssertIntEquals(tc, pad_chunks[1], untrimable_negative->pad_chunk);
	CuAssertIntEquals(tc, pad_chunks[0], trimable_positive->pad_chunk);
	CuAssertIntEquals(tc, pad_chunks[1], trimable_negative->pad_chunk);

	CuAssertIntEquals(tc, 2, untrimable_positive->nchunk);
	CuAssertIntEquals(tc, 2, untrimable_negative->nchunk);
	CuAssertIntEquals(tc, 1, trimable_positive->nchunk);
	CuAssertIntEquals(tc, 1, trimable_negative->nchunk);

	bigint_destroy(untrimable_positive);
	bigint_destroy(untrimable_negative);
	bigint_destroy(trimable_positive);
	bigint_destroy(trimable_negative);
#pragma GCC diagnostic pop
}


struct bigint *bigint_from_long(unsigned long n) {
	struct bigint *bigint = bigint_init(1);
	if (bigint == NULL) {
		return NULL;
	}

	bigint->chunks[0] = n;

	bigint_identify_pad_chunk_and_trim(bigint);
	return bigint;
}
void TestBigint_from_long(CuTest *tc) {
	unsigned long a_number = 1337;
	struct bigint *n = bigint_from_long(a_number);

	CuAssertPtrNotNull(tc, n);
	CuAssertIntEquals(tc, 1, n->nchunk);
	CuAssertIntEquals(tc, a_number, n->chunks[0]);

	bigint_destroy(n);
}


int bigint_to_long(const struct bigint *n, unsigned long *result) {
	if (n == NULL || result == NULL) {
		return EINVAL;
	}

	*result = n->chunks[0];

	return n->nchunk > 1 ? ERANGE : 0;
}
void TestBigint_to_long(CuTest *tc) {
	unsigned long a_number = 1337;
	struct bigint *n = bigint_from_long(a_number);
	CuAssertPtrNotNull(tc, n);

	unsigned long result = 0;
	CuAssertIntEquals(tc, 0, bigint_to_long(n, &result));
	CuAssertIntEquals(tc, a_number, result);

	bigint_destroy(n);
}


struct bigint *bigint_from_msb_first_hexstring(const char *s, size_t ns) {
	if (s == NULL) {
		return NULL;
	}

	if (ns == 0) {
		ns = strlen(s);
	}

	int nchunk = ns/NNIBBLE_IN_LONG;

	int empty_space_in_last_chunk = ns%NNIBBLE_IN_LONG != 0;
	if(empty_space_in_last_chunk) {
		nchunk += 1;
	}

	struct bigint *n = bigint_init(nchunk);
	if (n == NULL) {
		goto error;
	}

	unsigned long nibble;
	for (size_t i = 0; i < ns; i++) {
		char *hexchar = strchr(bigint_hex_charset, s[ns-1-i]);
		if (hexchar == NULL) {
			goto error;
		}


		nibble = hexchar-bigint_hex_charset;

		n->chunks[i/NNIBBLE_IN_LONG] |= nibble<<(NIBBLE_BIT * (i%NNIBBLE_IN_LONG));
	}

	if (empty_space_in_last_chunk && (nibble>>NIBBLE_MSB) == 1) {
		n->chunks[n->nchunk-1] |= pad_chunks[1] << (ns%NNIBBLE_IN_LONG);
	}

	bigint_identify_pad_chunk_and_trim(n);
	return n;
error:
	bigint_destroy(n);
	return NULL;
}


int bigint_nnibble(const struct bigint *n) {
	return n == NULL ? -EINVAL : n->nchunk*NNIBBLE_IN_LONG;
}
void TestBigint_nnibble(CuTest *tc) {
	for (int i = 1; i < 10; i++) {
		struct bigint *n = bigint_init(i);
		CuAssertPtrNotNull(tc, n);

		CuAssertIntEquals(tc, i, n->nchunk);
		CuAssertIntEquals(tc, i*NNIBBLE_IN_LONG, bigint_nnibble(n));

		bigint_destroy(n);
	}
}


int bigint_to_msb_first_hexstring(const struct bigint *n, char *s) {
	if (n == NULL || s == NULL) {
		return -EINVAL;
	}

	char *start_of_chunk = s;
	for (int i = n->nchunk-1; i >= 0; i--) {
		unsigned long chunk = n->chunks[i];

		for (char *p = start_of_chunk+NNIBBLE_IN_LONG-1; p >= start_of_chunk; p--) {
			*p = bigint_hex_charset[chunk & NIBBLE_MASK];
			chunk >>= NIBBLE_BIT;
		}

		start_of_chunk += NNIBBLE_IN_LONG;
	}
	*start_of_chunk = '\0';

	return start_of_chunk-s;
}


void TestBigintCreationAndHextring(CuTest *tc) {
	enum {
		RES_LEN = 128,
	};

	const unsigned long n_uint = (1UL<<31) - 1;
	char const * const n_hexstring =          "000000007fffffff";
	size_t n_hexstring_len = strlen(n_hexstring);

	struct bigint *bns[3];
	CuAssertPtrNotNull(tc, (bns[0] = bigint_from_long(n_uint)));
	CuAssertPtrNotNull(tc, (bns[1] = bigint_from_msb_first_hexstring(n_hexstring, 16)));
	CuAssertPtrNotNull(tc, (bns[2] = bigint_from_msb_first_hexstring(n_hexstring, 0)));

	char res[RES_LEN];

	for (int i = 0; i < 3; i++) {
		CuAssertIntEquals(tc, n_hexstring_len, bigint_to_msb_first_hexstring(bns[i], res));
		CuAssertStrEquals(tc, n_hexstring, res);
		bigint_destroy(bns[i]);
	}

	char const * const multi_long_hexstring =                    "effffffffffffffff";
	char const * const multi_long_hexstring_res = "fffffffffffffffeffffffffffffffff";
	size_t multi_long_hexstring_res_len = strlen(multi_long_hexstring_res);

	CuAssertPtrNotNull(tc, (bns[0] = bigint_from_msb_first_hexstring(multi_long_hexstring, 17)));
	CuAssertPtrNotNull(tc, (bns[1] = bigint_from_msb_first_hexstring(multi_long_hexstring, 0)));

	for (int i = 0; i < 2; i++) {
		CuAssertIntEquals(tc, multi_long_hexstring_res_len, bigint_to_msb_first_hexstring(bns[i], res));
		CuAssertStrEquals(tc, multi_long_hexstring_res, res);
		bigint_destroy(bns[i]);
	}
}


int bigint_compare(const struct bigint *a, const struct bigint *b) {
	if (a == NULL) {
		return b == NULL ? 0 : -1;
	} else if (b == NULL) {
		return a == NULL ? 0 : 1;
	}

	int a_msb = bigint_msb(a, -1);
	int b_msb = bigint_msb(b, -1);

	if (a_msb != b_msb) {
		return a_msb == 1 ? -1 : 1;
	}

	if (a->nchunk != b->nchunk) {
		int abs_res = a->nchunk > b->nchunk ? 1 : -1;
		return a_msb == 1 ? -abs_res : abs_res;
	}

	for (int i = a->nchunk-1; i >= 0; i--) {
		if (a->chunks[i] != b->chunks[i]) {
			return a->chunks[i] > b->chunks[i] ? 1 : -1;
		}
	}

	return 0;
}
void TestBigint_compare(CuTest *tc) {
	struct bigint *zero = bigint_from_long(0), *four = bigint_from_long(4);
	CuAssertPtrNotNull(tc, zero);
	CuAssertPtrNotNull(tc, four);

	struct bigint *long_min = bigint_from_long(LONG_MIN), *long_max = bigint_from_long(LONG_MAX);
	CuAssertPtrNotNull(tc, long_min);
	CuAssertPtrNotNull(tc, long_max);

	struct bigint *longer_min = bigint_shift_left(long_min, four), *longer_max = bigint_shift_left(long_max, four);
	CuAssertPtrNotNull(tc, longer_min);
	CuAssertPtrNotNull(tc, longer_max);

	CuAssertIntEquals(tc, 0, bigint_compare(NULL, NULL));
	CuAssertIntEquals(tc, 1, bigint_compare(longer_max, NULL));
	CuAssertIntEquals(tc, 1, bigint_compare(longer_min, NULL));
	CuAssertIntEquals(tc, -1, bigint_compare(NULL, longer_max));
	CuAssertIntEquals(tc, -1, bigint_compare(NULL, longer_min));

	CuAssertIntEquals(tc, 0,  bigint_compare(longer_max, longer_max));
	CuAssertIntEquals(tc, 1,  bigint_compare(longer_max, long_max));
	CuAssertIntEquals(tc, 1,  bigint_compare(longer_max, zero));
	CuAssertIntEquals(tc, 1,  bigint_compare(longer_max, long_min));
	CuAssertIntEquals(tc, 1,  bigint_compare(longer_max, longer_min));
	CuAssertIntEquals(tc, -1, bigint_compare(long_max, longer_max));
	CuAssertIntEquals(tc, 0,  bigint_compare(long_max, long_max));
	CuAssertIntEquals(tc, 1,  bigint_compare(long_max, zero));
	CuAssertIntEquals(tc, 1,  bigint_compare(long_max, long_min));
	CuAssertIntEquals(tc, 1,  bigint_compare(long_max, longer_min));
	CuAssertIntEquals(tc, -1, bigint_compare(long_min, longer_max));
	CuAssertIntEquals(tc, -1, bigint_compare(long_min, long_max));
	CuAssertIntEquals(tc, -1, bigint_compare(long_min, zero));
	CuAssertIntEquals(tc, 0,  bigint_compare(long_min, long_min));
	CuAssertIntEquals(tc, 1,  bigint_compare(long_min, longer_min));
	CuAssertIntEquals(tc, -1, bigint_compare(longer_min, longer_max));
	CuAssertIntEquals(tc, -1, bigint_compare(longer_min, long_max));
	CuAssertIntEquals(tc, -1, bigint_compare(longer_min, zero));
	CuAssertIntEquals(tc, -1, bigint_compare(longer_min, long_min));
	CuAssertIntEquals(tc, 0,  bigint_compare(longer_min, longer_min));

	bigint_destroy(longer_max);
	bigint_destroy(longer_min);
	bigint_destroy(long_max);
	bigint_destroy(long_min);
	bigint_destroy(four);
	bigint_destroy(zero);
}


struct bigint *bigint_not(const struct bigint *n) {
	if (n == NULL) {
		return NULL;
	}

	struct bigint *res = bigint_init(n->nchunk);

	for (int i = 0; i < n->nchunk; i++) {
		res->chunks[i] = ~n->chunks[i];
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}

#define BIGINT_BINOP(a, b, op) do { \
		if ((a) == NULL || (b) == NULL) {\
			return NULL;\
		}\
		\
		int maxchunks = (a)->nchunk > (b)->nchunk ? (a)->nchunk : (b)->nchunk;\
		\
		struct bigint *res = bigint_init(maxchunks);\
		if (res == NULL) {\
			return NULL;\
		}\
		\
		for (int i = 0; i < maxchunks; i++) {\
			res->chunks[i] = bigint_index_with_padding((a), i) op bigint_index_with_padding((b), i);\
		}\
		\
		bigint_identify_pad_chunk_and_trim(res);\
		return res;\
	} while(0)

struct bigint *bigint_and(const struct bigint *a, const struct bigint *b) {
	BIGINT_BINOP(a, b, &);
}


struct bigint *bigint_or(const struct bigint *a, const struct bigint *b) {
	BIGINT_BINOP(a, b, |);
}


struct bigint *bigint_xor(const struct bigint *a, const struct bigint *b) {
	BIGINT_BINOP(a, b, ^);
}


void TestBigintBitwiseOperators(CuTest *tc) {
	enum {
		NHEXSTRING=7,
		RES_LEN = 32+1,
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

	for (int i = 0; i < NHEXSTRING; i++) {
		char res[RES_LEN];
		CuAssertIntEquals(tc, strlen(hexstrings[i]), bigint_to_msb_first_hexstring(bns[i], res));
		CuAssertStrEquals(tc, hexstrings[i], res);
		bigint_destroy(bns[i]);
	}
}


struct bigint *bigint_shift_left(const struct bigint *a, const struct bigint *b) {
	if (a == NULL || b == NULL) {
		return NULL;
	}

	if (bigint_msb(b, -1) == 1) {
		struct bigint *minus_b = bigint_negate(b);
		struct bigint *right_shifted_a = bigint_shift_right(a, minus_b);

		bigint_destroy(minus_b);
		return right_shifted_a;
	}

	if (b->nchunk > 1) {
		return NULL; // left shift with more than ULONG_MAX would in most cases result in out of memory, so do not try
	}

	int npre_chunks = b->chunks[0]/LONG_BIT;
	int non_chunk_shift = b->chunks[0]%LONG_BIT;

	struct bigint *res = bigint_init(a->nchunk+npre_chunks + (non_chunk_shift != 0));
	if (res == NULL) {
		return NULL;
	}

	unsigned long carry = 0;
	for (int i = 0; i < a->nchunk; i++) {
		res->chunks[npre_chunks + i] = (a->chunks[i]<<non_chunk_shift) | carry;
		carry = a->chunks[i] >> (LONG_BIT-non_chunk_shift);
	}
	res->chunks[npre_chunks+a->nchunk] = carry | (a->pad_chunk<<non_chunk_shift);

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_shift_right(const struct bigint *a, const struct bigint *b) {
	if (a == NULL || b == NULL) {
		return NULL;
	}

	if (bigint_msb(b, -1) == 1) {
		struct bigint *minus_b = bigint_negate(b);
		struct bigint *left_shfted_a = bigint_shift_left(a, minus_b);

		bigint_destroy(minus_b);
		return left_shfted_a;
	}

	int removed_chunks = b->chunks[0]/LONG_BIT;
	int non_chunk_shift = b->chunks[0]%LONG_BIT;

	int shift_more_than_a = removed_chunks >= a->nchunk;

	struct bigint *res = bigint_init(shift_more_than_a ? 1 : a->nchunk-removed_chunks);
	if (res == NULL) {
		return NULL;
	}

	if (shift_more_than_a) {
		res->chunks[0] = a->pad_chunk;
	} else {
		unsigned long carry = a->pad_chunk << (LONG_BIT-non_chunk_shift);
		for (int i = a->nchunk-1; i >= removed_chunks; i--) {
			res->chunks[i-removed_chunks] = (a->chunks[i]>>non_chunk_shift) | carry;
			carry = a->chunks[i]<<(LONG_BIT-non_chunk_shift);
		}
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


void TestBigintBitshift(CuTest *tc) {
	enum {
		NHEXSTRING = 9,
		RES_LEN = 64+1,
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

	for (int i = 0; i < NHEXSTRING; i++) {
		char res[RES_LEN];
		CuAssertIntEquals(tc, strlen(hexstrings[i]), bigint_to_msb_first_hexstring(bns[i], res));
		CuAssertStrEquals(tc, hexstrings[i], res);
		bigint_destroy(bns[i]);
	}
}


struct bigint *bigint_negate(const struct bigint *n) {
	if (n == NULL) {
		return NULL;
	}

	struct bigint *not_n = NULL, *minus_n = NULL;

	not_n = bigint_not(n);
	if (not_n == NULL) {
		goto cleanup;
	}

	minus_n = bigint_add(not_n, &bigint_one);

cleanup:
	bigint_destroy(not_n);
	return minus_n;
}


struct bigint *bigint_abs(const struct bigint *n) {
	if (n == NULL) {
		return NULL;
	}

	return bigint_msb(n, -1) == 1 ? bigint_negate(n) : bigint_copy(n);
}


struct bigint *bigint_add(const struct bigint *a, const struct bigint *b) {
	if (a == NULL || b == NULL) {
		return NULL;
	}

	int maxchunk = a->nchunk > b->nchunk ? a->nchunk : b->nchunk;
	struct bigint *res = bigint_init(maxchunk);
	if (res == NULL) {
		return NULL;
	}

	for (int i = 0, carry = 0; i < maxchunk; i++) {
		unsigned long sum_of_all_but_msbs = (bigint_index_with_padding(a, i) & ~LONG_MSB_MASK) + (bigint_index_with_padding(b, i) & ~LONG_MSB_MASK) + carry;
		unsigned long sum_of_msbs = bigint_msb(a, i) + bigint_msb(b, i) + (sum_of_all_but_msbs>>LONG_MSB);

		res->chunks[i] = (sum_of_all_but_msbs & ~LONG_MSB_MASK) | sum_of_msbs<<LONG_MSB;
		carry = sum_of_msbs>>1;
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}

struct bigint *bigint_subtract(const struct bigint *a, const struct bigint *b) {
	if (a == NULL || b == NULL) {
		return NULL;
	}

	struct bigint *minus_b = bigint_negate(b);
	if (minus_b == NULL) {
		return NULL;
	}

	struct bigint *a_minus_b = bigint_add(a, minus_b);

	bigint_destroy(minus_b);
	return a_minus_b;
}


void TestBigintArithmeticOperators(CuTest *tc) {
	enum {
		NHEXSTRING = 10,
		RES_LEN = 32+1,
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

	for (int i = 0; i < NHEXSTRING; i++) {
		char res[RES_LEN];
		CuAssertIntEquals(tc, strlen(hexstrings[i]), bigint_to_msb_first_hexstring(bns[i], res));
		CuAssertStrEquals(tc, hexstrings[i], res);
		bigint_destroy(bns[i]);
	}
}


void TestBigintErroneousInput(CuTest *tc) {
	struct bigint *single_chunk_n = bigint_from_long(pad_chunks[1]);
	CuAssertPtrNotNull(tc, single_chunk_n);
	struct bigint *multi_chunk_n = bigint_from_msb_first_hexstring("0ffffffffffffffffffffffffffffff", 0);
	CuAssertPtrNotNull(tc, multi_chunk_n);

	unsigned long some_long;

	char some_string[] = "something";


	CuAssertPtrEquals(tc, NULL, bigint_copy(NULL));

	CuAssertIntEquals(tc, EINVAL, bigint_to_long(NULL, NULL));
	CuAssertIntEquals(tc, EINVAL, bigint_to_long(single_chunk_n, NULL));
	CuAssertIntEquals(tc, EINVAL, bigint_to_long(multi_chunk_n, NULL));
	CuAssertIntEquals(tc, EINVAL, bigint_to_long(NULL, &some_long));
	CuAssertIntEquals(tc, ERANGE, bigint_to_long(multi_chunk_n, &some_long));

	CuAssertPtrEquals(tc, NULL, bigint_from_msb_first_hexstring(NULL, 0));
	CuAssertPtrEquals(tc, NULL, bigint_from_msb_first_hexstring(NULL, 1));

	CuAssertIntEquals(tc, -EINVAL, bigint_nnibble(NULL));

	CuAssertIntEquals(tc, -EINVAL, bigint_to_msb_first_hexstring(NULL, NULL));
	CuAssertIntEquals(tc, -EINVAL, bigint_to_msb_first_hexstring(single_chunk_n, NULL));
	CuAssertIntEquals(tc, -EINVAL, bigint_to_msb_first_hexstring(multi_chunk_n, NULL));
	CuAssertIntEquals(tc, -EINVAL, bigint_to_msb_first_hexstring(NULL, some_string));

	CuAssertPtrEquals(tc, NULL, bigint_not(NULL));

	CuAssertPtrEquals(tc, NULL, bigint_and(NULL, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_and(single_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_and(multi_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_and(NULL, single_chunk_n));
	CuAssertPtrEquals(tc, NULL, bigint_and(NULL, multi_chunk_n));

	CuAssertPtrEquals(tc, NULL, bigint_or(NULL, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_or(single_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_or(multi_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_or(NULL, single_chunk_n));
	CuAssertPtrEquals(tc, NULL, bigint_or(NULL, multi_chunk_n));

	CuAssertPtrEquals(tc, NULL, bigint_xor(NULL, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_xor(single_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_xor(multi_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_xor(NULL, single_chunk_n));
	CuAssertPtrEquals(tc, NULL, bigint_xor(NULL, multi_chunk_n));

	CuAssertPtrEquals(tc, NULL, bigint_negate(NULL));
	CuAssertPtrEquals(tc, NULL, bigint_abs(NULL));

	CuAssertPtrEquals(tc, NULL, bigint_add(NULL, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_add(single_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_add(multi_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_add(NULL, single_chunk_n));
	CuAssertPtrEquals(tc, NULL, bigint_add(NULL, multi_chunk_n));

	CuAssertPtrEquals(tc, NULL, bigint_subtract(NULL, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_subtract(single_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_subtract(multi_chunk_n, NULL));
	CuAssertPtrEquals(tc, NULL, bigint_subtract(NULL, single_chunk_n));
	CuAssertPtrEquals(tc, NULL, bigint_subtract(NULL, multi_chunk_n));


	bigint_destroy(single_chunk_n);
	bigint_destroy(multi_chunk_n);
}
