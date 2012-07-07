#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigint.h"

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

const char *bigint_hex_charset = "0123456789abcdef";
const unsigned long pad_chunks[] = {0, ULONG_MAX};

struct bigint {
	unsigned long *chunks;
	int nchunk;
	unsigned long pad_chunk;
};

static struct bigint *bigint_init(int nchunk)
{
	struct bigint *n = (struct bigint *)malloc(sizeof(*n));
	if (n == NULL) {
		return NULL;
	}

	n->nchunk = nchunk;
	n->chunks = (unsigned long *)calloc(sizeof(long), nchunk);
	if (n->chunks == NULL) {
		bigint_destroy(n);
		return NULL;
	}

	return n;
}


static unsigned long bigint_index_with_padding(struct bigint *n, int i)
{
	return i < n->nchunk ? n->chunks[i] : n->pad_chunk;
}


static int bigint_msb(struct bigint *n, int i)
{
	return bigint_index_with_padding(n, i < 0 ? n->nchunk-1 : i) >> LONG_MSB;
}


void bigint_destroy(struct bigint *n)
{
	if (n != NULL) {
		free(n->chunks);
		free(n);
	}
}


static void bigint_identify_pad_chunk_and_trim(struct bigint *n)
{
	if (n == NULL) {
		return;
	}

	int msb = bigint_msb(n, -1);
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
		n->nchunk = new_nchunk;
		unsigned long *new_chunks = (unsigned long *)realloc(n->chunks, n->nchunk);
		if (new_chunks != NULL) {
			n->chunks = new_chunks;
		}
	}
}


struct bigint *bigint_from_long(unsigned long n)
{
	struct bigint *bigint = bigint_init(1);
	if (bigint == NULL) {
		return NULL;
	}

	bigint->chunks[0] = n;

	bigint_identify_pad_chunk_and_trim(bigint);
	return bigint;
}


struct bigint *bigint_from_msb_first_hexstring(const char *s, int nchar) {
	if (s == NULL) {
		return NULL;
	}

	if (nchar == 0) {
		nchar = strlen(s);
	}

	int nchunk = nchar/NNIBBLE_IN_LONG;

	int empty_space_in_last_chunk = nchar%NNIBBLE_IN_LONG != 0;
	if(empty_space_in_last_chunk) {
		nchunk += 1;
	}

	struct bigint *n = bigint_init(nchunk);
	if (n == NULL) {
		return NULL;
	}

	unsigned long nibble;
	int i;
	for (i = 0; i < nchar; i++) {
		char *hexchar = strchr(bigint_hex_charset, s[nchar-1-i]);
		if (hexchar == NULL) {
			goto error;
		}


		nibble = hexchar-bigint_hex_charset;

		n->chunks[i/NNIBBLE_IN_LONG] |= nibble<<(NIBBLE_BIT * (i%NNIBBLE_IN_LONG));
	}

	if (empty_space_in_last_chunk && (nibble>>NIBBLE_MSB) == 1) {
		n->chunks[n->nchunk-1] |= pad_chunks[1] << (nchar%NNIBBLE_IN_LONG);
	}

	bigint_identify_pad_chunk_and_trim(n);
	return n;
error:
	bigint_destroy(n);
	return NULL;
}


char *bigint_to_msb_first_hexstring(struct bigint *n)
{
	if (n == NULL) {
		return NULL;
	}

	char *s = (char *)malloc(sizeof(*s)*n->nchunk*NNIBBLE_IN_LONG + 1);
	if (s == NULL) {
		return NULL;
	}

	int i;
	char *start_of_chunk = s;
	for (i = n->nchunk-1; i >= 0; i--) {
		unsigned long chunk = n->chunks[i];

		char *p;
		for (p = start_of_chunk+NNIBBLE_IN_LONG-1; p >= start_of_chunk; p--) {
			*p = bigint_hex_charset[chunk & NIBBLE_MASK];
			chunk >>= NIBBLE_BIT;
		}

		start_of_chunk += NNIBBLE_IN_LONG;
	}
	*start_of_chunk = '\0';

	return s;
}


int bigint_compare(struct bigint *a, struct bigint *b) {
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

	int i;
	for (i = a->nchunk-1; i >= 0; i--) {
		if (a->chunks[i] != b->chunks[i]) {
			return a->chunks[i] > b->chunks[i] ? 1 : -1;
		}
	}

	return 0;
}


