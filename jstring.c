#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "CuTest/CuTest.h"
#include "jstring.h"

#define LOWER_CASE_US_ALPHABET "abdefghijklmnopqrstuvwxyz"
#define ZERO_THROUGH_NINE "0123456789"

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
	char s[] = LOWER_CASE_US_ALPHABET;

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

#ifndef strnstr // this is based on the linux interface, https://www.kernel.org/doc/htmldocs/kernel-api/API-strnchr.html
char *strnstr(const char *s1, const char *s2, size_t len)
{
	if (s1 == NULL || s2 == NULL) {
		return NULL;
	}

	size_t s2_len = strlen(s2);
	while(len >= s2_len) {
		if (strncmp(s1, s2, s2_len) == 0) {
			return (char *)s1;
		}
		s1++, len--;
	}

	return NULL;
}
#endif /*strnstr*/
void TestStrnstr(CuTest *tc)
{
	char s[] = LOWER_CASE_US_ALPHABET;

	CuAssertStrEquals(tc, s, strnstr(s, "", 0));

	CuAssertStrEquals(tc, NULL, strnstr(s, ZERO_THROUGH_NINE, sizeof(s)));
	int i;
	for (i = 0; i < sizeof(s)-1; i++) {
		int j;
		for (j = 0; j < i; j++) {
			char str_with_this_char[2];
			str_with_this_char[0] = s[j];
			str_with_this_char[1] = '\0';
			CuAssertPtrEquals(tc, s+j, strnstr(s, str_with_this_char, i));
		}
		for ( ; j < sizeof(s)-1; j++) {
			char str_with_this_char[2];
			str_with_this_char[0] = s[j];
			str_with_this_char[1] = '\0';
			CuAssertPtrEquals(tc, NULL, strnstr(s, str_with_this_char, i));
		}
	}
}


static char *end_of_quoted_string_escaped_by_double_quotes(const char *s, size_t count, char quote)
{
	while (count > 0) {
		const char *next_quote = strnchr(s, count, quote);
		if (next_quote == NULL) {
			break;
		}
		if (count == 1 || next_quote[1] != quote) {
			return (char *)next_quote;
		}

		const char *char_after_escaped_quote = next_quote + 1 + 1; // + '"'
		count -= char_after_escaped_quote-s;
		s = char_after_escaped_quote;
	}

	return NULL;
}

char *end_of_quoted_string(const char *s, size_t count, char quote, char escape)
{
	if (s == NULL) {
		return NULL;
	}
	if (quote == escape) {
		return end_of_quoted_string_escaped_by_double_quotes(s, count, quote);
	}

	while (count > 0) {
		const char *next_quote = strnchr(s, count, quote);
		if (next_quote == NULL) {
			break;
		}

		const char *first_preceding_non_escape = next_quote-1;
		while (first_preceding_non_escape >= s && *first_preceding_non_escape == escape) {
			first_preceding_non_escape--;
		}
		size_t npreceding_escape = next_quote-(first_preceding_non_escape+1); // + character that is not an escape character

		if ((npreceding_escape & 1) == 0) {
			return (char *)next_quote;
		}

		const char *succeeding_char = next_quote + 1;
		count -= succeeding_char-s;
		s = succeeding_char;
	}

	return NULL;
}
void TestEnd_of_quoted_string(CuTest *tc)
{
	char s[] = "\"\\\"\\\\\\\"\\\\\\\\\"";
	size_t count = sizeof(s)-1; // -'\0'

	char *last_quote = s+count-1;

	CuAssertPtrEquals(tc, NULL, end_of_quoted_string(s, 0, '\"', '\\'));
	CuAssertPtrEquals(tc, s, end_of_quoted_string(s, count, '\"', '\\'));
	CuAssertPtrEquals(tc, NULL, end_of_quoted_string(s+1, count-2, '\"', '\\'));
	CuAssertPtrEquals(tc, last_quote, end_of_quoted_string(s+1, count-1, '\"', '\\'));
}
void TestEnd_of_quoted_stringWithQuoteAsEscape(CuTest *tc)
{
	char s[] = "\"\"\"\"";
	size_t count = sizeof(s)-1; // -'\0'

	char *last_quote = s+count-1;

	CuAssertPtrEquals(tc, NULL, end_of_quoted_string(s+1, count-2, '\"', '\"'));
	CuAssertPtrEquals(tc, last_quote, end_of_quoted_string(s+1, count-1, '\"', '\"'));
}
void TestEnd_of_quoted_stringWithEmptyString(CuTest *tc) {
	char s[] = "\"\"";
	size_t count = sizeof(s)-1; // -'\0'

	char *last_quote = s+count-1;

	CuAssertPtrEquals(tc, last_quote, end_of_quoted_string(s+1, count-1, '\"', '\\'));
	CuAssertPtrEquals(tc, last_quote, end_of_quoted_string(s+1, count-1, '\"', '\"'));
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

	char another_string_longer_than_4[] = "another";
	CuAssertStrEquals(tc, "ANOTcamelcase", strncpy_with_modifier(buf, another_string_longer_than_4, 4, toupper));
	CuAssertStrnEquals(tc, "ANOT", 4, strncpy_with_modifier(buf, another_string_longer_than_4, 4, toupper));
}
