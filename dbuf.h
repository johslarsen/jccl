#ifndef DBUF_H
#define DBUF_H
#include <sys/types.h>

/* Dynamic Buffer ADT */

/* WARNING: the buffer is reallocated when it grows, so assume that all pointers
 * into the buffer points into memory that have been freed after calls to grow
 * functions and functions that could cause the buffer to grow */

struct dbuf {
	void *b;
	size_t n;
	size_t top;
};
#define DBUF_STATIC_INIT(initial_size) {\
	.b = NULL,\
	.n = initial_size,\
	.top = 0,\
}

/*
 * dbuf initializer. caller allocates the dbuf structure, and this just
 * duplicates the contents of a static initialized dbuf structure.
 */
extern void dbuf_init(struct dbuf *buf, size_t initial_n);

/*
 * dbuf destroyer. caller allocated the dbuf structure, so caller has the
 * responsibility for deallocating it. if buf == NULL it does nothing.
 */
extern void dbuf_destroy(struct dbuf *buf);

/*
 * grows the dbuf to new_n bytes.
 *
 * returns:
 *   buf == NULL || new_n <= buf->n --> EINVAL
 *   not enough space --> ENOMEM
 *   --> 0
 */
extern int dbuf_grow_to(struct dbuf *buf, size_t new_n);

/*
 * grows the dbuf by some factor. if buf->b == NULL it grows to buf->n bytes.
 *
 * returns:
 *   buf == NULL --> EINVAL
 *   not enough space --> ENOMEM
 *   --> 0
 */
extern int dbuf_grow(struct dbuf *buf);

/*
 * copies n byte of data from s to the end of the buffer. this may cause the
 * dbuf to grow.
 *
 * WARNING: this assumes all bytes from buf->top to buf->n-1 is free.
 *
 * returns:
 *   buf == NULL || s == NULL --> -EINVAL
 *   not enough space --> -ENOMEM
 *   --> offset from buf->b to the allocated memory
 */
extern ssize_t dbuf_push(struct dbuf *buf, const void *s, size_t n);

/*
 * copies and pops the n last byte in the buffer into s. assuming s is large
 * enough.
 *
 * returns:
 *   buf == NULL || s == NULL --> EINVAL
 *   n > buf->top --> ENODATA
 *   --> 0,
 */
extern int dbuf_pop(struct dbuf *buf, void *s, size_t n);

#endif /*DBUF_H*/
