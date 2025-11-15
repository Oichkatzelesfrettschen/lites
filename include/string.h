/*
 * Minimal string.h wrapper for kernel builds with -nostdinc
 * Provides access to compiler built-in string functions
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* String comparison */
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

/* String copy */
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);

/* String concatenation */
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);

/* String length */
size_t strlen(const char *s);

/* String search */
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);

/* Memory operations */
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

/* BSD string functions (already in kernel) */
void bcopy(const void *src, void *dst, size_t len);
void bzero(void *dst, size_t len);
int bcmp(const void *s1, const void *s2, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* !_STRING_H_ */
