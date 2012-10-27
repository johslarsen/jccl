#include "unittest.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void unittest_fail(const char *expr, const char *file, unsigned int line, const char *func)
{
	fprintf(stderr, "%s:%i: %s: Unittest '%s' failed\n", file, line, func, expr);
}


int acquire_seed(char *seed_from_argument)
{
	int seed;
	FILE *dev_random;
	if (seed_from_argument != NULL) {
		seed = atoi(seed_from_argument);
	} else if ((dev_random = fopen("/dev/random", "rb")) != NULL) {
		fread(&seed, sizeof(seed), 1, dev_random);
		fclose(dev_random);
	} else {
		seed = time(NULL);
	}

	return seed;
}


int acquire_and_print_seed(FILE *fp, char *seed_from_argument)
{
	int seed = acquire_seed(seed_from_argument);
	fprintf(fp, "seed: %d\n", seed);
	return seed;
}
