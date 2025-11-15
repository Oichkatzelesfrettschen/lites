/*	mount.h	6.1	83/07/29	*/

/*
 * Mount structure - one allocated for each mounted filesystem
 */

#ifndef _MOUNT_H_
#define _MOUNT_H_

#ifndef KERNEL
#include <sys/types.h>
#else
#ifndef LOCORE
#include "types.h"
#endif
#endif

struct mount {
	dev_t	m_dev;			/* device mounted */
	struct	buf *m_bufp;		/* pointer to superblock */
	struct	inode *m_inodp;		/* pointer to mounted on inode */
	struct	inode *m_mount;		/* pointer to root inode of filesystem */
};

#ifdef KERNEL
struct	mount mount[NMOUNT];
#endif

#endif /* _MOUNT_H_ */
