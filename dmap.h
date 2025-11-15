/*	dmap.h	6.1	83/07/29	*/

/*
 * Definitions for the mapping of virtual to physical memory
 */

#ifndef _DMAP_H_
#define _DMAP_H_

#ifndef KERNEL
#include <sys/types.h>
#else
#ifndef LOCORE
#include "types.h"
#endif
#endif

/*
 * Disk map structure - maps process virtual memory to swap space
 */
struct dmap {
	swblk_t	dm_size;	/* current size used */
	swblk_t	dm_alloc;	/* amount allocated */
	swblk_t	dm_map[1];	/* begins here */
};

#define	NDMAP 		38	/* size of core map */
#define	NSWAPMAP	200	/* size of swap map */

#endif /* _DMAP_H_ */
