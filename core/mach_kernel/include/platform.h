/* 
 * platform.h
 *
 * x-kernel v3.2
 *
 * Copyright (c) 1993,1991,1990  Arizona Board of Regents
 *
 *
 * $Revision: 1.31 $
 * $Date: 1993/09/28 18:10:17 $
 */

#ifndef platform_h
#define platform_h


#if defined(__GNUC__) && ! defined(XK_DEBUG)
#  define XK_USE_INLINE
#endif

#include <sys/types.h>

#ifdef XKMACHKERNEL

#define u_short unsigned short
#define u_long unsigned long
#define u_char unsigned char

#else

#include <limits.h>
#include <netinet/in.h>
#include <ctype.h>

#endif /* XKMACHKERNEL */

#include "process.h"
#include "msg_s.h"



extern	char *	xMalloc( unsigned );
extern  void 	xFree( char * );
extern  void	xMallocInit( void );



extern char *strcpy();

/* 
 * The mach kernel stack size is small ... need to avoid function
 * calls where we can.
 */
#ifdef XKMACHKERNEL
#  define XK_UPI_MACROS
#endif



void	evInit( int );
u_short inCkSum( Msg *m, u_short *buf, int len );
u_short ocsum( u_short *buf, int count );



typedef long *Unspec;
typedef unsigned long ProcessId;
typedef unsigned long ContextId;

#ifndef NULL
#define NULL	0
#endif
#define MAXUNSIGNED	((unsigned) (-1)

#ifndef XKMACHKERNEL
#define splx(x)
#define spl7() 1
#define splnet spl7
#define splclk spl7
#endif XKMACHKERNEL

#define INIT_STACK_SIZE 1024

#define BYTES_PER_WORD	4

#define CLICKS_PER_SEC 100	/* Clock interrupts per second *//***/

#if !defined(ROUND4)
#define ROUND4(len)  ((len + 3) & ~3)
#endif
#define BETWEEN(A,B,C) ((A) <= (B) && (B) < (C))

typedef	char	*mapKeyPtr_t;
typedef	int	mapVal_t;

typedef char	*statePtr_t;

#ifndef	TRUE
#define	TRUE	1
#define FALSE	0
#endif

#define	SUCCESS_RET_VAL		0
#define	FAILURE_RET_VAL		(-1)

/* Used for numbers? */

#define	LO_BYTE_OF_2(word)	 ((unsigned char) (0xff & (unsigned) word))
#define HI_BYTE_OF_2(word)	 ((unsigned char) (((unsigned) word) >> 8 ))

#define CAT_BYTES(hiByte,loByte) ((((unsigned)hiByte)<<8) + (unsigned)loByte)

#ifndef ENDIAN
/*
 * Definitions for byte order,
 * according to byte significance from low address to high.
 */
#define	LITTLE	1234		/* least-significant byte first (vax) */
#define	BIG	4321		/* most-significant byte first */
#define	PDP	3412		/* LSB first in word, MSW first in long (pdp) */
#if defined(vax) || defined(pmax) || defined(mips) || defined(i386)
#define	ENDIAN	LITTLE
#else
#define	ENDIAN	BIG		/* byte order on mc68000, tahoe, most others */
#endif
#endif ENDIAN

#if (ENDIAN == BIG)
# ifndef ntohs
#  define ntohs(s) (s)
#  define htons(s) (s)
#  define ntohl(s) (s)
#  define htonl(s) (s)
# endif
#endif /* ENDIAN == BIG */

/* 
 * LONG_ALIGNMENT_REQUIRED indicates that int's must be 32-bit aligned
 * on this architecture.
 */
#define LONG_ALIGNMENT_REQUIRED
#define LONG_ALIGNED( address )  (! ((int)(address) & 0x3))
#define SHORT_ALIGNED( address ) ( ! ((int)(address) & 0x1))

#define ROM_MAX_LINES 	100 	/* Up to 100 table entries */
#define ROM_MAX_FIELDS 	20

extern	char *rom[ROM_MAX_LINES + 1][ROM_MAX_FIELDS + 1];  

#if defined(__hp700__) && defined(__GNUC__)

/*
 * read_itimer provides access to a high-resolution timer in the PA-RISC
 * processor, with minimal overhead.  The timer "is continually counting
 * up by 1 at a rate which is implementation-dependent and between twice
 * the "peak instruction rate" and half the "peak instruction rate",
 * according to the Architecture Reference Manual.
 * For the 720's it's incremented once every 1/50th usec and for the
 * 730's and 750's it's incremented once every 1/66th usec.
 */
#define read_itimer()	({u_int r; asm volatile("mfctl 16,%0" : "=r" (r)); r;})

#endif /* __hp700__ && __GNUC__ */

#endif
