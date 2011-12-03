#ifndef UNITTEST_H
#define UNITTEST_H

#define UNITTEST(expr)					((expr) ? (void)0 : unittest_fail(__STRING(expr), __FILE__, __LINE__, __func__))

void unittest_fail(const char *expr, const char *file, unsigned int line, const char *func);

#endif /* UNITTEST_H */
