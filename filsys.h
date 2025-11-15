/*	filsys.h	6.1	83/07/29	*/

/*
 * Structure of the super-block for ancient Unix filesystem
 */

#ifndef KERNEL
#include <sys/types.h>
#else
#ifndef LOCORE
#include "types.h"
#endif
#endif

struct	filsys {
	u_short	s_isize;	/* size in blocks of i-list */
	daddr_t	s_fsize;	/* size in blocks of entire volume */
	short	s_nfree;	/* number of addresses in s_free */
	daddr_t	s_free[50];	/* free block list */
	short	s_ninode;	/* number of i-nodes in s_inode */
	u_short	s_inode[100];	/* free i-node list */
	char	s_flock;	/* lock during free list manipulation */
	char	s_ilock;	/* lock during i-list manipulation */
	char	s_fmod;		/* super block modified flag */
	char	s_ronly;	/* mounted read-only flag */
	time_t	s_time;		/* last super block update */
	/* remainder not maintained by this version of the system */
	daddr_t	s_tfree;	/* total free blocks */
	u_short	s_tinode;	/* total free inodes */
	short	s_m;		/* interleave factor */
	short	s_n;		/* interleave factor */
	char	s_fname[6];	/* file system name */
	char	s_fpack[6];	/* file system pack name */
};

#ifdef KERNEL
struct	filsys *getfs();
#endif
