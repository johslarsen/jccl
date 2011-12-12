#include "sort.h"

void swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void sort_odd_even(int *a, int na)
{
	int i, *first_even;

	// locate the first even number
	for (i = 0; i < na; i++) {
		if ((a[i] & 1) == 0) { // even
			first_even = &a[i];
			break;
		}
	}

	for ( ; i < na; i++) {
		if (a[i] & 1) { // odd
			swap(first_even++, &a[i]);
		}
	}
}
