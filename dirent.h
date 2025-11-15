/*	dirent.h	6.1	83/07/29	*/

/*
 * Directory entry structure
 */

#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifndef KERNEL
#include <sys/types.h>
#else
#ifndef LOCORE
#include "types.h"
#endif
#endif

/* Directory entry structure */
struct direct {
	u_long	d_ino;			/* inode number of entry */
	u_short	d_reclen;		/* length of this record */
	u_short	d_namlen;		/* length of string in d_name */
	char	d_name[MAXNAMLEN + 1];	/* name (up to MAXNAMLEN + 1) */
};

/* Old BSD 4.2 format */
struct odirect {
	u_short	d_ino;
	char	d_name[14];
};

#endif /* _DIRENT_H_ */