struct bigint *bigint_not(struct bigint *n)
{
	if (n == NULL) {
		return NULL;
	}

	struct bigint *res = bigint_init(n->nchunk);

	int i;
	for (i = 0; i < n->nchunk; i++) {
		res->chunks[i] = ~n->chunks[i];
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_and(struct bigint *a, struct bigint *b)
{
	if (a == NULL || b == NULL) {
		return NULL;
	}

	int maxchunks = a->nchunk > b->nchunk ? a->nchunk : b->nchunk;

	struct bigint *res = bigint_init(maxchunks);
	if (res == NULL) {
		return NULL;
	}

	int i;
	for (i = 0; i < maxchunks; i++) {
		res->chunks[i] = bigint_index_with_padding(a, i) & bigint_index_with_padding(b, i);
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_or(struct bigint *a, struct bigint *b)
{
	if (a == NULL || b == NULL) {
		return NULL;
	}

	int maxchunks = a->nchunk > b->nchunk ? a->nchunk : b->nchunk;

	struct bigint *res = bigint_init(maxchunks);
	if (res == NULL) {
		return NULL;
	}

	int i;
	for (i = 0; i < maxchunks; i++) {
		res->chunks[i] = bigint_index_with_padding(a, i) | bigint_index_with_padding(b, i);
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_xor(struct bigint *a, struct bigint *b)
{
	if (a == NULL || b == NULL) {
		return NULL;
	}

	int maxchunks = a->nchunk > b->nchunk ? a->nchunk : b->nchunk;

	struct bigint *res = bigint_init(maxchunks);
	if (res == NULL) {
		return NULL;
	}

	int i;
	for (i = 0; i < maxchunks; i++) {
		res->chunks[i] = bigint_index_with_padding(a, i) ^ bigint_index_with_padding(b, i);
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_shift_left(struct bigint *a, struct bigint *b)
{
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
	int i;
	for (i = 0; i < a->nchunk; i++) {
		res->chunks[npre_chunks + i] = (a->chunks[i]<<non_chunk_shift) | carry;
		carry = a->chunks[i] >> (LONG_BIT-non_chunk_shift);
	}
	res->chunks[npre_chunks+a->nchunk] = carry | (a->pad_chunk<<non_chunk_shift);

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_shift_right(struct bigint *a, struct bigint *b)
{
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
		int i;
		unsigned long carry = a->pad_chunk << (LONG_BIT-non_chunk_shift);
		for (i = a->nchunk-1; i >= removed_chunks; i--) {
			res->chunks[i-removed_chunks] = (a->chunks[i]>>non_chunk_shift) | carry;
			carry = a->chunks[i]<<(LONG_BIT-non_chunk_shift);
		}
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_negate(struct bigint *n) {
	if (n == NULL) {
		return NULL;
	}

	struct bigint *bn_one = NULL, *not_n = NULL, *minus_n = NULL;

	bn_one = bigint_from_long(1);
	if (bn_one == NULL) {
		goto cleanup;
	}

	not_n = bigint_not(n);
	if (not_n == NULL) {
		goto cleanup;
	}

	minus_n = bigint_add(not_n, bn_one);

cleanup:
	bigint_destroy(not_n);
	bigint_destroy(bn_one);
	return minus_n;
}


struct bigint *bigint_add(struct bigint *a, struct bigint *b)
{
	if (a == NULL || b == NULL) {
		return NULL;
	}

	int maxchunk = a->nchunk > b->nchunk ? a->nchunk : b->nchunk;
	struct bigint *res = bigint_init(maxchunk);
	if (res == NULL) {
		return NULL;
	}

	int i, carry = 0;
	for (i = 0; i < maxchunk; i++) {
		unsigned long sum_of_all_but_msbs = (bigint_index_with_padding(a, i) & ~LONG_MSB_MASK) + (bigint_index_with_padding(b, i) & ~LONG_MSB_MASK) + carry;
		unsigned long sum_of_msbs = bigint_msb(a, i) + bigint_msb(b, i) + (sum_of_all_but_msbs>>LONG_MSB);

		res->chunks[i] = (sum_of_all_but_msbs & ~LONG_MSB_MASK) | sum_of_msbs<<LONG_MSB;
		carry = sum_of_msbs>>1;
	}

	bigint_identify_pad_chunk_and_trim(res);
	return res;
}


struct bigint *bigint_subtract(struct bigint *a, struct bigint *b)
{
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
