#ifndef JSTRING_H
#define JSTRING_H
#include <sys/types.h>

extern char *strnchr(const char *s, size_t count, int character);
extern char *strnstr(const char *s1, const char *s2, size_t len);

/*
 * given a quoted string "\"abcde\"", quote='"', escape='\\'
 * s should point to        ^
 *
 * returns:
 *   end of string --> NULL
 *   -->                          ^
 */
extern char *end_of_quoted_string(const char *s, size_t count, char quote, char escape);

typedef int (*character_modifier)(int c); // tolower(),...
char *strncpy_with_modifier(char *dest, const char *src, size_t count, character_modifier modifier);

#endif /*JSTRING_H*/
