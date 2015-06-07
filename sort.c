#include "CuTest/CuTest.h"
#include "sort.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ODD(n) (n & 1)

inline static void swap(void *a, void *b, size_t size) {
	char *a_byte = (char *)a;
	char *b_byte = (char *)b;
	do {
		char tmp = *a_byte;
		*a_byte++ = *b_byte;
		*b_byte++ = tmp;
	} while (--size > 0);
}

void sort_odd_even(int *a, int na) {
	int i, *first_even;

	// locate the first even number
	for (i = 0; i < na; i++) {
		if ( !ODD(a[i]) ) { // even
			first_even = &a[i];
			break;
		}
	}

	for ( ; i < na; i++) {
		if ( ODD(a[i]) ) { // odd
			swap(first_even++, &a[i], sizeof(*a));
		}
	}
}

static int issorted(void *base, size_t nmemb, size_t size, bsort_categorizer categorizer) {
	size_t max = 0;

	void *eobase = base + (nmemb*size);
	for (void *p = base; p < eobase; p += size) {
		size_t bucket = categorizer(p);
		if (bucket < max) {
			return 0;
		} else {
			max = bucket;
		}
	}
	return 1;
}

static size_t bsort_categorize_odd_even(const void *a) {
	return !ODD((*((const int *)a)));
}

void TestSort_odd_even(CuTest *tc) {
	enum {
		NINT = 100,
	};

	int ints[NINT];
	for (int i = 0; i < NINT; i++) {
		ints[i] = rand();
	}

	CuAssertIntEquals(tc, 0, issorted(ints, NINT, sizeof(*ints), bsort_categorize_odd_even));
	sort_odd_even(ints, NINT);
	CuAssertIntEquals(tc, 1, issorted(ints, NINT, sizeof(*ints), bsort_categorize_odd_even));
}


void bsort_partial(void *base, size_t nmemb, size_t size, bsort_categorizer categorizer, size_t nbucket, void *buckets[], void *base_unsorted) {
	assert((base_unsorted-base) % size == 0);
	void *eobase = base + (nmemb*size);
	for (void *p = base_unsorted; p < eobase; p += size) {
		size_t bucket = categorizer(p);
		assert(bucket < nbucket);

		void *slot = p;
		for (int i = nbucket-1; i > bucket; i--) {
			if (slot != buckets[i]) {
				swap(buckets[i], slot, size);
				slot = buckets[i];
			}
			buckets[i] += size;
		}
	}
}

inline static void *bsort_first_unsorted_element(void *base, size_t nmemb, size_t size, bsort_categorizer categorizer, size_t nbucket, void *buckets[]) {
	size_t last_bucket = 0;
	buckets[last_bucket] = base;

	void *p, *eobase = base + (nmemb*size);
	for (p = base; p < eobase; p += size) {
		size_t bucket = categorizer(p);
		assert(bucket < nbucket);

		if (bucket < last_bucket) {
			break; // i.e. from here on out base is unsorted
		} else if (bucket > last_bucket) {
			for (int i = last_bucket+1; i <= bucket; i++) {
				buckets[i] = p;
			}
			last_bucket = bucket;
		}
	}
	for (int i = last_bucket+1; i < nbucket; i++) {
		buckets[i] = p;
	}

	return p;
}

void bsort(void *base, size_t nmemb, size_t size, bsort_categorizer categorizer, size_t nbucket, void *buckets[]) {
	void *base_unsorted = bsort_first_unsorted_element(base, nmemb, size, categorizer, nbucket, buckets);
	return bsort_partial(base, nmemb, size, categorizer, nbucket, buckets, base_unsorted);
}

size_t bsort_categorize_least_significant_nibble(const void *a) {
	return (*((const int *)a))&0xf;
}

void TestBsort_odd_even(CuTest *tc) {
	enum {
		NINT = 100,
	};

	int ints[NINT];
	for (int i = 0; i < NINT; i++) {
		ints[i] = rand();
	}

	int *buckets[0x10];

	CuAssertIntEquals(tc, 0, issorted(ints, NINT/2, sizeof(*ints), bsort_categorize_least_significant_nibble));
	bsort(ints, NINT/2, sizeof(*ints), bsort_categorize_least_significant_nibble, 0x10, (void **)buckets);
	CuAssertIntEquals(tc, 1, issorted(ints, NINT/2, sizeof(*ints), bsort_categorize_least_significant_nibble));
	bsort_partial(ints, NINT, sizeof(*ints), bsort_categorize_least_significant_nibble, 0x10, (void **)buckets, ints + (NINT/2));
	CuAssertIntEquals(tc, 1, issorted(ints, NINT, sizeof(*ints), bsort_categorize_least_significant_nibble));
}
