#include "unittest.h"
#include <stdio.h>

void unittest_fail(const char *expr, const char *file, unsigned int line, const char *func)
{
	fprintf(stderr, "%s:%i: %s: Unittest '%s' failed\n", file, line, func, expr);
}
