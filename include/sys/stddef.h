/*
 * Standard type definitions
 */

#ifndef _SYS_STDDEF_H_
#define _SYS_STDDEF_H_

#include <sys/cdefs.h>
#include <machine/ansi.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
typedef unsigned long size_t;
#endif

#ifndef _PTRDIFF_T_DEFINED_
#define _PTRDIFF_T_DEFINED_
typedef long ptrdiff_t;
#endif

#define offsetof(type, member) ((size_t)(&((type *)0)->member))

#endif /* _SYS_STDDEF_H_ */
