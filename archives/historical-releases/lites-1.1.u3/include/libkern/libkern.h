/*
 * Kernel Library Header
 * Generated stub for Lites 1.1.u3 build
 *
 * This header provides common kernel utility functions.
 * In BSD systems, libkern provides string and memory operations
 * that are safe for use in kernel context.
 */

#ifndef _LIBKERN_LIBKERN_H_
#define _LIBKERN_LIBKERN_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

/*
 * Common kernel string functions
 * Most of these are provided by standard C library,
 * but declared here for kernel context.
 */

/* String comparison and manipulation */
int	 strcmp __P((const char *, const char *));
int	 strncmp __P((const char *, const char *, size_t));
size_t	 strlen __P((const char *));
char	*strcpy __P((char *, const char *));
char	*strncpy __P((char *, const char *, size_t));
char	*strcat __P((char *, const char *));

/* Memory operations */
void	*memcpy __P((void *, const void *, size_t));
void	*memmove __P((void *, const void *, size_t));
int	 memcmp __P((const void *, const void *, size_t));
void	*memset __P((void *, int, size_t));

/* BSD-specific kernel functions */
int	 bcmp __P((const void *, const void *, size_t));
void	 bcopy __P((const void *, void *, size_t));
void	 bzero __P((void *, size_t));

/* Random number generation for kernel */
u_long	 random __P((void));
void	 srandom __P((u_long));

__END_DECLS

#endif /* _LIBKERN_LIBKERN_H_ */
