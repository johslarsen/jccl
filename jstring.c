#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "CuTest/CuTest.h"

#ifndef strnchr
char *strnchr(const char *s, size_t count, int c)
{
	if (s == NULL || count == 0) {
		return NULL;
	}

	const char *end_of_str = s+count;
	do {
		if (*s == (char)c) {
			return (char *)s;
		}
	} while (++s < end_of_str);

	return NULL;
}


void TestStrnchr(CuTest *tc)
{
	char s[] = "abcdefghijklmnopqrstuvwxyz";

	CuAssertPtrEquals(tc, NULL, strnchr(s, sizeof(s), -1));
	int i;
	for (i = 0; i < sizeof(s); i++) {
		int j;
		for (j = 0; j < i; j++) {
			CuAssertPtrEquals(tc, s+j, strnchr(s, i, s[j]));
		}
		for ( ; j < sizeof(s); j++) {
			CuAssertPtrEquals(tc, NULL, strnchr(s, i, s[j]));
		}
	}
}
#endif /*strnchr*/
