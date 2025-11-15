/*	systm.h	6.1	83/07/29	*/

/*
 * Random set of variables used by more than one routine in the system.
 */

#ifndef KERNEL
#include <sys/types.h>
#else
#ifndef LOCORE
#include "types.h"
#endif
#endif

#ifdef KERNEL
extern char version[];		/* system version */

/* 1.1 */
extern long time;		/* time in seconds since 1970 */

/* 1.2 */
extern int hz;			/* system clock's frequency */
extern int tick;		/* usec per tick (1000000 / hz) */
extern int lbolt;		/* time of day in hz since last boot */

extern int nproc;
extern int maxuprc;		/* max processes per user */

extern dev_t rootdev;		/* root device */
extern dev_t swapdev;		/* swapping device */
extern dev_t pipedev;		/* pipe device */

extern int updlock;		/* lock for sync */
extern daddr_t rablock;		/* block to be read ahead */

/* Bit map related macros. */
#define	setbit(a,i)	((a)[(i)/8] |= 1<<((i)%8))
#define	clrbit(a,i)	((a)[(i)/8] &= ~(1<<((i)%8)))
#define	isset(a,i)	((a)[(i)/8] & (1<<((i)%8)))
#define	isclr(a,i)	(((a)[(i)/8] & (1<<((i)%8))) == 0)

/* Function prototypes */
int	splnet(void);
int	splimp(void);
int	splbio(void);
int	spltty(void);
int	splclock(void);
int	spl0(void);
int	spl1(void);
int	spl4(void);
int	spl5(void);
int	spl6(void);
int	spl7(void);
void	splx(int);

void	panic(const char *);
void	tablefull(const char *);
void	uprintf(const char *, ...);
void	printf(const char *, ...);

#endif /* KERNEL */
