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

void dbuf_init(struct dbuf *buf, size_t initial_n) {
	if (buf != NULL) {
		static const struct dbuf template = DBUF_STATIC_INIT(0);
		memmove(buf, &template, sizeof(template));
		buf->n = initial_n;
	}
}
void dbuf_destroy(struct dbuf *buf) {
	if (buf != NULL) {
		free(buf->b);

		// NOTE: buf is allocated by caller, so caller is responsible for deallocating it
	}
}
void TestDbufInitAndDestroy(CuTest *tc) {
	struct dbuf sbuf = DBUF_STATIC_INIT(TEST_NINITIAL);
	struct dbuf rbuf;

	dbuf_init(&rbuf, TEST_NINITIAL);

	CuAssertPtrEquals(tc, sbuf.b, rbuf.b);
	CuAssertIntEquals(tc, sbuf.n, rbuf.n);
	CuAssertIntEquals(tc, sbuf.top, rbuf.top);

	dbuf_init(&rbuf, 1337);
	CuAssertIntEquals(tc, 1337, rbuf.n);

	dbuf_destroy(&sbuf);
	dbuf_destroy(&rbuf);
}


int dbuf_grow_to(struct dbuf *buf, size_t new_n) {
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
void TestDbuf_grow_to(CuTest *tc) {
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


int dbuf_grow(struct dbuf *buf) {
	if (buf == NULL) {
		return EINVAL;
	}

	size_t new_n;
	if (buf->b != NULL) {
		new_n = buf->n * GROW_FACTOR;
	} else {
		if (buf->n != 0) {
			new_n = buf->n;
		} else {
			new_n = sizeof(void *); // fall back to architecture word size
		}
	}
	return dbuf_grow_to(buf, new_n);
}
void TestDbuf_grow(CuTest *tc) {
	struct dbuf buf = DBUF_STATIC_INIT(TEST_NINITIAL);

	CuAssertIntEquals(tc, 0, dbuf_grow(&buf));
	CuAssertPtrNotNull(tc, buf.b);
	CuAssertIntEquals(tc, TEST_NINITIAL, buf.n);

	CuAssertIntEquals(tc, 0, dbuf_grow(&buf));
	CuAssertPtrNotNull(tc, buf.b);
	CuAssertIntEquals(tc, TEST_NINITIAL*GROW_FACTOR, buf.n);

	dbuf_destroy(&buf);
}
void TestDbuf_growNinitialIsZero(CuTest *tc) {
	struct dbuf buf = DBUF_STATIC_INIT(0);

	CuAssertIntEquals(tc, 0, dbuf_grow(&buf));
	CuAssertPtrNotNull(tc, buf.b);
	CuAssertTrue(tc, buf.n > 0);

	dbuf_destroy(&buf);
}


ssize_t dbuf_push(struct dbuf *buf, const void *s, size_t n) {
	if (buf == NULL || s == NULL) {
		return -EINVAL;
	}

	size_t new_top = buf->top + n;
	if (buf->b == NULL || new_top > n) {
		int retval = dbuf_grow(buf);
		if (retval != 0) {
			retval = dbuf_grow_to(buf, new_top);
			if (retval != 0) {
				return -retval;
			}
		}
	}
	assert(buf->n >= new_top);

	size_t offset =buf->top;

	memmove(buf->b+offset, s, n);
	buf->top += n;

	return offset;
}
void Testdbuf_push(CuTest *tc) {
#define A_STRING "0123456789" // its size should be <TEST_NINITIAL
#define ANOTHER_STRING "abcdefghij" // the size of both string should be >TEST_NINITIAL
	struct dbuf buf = DBUF_STATIC_INIT(TEST_NINITIAL);

	CuAssertIntEquals(tc, -EINVAL, dbuf_push(NULL, A_STRING, sizeof(A_STRING)));
	CuAssertIntEquals(tc, -EINVAL, dbuf_push(&buf, NULL, sizeof(A_STRING)));
	CuAssertIntEquals(tc, 0, dbuf_push(&buf, A_STRING, 0));
	CuAssertIntEquals(tc, 0, dbuf_push(&buf, A_STRING, 0));

	ssize_t offset_a_string = dbuf_push(&buf, A_STRING, sizeof(A_STRING));
	CuAssertIntEquals(tc, 0, offset_a_string);
	CuAssertStrEquals(tc, A_STRING, (char *)buf.b+offset_a_string);
	CuAssertIntEquals(tc, sizeof(A_STRING), buf.top);

	ssize_t offset_another_string = dbuf_push(&buf, ANOTHER_STRING, sizeof(ANOTHER_STRING));
	CuAssertIntEquals(tc, sizeof(A_STRING), offset_another_string);
	CuAssertStrEquals(tc, A_STRING, (char *)buf.b+offset_a_string);
	CuAssertStrEquals(tc, ANOTHER_STRING, (char *)buf.b+offset_another_string);
	CuAssertIntEquals(tc, sizeof(A_STRING)+sizeof(ANOTHER_STRING), buf.top);

	dbuf_destroy(&buf);
}
void Testdbuf_pushWithGrowTo(CuTest *tc) {
	struct dbuf buf = DBUF_STATIC_INIT(13); // some prime, assuming the OS maximum allocatable buffer is some power of 2, and we want to avoid it by having a maximum allocatable byte of buf including a prime factor

	size_t n_before_append;
	do {
		int retval;
		do {
			retval = dbuf_grow(&buf); // allocate whatever space it can get by multiplying with GROW_FACTOR
		} while (retval == 0);

		buf.top = buf.n; // simulate a full buffer
		n_before_append = buf.n;

		ssize_t offset_a_string = dbuf_push(&buf, A_STRING, sizeof(A_STRING));
		CuAssertIntEquals(tc, n_before_append, offset_a_string);
		CuAssertStrEquals(tc, A_STRING, (char *)buf.b+offset_a_string);
		CuAssertIntEquals(tc, n_before_append+sizeof(A_STRING), buf.top);
	} while(buf.n == n_before_append*GROW_FACTOR); // if this happens we have just retested growing using dbuf_grow, repeat until dbuf_grow_to is used

	dbuf_destroy(&buf);
}


int dbuf_pop(struct dbuf *buf, void *s, size_t n) {
	if (buf == NULL || s == NULL) {
		return EINVAL;
	}
	if (n == 0) {
		return 0;
	}

	if (n > buf->top) {
		return ENODATA;
	}

	buf->top -= n;
	memmove(s, buf->b+buf->top, n);

	return 0;
}
void TestDbuf_pop(CuTest *tc) {
	struct dbuf buf = DBUF_STATIC_INIT(TEST_NINITIAL);

	static char buffer[TEST_NINITIAL];
	CuAssertIntEquals(tc, EINVAL, dbuf_pop(NULL, buffer, sizeof(buffer)));
	CuAssertIntEquals(tc, EINVAL, dbuf_pop(&buf, NULL, sizeof(buffer)));
	CuAssertIntEquals(tc, ENODATA, dbuf_pop(&buf, buffer, sizeof(buffer)));

	CuAssertIntEquals(tc, 0, dbuf_push(&buf, A_STRING, sizeof(A_STRING)));
	CuAssertIntEquals(tc, sizeof(A_STRING), dbuf_push(&buf, ANOTHER_STRING, sizeof(ANOTHER_STRING)));

	CuAssertIntEquals(tc, 0, dbuf_pop(&buf, buffer, sizeof(ANOTHER_STRING)));
	CuAssertStrEquals(tc, ANOTHER_STRING, buffer);
	CuAssertIntEquals(tc, 0, dbuf_pop(&buf, buffer, sizeof(A_STRING)));
	CuAssertStrEquals(tc, A_STRING, buffer);

	dbuf_destroy(&buf);
}
