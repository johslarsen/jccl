#ifndef BIGINT_H_
#define BIGINT_H_

struct bigint;

extern const char *bigint_hex_charset;

void bigint_destroy(struct bigint *n);

struct bigint *bigint_from_long(unsigned long n);
struct bigint *bigint_from_msb_first_hexstring(const char *s, int nchar);
char *bigint_to_msb_first_hexstring(struct bigint *n);

struct bigint *bigint_not(struct bigint *n); // ~n
struct bigint *bigint_and(struct bigint *a, struct bigint *b); // a&b
struct bigint *bigint_or(struct bigint *a, struct bigint *b); // a|b
struct bigint *bigint_xor(struct bigint *a, struct bigint *b); // a^b

struct bigint *bigint_shift_left(struct bigint *a, struct bigint *b); // a<<b
struct bigint *bigint_shift_right(struct bigint *a, struct bigint *b); // a>>b

struct bigint *bigint_negate(struct bigint *n); // -n
struct bigint *bigint_add(struct bigint *a, struct bigint *b); // a+b
struct bigint *bigint_subtract(struct bigint *a, struct bigint *b); // a+b

#endif /*BIGINT_H_*/
