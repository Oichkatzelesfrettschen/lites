/*
 * Machine-dependent parameters for i386
 * Stub for Lites 1.1.u3 build
 */

#ifndef _MACHINE_PARAM_H_
#define _MACHINE_PARAM_H_

/* Page size parameters */
#define NBPG		4096		/* bytes/page */
#define PGOFSET		(NBPG-1)	/* byte offset into page */
#define PGSHIFT		12		/* LOG2(NBPG) */

/* Clustering */
#define CLSIZE		1
#define CLSIZELOG2	0

/* Other machine parameters */
#define MAXBSIZE	8192		/* maximum filesystem block size */
#define DEV_BSIZE	512		/* device block size */

#endif /* _MACHINE_PARAM_H_ */
