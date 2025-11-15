/*
 * Machine-dependent constants for x86_64 and i386
 */

#ifndef _MACHINE_PARAM_H_
#define _MACHINE_PARAM_H_

/*
 * Machine-dependent page size and related constants
 */
#define NBPG		4096		/* bytes/page */
#define PGOFSET		(NBPG-1)	/* byte offset into page */
#define PGSHIFT		12		/* LOG2(NBPG) */

#define	CLSIZE		1
#define	CLSIZELOG2	0

/* Clicking clears all of the page's bits */
#define	CLBYTES		(CLSIZE * NBPG)

/*
 * Size of kernel malloc arena in CLBYTES-sized logical pages
 */
#define	NKMEMCLUSTERS	(4096*1024/CLBYTES)

/*
 * Constants related to network buffer management.
 * MCLBYTES must be no larger than CLBYTES (the software page size), and,
 * on machines that exchange pages of input or output buffers with mbuf
 * clusters (MAPPED_MBUFS), MCLBYTES must also be an integral multiple
 * of the hardware page size.
 */
#define	MSIZE		128		/* size of an mbuf */

#ifndef MCLSHIFT
#define	MCLSHIFT	11		/* convert bytes to m_buf clusters */
#endif	/* MCLSHIFT */

#define	MCLBYTES	(1 << MCLSHIFT)	/* size of a m_buf cluster */
#define	MCLOFSET	(MCLBYTES - 1)	/* offset within a m_buf cluster */

#ifdef GATEWAY
#define	NMBCLUSTERS	2048		/* map size, max cluster allocation */
#else
#define	NMBCLUSTERS	1024		/* map size, max cluster allocation */
#endif

/*
 * Minimum and maximum sizes of the kernel malloc arena in PAGE_SIZE-sized
 * logical pages.
 */
#define	NKMEMPAGES_MIN_DEFAULT	((6 * 1024 * 1024) >> PAGE_SHIFT)
#define	NKMEMPAGES_MAX_DEFAULT	((128 * 1024 * 1024) >> PAGE_SHIFT)

/*
 * Some macros for units conversion
 */

/* pages ("clicks") to disk blocks */
#define	ctod(x)		((x) << (PGSHIFT - DEV_BSHIFT))
#define	dtoc(x)		((x) >> (PGSHIFT - DEV_BSHIFT))

/* pages to bytes */
#define	ctob(x)		((x) << PGSHIFT)
#define	btoc(x)		(((x) + PGOFSET) >> PGSHIFT)

/* bytes to pages */
#define	btop(x)		(((unsigned)(x)) >> PGSHIFT)
#define	ptob(x)		((x) << PGSHIFT)

/*
 * Map a ``block device block'' to a file system block.
 * This should be device dependent, and should use the bsize
 * field from the disk label.
 * For now though just use DEV_BSIZE.
 */
#define	bdbtofsb(bn)	((bn) / (BLKDEV_IOSIZE/DEV_BSIZE))

/*
 * Mach derived conversion macros
 */
#define x86_round_page(x)	((((unsigned)(x)) + PGOFSET) & ~PGOFSET)
#define x86_trunc_page(x)	((unsigned)(x) & ~PGOFSET)
#define x86_btop(x)		((unsigned)(x) >> PGSHIFT)
#define x86_ptob(x)		((unsigned)(x) << PGSHIFT)

/*
 * Round up to nearest page / block boundary
 */
#define round_page(x)	x86_round_page(x)
#define trunc_page(x)	x86_trunc_page(x)

/*
 * Constants related to disk I/O
 */
#ifndef DEV_BSIZE
#define DEV_BSIZE	512
#endif
#define DEV_BSHIFT	9		/* log2(DEV_BSIZE) */
#define BLKDEV_IOSIZE	2048
#define MAXPHYS		(64 * 1024)	/* max raw I/O transfer size */

#define	DELAY(n)	delay(n)

#endif /* !_MACHINE_PARAM_H_ */
