#ifndef JSTRING_H
#define JSTRING_H
#include <sys/types.h>

extern char *strnchr(const char *s, size_t count, int character);

/*
 * given a quoted string "\"abcde\"", quote='"', escape='\\'
 * s should point to        ^
 *
 * returns:
 *   end of string --> NULL
 *   -->                          ^
 */
char *end_of_quoted_string(const char *s, size_t count, char quote, char escape);

#endif /*JSTRING_H*/
