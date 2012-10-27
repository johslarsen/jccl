#ifndef UNITTEST_H
#define UNITTEST_H
#include <stdio.h>

#define UNITTEST(expr)					((expr) ? (void)0 : unittest_fail(__STRING(expr), __FILE__, __LINE__, __func__))

extern void unittest_fail(const char *expr, const char *file, unsigned int line, const char *func);
extern int acquire_seed(char *seed_from_argument);
extern int acquire_and_print_seed(FILE *fp, char *seed_from_argument);

#endif /* UNITTEST_H */
