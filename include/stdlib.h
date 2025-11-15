/*
 * Minimal stdlib.h wrapper for kernel builds with -nostdinc
 * Provides access to compiler built-in standard library functions
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Program termination */
void exit(int status) __attribute__((__noreturn__));
void abort(void) __attribute__((__noreturn__));

/* Memory allocation (kernel versions) */
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

/* String conversion */
int atoi(const char *nptr);
long atol(const char *nptr);
long long atoll(const char *nptr);
long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);
long long strtoll(const char *nptr, char **endptr, int base);
unsigned long long strtoull(const char *nptr, char **endptr, int base);

/* Searching and sorting */
void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *));
void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));

/* Absolute value */
int abs(int j);
long labs(long j);
long long llabs(long long j);

/* Random numbers */
int rand(void);
void srand(unsigned int seed);

/* Environment (not typically used in kernel) */
char *getenv(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* !_STDLIB_H_ */
