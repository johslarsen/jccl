#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bigint.h"

enum {
	NIBBLE_BIT = 4,
	NIBBLE_MASK = (1<<4)-1,

	LONG_BIT = sizeof(long) * CHAR_BIT,
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


static int bigint_msb(struct bigint *n, int i)
{
	return n->chunks[ i < 0 ? n->nchunk-1 : i ] >> LONG_MSB;
}


static unsigned long bigint_index_with_padding(struct bigint *n, int i)
{
	return i < n->nchunk ? n->chunks[i] : n->pad_chunk;
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
