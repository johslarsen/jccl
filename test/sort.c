#include "../sort.h"
#include "../unittest.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void sort_odd_even_test(void)
{
	enum {
		Narray = 1000,
		Na_max = 1000,
	};

	int na, *a;
	
	int i, j;
	for (i = 0; i < Narray; i++) {
		na = rand() % Na_max;
		a = (int *) malloc(sizeof(*a)*na);
		for (j = 0; j < na; j++) {
			a[j] = rand();
		}

		sort_odd_even(a, na);

		int post_odd = 0;
		for (j = 0; j < na; j++) {
			if (post_odd == 1) {
				UNITTEST((a[j] & 1) == 0);
			} else if ((a[j] & 1) == 0) {
				post_odd = 1;
			}
		}
	}


}

int main(void)
{
	int seed;
	FILE *dev_random = fopen("/dev/random", "r");
	if (dev_random == NULL) {
		seed = time(NULL);
	} else {
		fread(&seed, sizeof(int), 1, dev_random);
	}
	srand(seed);
	
	sort_odd_even_test();

	return 0;
}
