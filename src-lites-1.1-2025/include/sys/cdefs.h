#ifndef _CDEFS_H_
#define _CDEFS_H_

#if defined(__cplusplus)
#define __BEGIN_DECLS   extern "C" {
#define __END_DECLS     }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

#define __CONCAT(x,y)   x ## y
#define __STRING(x)     #x

/* Function attributes */
#if defined(__GNUC__)
#define __dead2         __attribute__((__noreturn__))
#define __pure2         __attribute__((__const__))
#else
#define __dead2
#define __pure2
#endif
#define __dead
#define __pure

#endif /* !_CDEFS_H_ */
