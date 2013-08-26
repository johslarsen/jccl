#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "CuTest/CuTest.h"
#include "jstring.h"

#ifndef strnchr // this is based on the linux interface, https://www.kernel.org/doc/htmldocs/kernel-api/API-strnchr.html
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
#endif /*strnchr*/
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


char *end_of_quoted_string(const char *s, size_t count, char quote, char escape)
{
	if (s == NULL) {
		return NULL;
	}

	const char *next_quote = NULL;
	while (count > 0) {
		next_quote = strnchr(s, count, quote);
		if (next_quote == NULL) {
			break;
		}

		const char *first_preceding_non_escape = next_quote-1;
		while (first_preceding_non_escape >= s && *first_preceding_non_escape == escape) {
			first_preceding_non_escape--;
		}
		size_t npreceding_escape = next_quote-(first_preceding_non_escape+1); // + character that is not an escape character

		if ((npreceding_escape & 1) == 0) {
			// all preceding escape characters are escaped
			break;
		}

		const char *succeeding_char = next_quote + 1;
		count -= succeeding_char-s;
		s = succeeding_char;
	}

	return (char *)next_quote;
}
void TestEnd_of_quoted_string(CuTest *tc)
{
	char s[] = "\"\\\"\\\\\\\"\\\\\\\\\"";
	size_t count = sizeof(s)-1; // -'\0'

	char *last_quote = s+count-1;

	CuAssertPtrEquals(tc, NULL, end_of_quoted_string(s, 0, '\"', '\\'));
	CuAssertPtrEquals(tc, s, end_of_quoted_string(s, count, '\"', '\\'));
	CuAssertPtrEquals(tc, last_quote, end_of_quoted_string(s+1, count, '\"', '\\'));
}


char *strncpy_with_modifier(char *dest, const char *src, size_t count, character_modifier modifier)
{
	if (dest == NULL || src == NULL) {
		return dest;
	}

	char *original_dest = dest;

	for (; count > 0 && *src != '\0'; count--) {
		*dest++ = modifier(*src++);
	}
	if (count > 0) {
		*dest = '\0';
	}

	return original_dest;
}
void TestStrncpy_with_modifier(CuTest *tc)
{
	char buf[200];
	char some_camelcase[] = "SoMeCaMeLCaSe";
	CuAssertStrEquals(tc, "somecamelcase", strncpy_with_modifier(buf, some_camelcase, sizeof(some_camelcase), tolower));

	buf[4] = '\0';
	char another_string_longer_than_4[] = "another";
	CuAssertStrEquals(tc, "ANOT", strncpy_with_modifier(buf, another_string_longer_than_4, 4, toupper));
}
