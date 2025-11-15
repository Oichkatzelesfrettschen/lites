/*	seg.h	6.1	83/07/29	*/

/*
 * Segment structure - ancient Unix memory segmentation
 */

#ifndef KERNEL
#include <sys/types.h>
#else
#ifndef LOCORE
#include "types.h"
#endif
#endif

/* Segment structure - minimal stub */
struct seg {
	unsigned char	*s_base;	/* virtual base address */
	size_t		s_size;		/* size */
	unsigned char	s_flags;	/* flags */
	unsigned char	s_type;		/* type */
};

/* Segment types */
#define	S_TEXT	0	/* text segment */
#define	S_DATA	1	/* data segment */
#define	S_STACK	2	/* stack segment */

/* Segment flags */
#define	S_READ	0x01	/* readable */
#define	S_WRITE	0x02	/* writable */
#define	S_EXEC	0x04	/* executable */
