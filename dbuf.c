#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "dbuf.h"
#include "CuTest/CuTest.h"

enum {
	GROW_FACTOR = 2,

	TEST_NINITIAL = 0x10,
};

void dbuf_init(struct dbuf *buf, size_t initial_n)
{
	if (buf != NULL) {
		static const struct dbuf template = DBUF_STATIC_INIT(0);
		memmove(buf, &template, sizeof(*buf));
		buf->n = initial_n;
	}
}
void dbuf_destroy(struct dbuf *buf)
{
	if (buf != NULL) {
		free(buf->b);

		// NOTE: buf is allocated by caller, so caller is responsible for deallocating it
	}
}
void TestDbufInitAndDestroy(CuTest *tc)
{
	struct dbuf sbuf = DBUF_STATIC_INIT(TEST_NINITIAL);
	static struct dbuf rbuf;

	dbuf_init(&rbuf, TEST_NINITIAL);

	int i;
	for (i = 0; i < sizeof(sbuf); i++) {
		CuAssertIntEquals(tc, *(((char *)&sbuf)+i), *(((char *)&rbuf)+i));
	}

	dbuf_init(&rbuf, 1337);
	CuAssertIntEquals(tc, 1337, rbuf.n);

	dbuf_destroy(&sbuf);
	dbuf_destroy(&rbuf);
}


int dbuf_grow_to(struct dbuf *buf, size_t new_n)
{
	if (buf == NULL || new_n < buf->n) {
		return EINVAL;
	}

	void *new_b = realloc(buf->b, new_n);
	if (new_b == NULL) {
		return ENOMEM;
	}

	buf->b = new_b;
	buf->n = new_n;

	return 0;
}
void TestDbuf_grow_to(CuTest *tc)
{
	enum {
		A_PRIME = 13,
		A_LARGER_PRIME = 127,
	};
	struct dbuf buf = DBUF_STATIC_INIT(0);

	CuAssertIntEquals(tc, 0, dbuf_grow_to(&buf, A_PRIME));
	CuAssertIntEquals(tc, A_PRIME, buf.n);
	CuAssertPtrNotNull(tc, buf.b);

	CuAssertIntEquals(tc, 0, dbuf_grow_to(&buf, A_LARGER_PRIME));
	CuAssertIntEquals(tc, A_LARGER_PRIME, buf.n);
	CuAssertPtrNotNull(tc, buf.b);

	CuAssertIntEquals(tc, EINVAL, dbuf_grow_to(NULL, 13));
	CuAssertIntEquals(tc, EINVAL, dbuf_grow_to(&buf, A_PRIME));

	dbuf_destroy(&buf);
}


int dbuf_grow(struct dbuf *buf)
{
	if (buf == NULL) {
		return EINVAL;
	}

	size_t new_n = buf->n * (buf->b == NULL ? 1 : GROW_FACTOR);
	return dbuf_grow_to(buf, new_n);
}
void TestDbuf_grow(CuTest *tc)
{
	struct dbuf buf = DBUF_STATIC_INIT(TEST_NINITIAL);

	CuAssertIntEquals(tc, 0, dbuf_grow(&buf));
	CuAssertPtrNotNull(tc, buf.b);
	CuAssertIntEquals(tc, TEST_NINITIAL, buf.n);

	CuAssertIntEquals(tc, 0, dbuf_grow(&buf));
	CuAssertPtrNotNull(tc, buf.b);
	CuAssertIntEquals(tc, TEST_NINITIAL*GROW_FACTOR, buf.n);

	dbuf_destroy(&buf);
}


ssize_t dbuf_append(struct dbuf *buf, const void *s, size_t n)
{
	if (buf == NULL || s == NULL) {
		return -EINVAL;
	}

	size_t new_next_append = buf->next_append + n;
	if (buf->b == NULL || new_next_append > n) {
		int retval = dbuf_grow(buf);
		if (retval != 0) {
			retval = dbuf_grow_to(buf, new_next_append);
			if (retval != 0) {
				return -retval;
			}
		}
	}
	assert(buf->n >= new_next_append);

	size_t offset =buf->next_append;

	memmove(buf->b+offset, s, n);
	buf->next_append += n;

	return offset;
}
void TestDbuf_append(CuTest *tc)
{
#define A_STRING "0123456789" // its size should be <TEST_NINITIAL
#define ANOTHER_STRING "abcdefghij" // the size of both string should be >TEST_NINITIAL
	struct dbuf buf = DBUF_STATIC_INIT(TEST_NINITIAL);

	ssize_t offset_a_string = dbuf_append(&buf, A_STRING, sizeof(A_STRING));
	CuAssertIntEquals(tc, 0, offset_a_string);
	CuAssertStrEquals(tc, A_STRING, (char *)buf.b+offset_a_string);
	CuAssertIntEquals(tc, sizeof(A_STRING), buf.next_append);

	ssize_t offset_another_string = dbuf_append(&buf, ANOTHER_STRING, sizeof(ANOTHER_STRING));
	CuAssertIntEquals(tc, sizeof(A_STRING), offset_another_string);
	CuAssertStrEquals(tc, A_STRING, (char *)buf.b+offset_a_string);
	CuAssertStrEquals(tc, ANOTHER_STRING, (char *)buf.b+offset_another_string);
	CuAssertIntEquals(tc, sizeof(A_STRING)+sizeof(ANOTHER_STRING), buf.next_append);

	dbuf_destroy(&buf);
}
void TestDbuf_appendWithGrowTo(CuTest *tc)
{
	struct dbuf buf = DBUF_STATIC_INIT(13); // some prime, assuming the OS maximum allocatable buffer is some power of 2, and we want to avoid it by having a maximum allocatable byte of buf including a prime factor

	size_t n_before_append;
	do {
		int retval;
		do {
			retval = dbuf_grow(&buf); // allocate whatever space it can get by multiplying with GROW_FACTOR
		} while (retval == 0);

		buf.next_append = buf.n; // simulate a full buffer
		n_before_append = buf.n;

		ssize_t offset_a_string = dbuf_append(&buf, A_STRING, sizeof(A_STRING));
		CuAssertIntEquals(tc, n_before_append, offset_a_string);
		CuAssertStrEquals(tc, A_STRING, (char *)buf.b+offset_a_string);
		CuAssertIntEquals(tc, n_before_append+sizeof(A_STRING), buf.next_append);
	} while(buf.n == n_before_append*GROW_FACTOR); // if this happens we have just retested growing using dbuf_grow, repeat until dbuf_grow_to is used

	dbuf_destroy(&buf);
}
