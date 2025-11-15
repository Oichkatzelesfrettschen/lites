/*	callout.h	6.1	83/07/29	*/

/*
 * Callout structure - used for kernel timeout mechanism
 */

#ifndef _CALLOUT_H_
#define _CALLOUT_H_

struct callo {
	int	c_time;			/* incremental time */
	caddr_t	c_arg;			/* argument to routine */
	int	(*c_func)();		/* routine */
};

#ifdef KERNEL
struct	callo callout[NCALL];
#define	NCALL	20			/* number of callout entries */
#endif

#endif /* _CALLOUT_H_ */
