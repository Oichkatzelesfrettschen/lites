/*	resource.h	6.1	83/07/29	*/

/*
 * Process resource usage and limits
 */

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#ifndef KERNEL
#include <sys/types.h>
#include <sys/time.h>
#else
#ifndef LOCORE
#include "types.h"
#include "time.h"
#endif
#endif

/*
 * Resource usage structure
 */
struct rusage {
	struct timeval ru_utime;	/* user time used */
	struct timeval ru_stime;	/* system time used */
	long	ru_maxrss;		/* max resident set size */
	long	ru_ixrss;		/* integral shared text size */
	long	ru_idrss;		/* integral unshared data size */
	long	ru_isrss;		/* integral unshared stack size */
	long	ru_minflt;		/* page faults not requiring I/O */
	long	ru_majflt;		/* page faults requiring I/O */
	long	ru_nswap;		/* swaps */
	long	ru_inblock;		/* block input operations */
	long	ru_oublock;		/* block output operations */
	long	ru_msgsnd;		/* messages sent */
	long	ru_msgrcv;		/* messages received */
	long	ru_nsignals;		/* signals received */
	long	ru_nvcsw;		/* voluntary context switches */
	long	ru_nivcsw;		/* involuntary context switches */
};

/*
 * Resource limits
 */
struct rlimit {
	long	rlim_cur;		/* current (soft) limit */
	long	rlim_max;		/* hard limit */
};

/* Resource limit identifiers */
#define	RLIMIT_CPU	0		/* cpu time in milliseconds */
#define	RLIMIT_FSIZE	1		/* maximum file size */
#define	RLIMIT_DATA	2		/* data size */
#define	RLIMIT_STACK	3		/* stack size */
#define	RLIMIT_CORE	4		/* core file size */
#define	RLIMIT_RSS	5		/* resident set size */

#define	RLIM_NLIMITS	6		/* number of resource limits */

#define	RLIM_INFINITY	0x7fffffff

#endif /* _RESOURCE_H_ */
