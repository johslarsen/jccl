#ifndef BIGINT_H_
#define BIGINT_H_
#include <stdlib.h>

struct bigint;


/*
 * bigint destructor. if n == NULL it does nothing.
 */
extern void bigint_destroy(struct bigint *n);


/*
 * make a copy of a bigint.
 *
 * returns:
 *   n == NULL --> NULL
 *   error --> NULL
 *   --> *(new bigint)
 */
extern struct bigint *bigint_copy(const struct bigint *n);


/*
 * create a bigint from a long.
 *
 * returns:
 *   error --> NULL
 *   --> *(new bigint)
 */
extern struct bigint *bigint_from_long(unsigned long n);


/*
 * convert a bigint into a long. the result is stored in result.
 *
 * returns:
 *   n == NULL || result == NULL --> EINVAL
 *   n to big to fit into long --> ERANGE
 *   --> 0
 */
extern int bigint_to_long(const struct bigint *n, unsigned long *result);


/*
 * this is the charset used for strings of hexadecimal characters (aka.
 * hexstrings). it is {0..9}{a..z}, and the charset is stored so that
 * bigint_hex_charset[0..15] gives the correct character.
 */
extern const char *bigint_hex_charset;


/*
 * create a bigint from a string of hexadecimal characters ordered with the
 * most significant bit first. the result is stored in s.
 *
 * ns is the length of s (excluding null character), if ns == 0 strlen(s) is
 * used instead.
 *
 * returns:
 *   s == NULL --> NULL
 *   error --> NULL
 *   --> *(new bigint)
 */
extern struct bigint *bigint_from_msb_first_hexstring(const char *s, size_t ns);


/*
 * number of nibbles in a bigint. a nibble is a number represented by 4 bits
 * (hex character).
 *
 * returns:
 *   n == NULL --> -EINVAL
 *   --> nbit(n)/4
 */
extern int bigint_nnibble(const struct bigint *n);


/*
 * convert a bigint into a string of hexadecimal characters ordered with the
 * most significant bit first. the resulting string is put into s.
 *
 * returns:
 *   n == NULL || s == NULL --> -EINVAL
 *   error --> -error
 *   --> strlen(s)
 */
extern int bigint_to_msb_first_hexstring(const struct bigint *n, char *s);


/*
 * function to compare two bigints. any NULL arguments is interpreted as
 * -infinity.
 *
 * returns:
 *   a > b --> 1
 *   a == b --> 0
 *   a < b --> -1
 */
extern int bigint_compare(const struct bigint *a, const struct bigint *b);


/*
 * the rest of the functions are bitwise or arithmetical operators. the
 * operator used for the operation in C, or if C does not support this
 * operation; the logical one, is included as a comment behind the declaration.
 *
 * they all return:
 *   any operand == NULL --> NULL
 *   error --> NULL
 *   --> *(new bigint)
 */


extern struct bigint *bigint_not(const struct bigint *n); // ~n
extern struct bigint *bigint_and(const struct bigint *a, const struct bigint *b); // a&b
extern struct bigint *bigint_or(const struct bigint *a, const struct bigint *b); // a|b
extern struct bigint *bigint_xor(const struct bigint *a, const struct bigint *b); // a^b

extern struct bigint *bigint_shift_left(const struct bigint *a, const struct bigint *b); // a<<b
extern struct bigint *bigint_shift_right(const struct bigint *a, const struct bigint *b); // a>>b

extern struct bigint *bigint_negate(const struct bigint *n); // -n
extern struct bigint *bigint_abs(const struct bigint *n); // |n|

extern struct bigint *bigint_add(const struct bigint *a, const struct bigint *b); // a+b
extern struct bigint *bigint_subtract(const struct bigint *a, const struct bigint *b); // a-b

#endif /*BIGINT_H_*/
