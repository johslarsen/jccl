#include "dmath_test.h"
#include "list_test.h"
#include <stdio.h>
#include <time.h>

int main(void)
{
	FILE *devrandom;
	if ((devrandom = fopen("/dev/random", "r")) != NULL) {
		int seed;
		fread(&seed, sizeof(int), 1, devrandom);
		srand(seed);
		fclose(devrandom);
	} else {
		srand(time(NULL));
	}

	fibonacci_test();
	gcd_lcm_arerelativeprime_test();
	chinese_remainder_test();
	isprime_test();

	list_test();
	list_iterator_test();

	return 0;
}
