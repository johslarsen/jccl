#include "CuTest/CuTest.h"
#include "sort.h"
#include <stdio.h>
#include <stdlib.h>

#define ODD(n) (n & 1)

void swap(int *a, int *b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
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
			swap(first_even++, &a[i]);
		}
	}
}

static int issorted_odd_even(int *a, int na) {
	int odd = 1, *p = a;
	int *eoa = p+na;
	for (; p < eoa; p++) {
		if ( ODD(*p) ) {
			if ( !odd ) {
				return 0;
			}
		} else {
			odd = 0;
		}
	}

	return 1;
}

void TestSort_odd_even(CuTest *tc) {
	enum {
		NINT = 100,
	};

	int ints[NINT];
	int i;
	for (i = 0; i < NINT; i++) {
		ints[i] = rand();
	}

	CuAssertIntEquals(tc, 0, issorted_odd_even(ints, NINT));
	sort_odd_even(ints, NINT);
	CuAssertIntEquals(tc, 1, issorted_odd_even(ints, NINT));
}
