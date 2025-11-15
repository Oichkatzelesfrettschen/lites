/*
 * Copyright (c) 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)malloc.h	8.5 (Berkeley) 5/3/95
 */

#ifndef _SYS_MALLOC_H_
#define _SYS_MALLOC_H_

#include <sys/types.h>

/*
 * Flags to malloc/free
 */
#define M_WAITOK    0x0000  /* can wait for resources */
#define M_NOWAIT    0x0001  /* do not wait for resources */
#define M_ZERO      0x0100  /* zero allocated memory */

/*
 * Types of memory to be allocated
 */
#define M_FREE      0   /* should be on free list */
#define M_MBUF      1   /* mbuf */
#define M_DEVBUF    2   /* device driver memory */
#define M_SOCKET    3   /* socket structure */
#define M_PCB       4   /* protocol control block */
#define M_RTABLE    5   /* routing tables */
#define M_HTABLE    6   /* IMP host tables */
#define M_FTABLE    7   /* fragment reassembly header */
#define M_ZOMBIE    8   /* zombie proc status */
#define M_IFADDR    9   /* interface address */
#define M_SOOPTS    10  /* socket options */
#define M_SONAME    11  /* socket name */
#define M_NAMEI     12  /* namei path name buffer */
#define M_GPROF     13  /* kernel profiling buffer */
#define M_IOCTLOPS  14  /* ioctl data buffer */
#define M_MAPMEM    15  /* mapped memory descriptors */
#define M_CRED      16  /* credentials */
#define M_PGRP      17  /* process group header */
#define M_SESSION   18  /* session header */
#define M_IOV       19  /* large iov's */
#define M_MOUNT     20  /* vfs mount struct */
#define M_FHANDLE   21  /* network file handle */
#define M_NFSREQ    22  /* NFS request header */
#define M_NFSMNT    23  /* NFS mount structure */
#define M_NFSNODE   24  /* NFS vnode private part */
#define M_VNODE     25  /* Dynamically allocated vnodes */
#define M_CACHE     26  /* Dynamically allocated cache entries */
#define M_DQUOT     27  /* UFS quota entries */
#define M_UFSMNT    28  /* UFS mount structure */
#define M_SHM       29  /* SVID compatible shared memory segments */
#define M_VMMAP     30  /* VM map structures */
#define M_VMMAPENT  31  /* VM map entry structures */
#define M_VMOBJ     32  /* VM object structure */
#define M_VMOBJHASH 33  /* VM object hash structure */
#define M_VMPMAP    34  /* VM pmap */
#define M_VMPVENT   35  /* VM phys-virt mapping entry */
#define M_VMPAGER   36  /* XXX: VM pager struct */
#define M_VMPGDATA  37  /* XXX: VM pager private data */
#define M_FILE      38  /* Open file structure */
#define M_FILEDESC  39  /* Open file descriptor table */
#define M_LOCKF     40  /* Byte-range locking structures */
#define M_PROC      41  /* Proc structures */
#define M_SUBPROC   42  /* Proc sub-structures */
#define M_SEGMENT   43  /* Segment for LFS */
#define M_LFSNODE   44  /* LFS vnode private part */
#define M_FFSNODE   45  /* FFS vnode private part */
#define M_MFSNODE   46  /* MFS vnode private part */
#define M_NQLEASE   47  /* Nqnfs lease */
#define M_NQMHOST   48  /* Nqnfs host address table */
#define M_NETADDR   49  /* Export host address structure */
#define M_NFSSVC    50  /* Nfs server structure */
#define M_NFSUID    51  /* Nfs uid mapping structure */
#define M_NFSD      52  /* Nfs server daemon structure */
#define M_IPMOPTS   53  /* internet multicast options */
#define M_IPMADDR   54  /* internet multicast address */
#define M_IFMADDR   55  /* link-level multicast address */
#define M_MRTABLE   56  /* multicast routing tables */
#define M_ISOFSMNT  57  /* ISOFS mount structure */
#define M_ISOFSNODE 58  /* ISOFS vnode private part */
#define M_NFSRVDESC 59  /* NFS server socket descriptor */
#define M_NFSDIROFF 60  /* NFS directory offset data */
#define M_NFSBIGFH  61  /* NFS version 3 file handle */
#define M_MSDOSFSMNT 62 /* MSDOSFS mount structure */
#define M_MSDOSFSFAT 63 /* MSDOSFS file allocation table */
#define M_MSDOSFSNODE 64 /* MSDOSFS vnode private part */
#define M_TTYS      65  /* tty data structures */
#define M_EXEC      66  /* argument lists & other mem used by exec */
#define M_MISCFSMNT 67  /* miscfs mount structures */
#define M_MISCFSNODE 68 /* miscfs vnode private part */
#define M_ADOSFSMNT 69  /* adosfs mount structures */
#define M_ADOSFSNODE 70 /* adosfs vnode private part */
#define M_ANODE     71  /* adosfs anode structures and tables. */

#define M_TEMP      74  /* misc temporary data buffers */
#define M_KTRACE    75  /* ktrace data */

#define M_LAST      76  /* Must be last type + 1 */

/*
 * Memory allocation statistics
 */
struct kmemstats {
    long    ks_inuse;       /* # of packets of this type currently in use */
    long    ks_calls;       /* total packets of this type ever allocated */
    long    ks_memuse;      /* total memory held in bytes */
    u_short ks_limblocks;   /* number of times blocked for hitting limit */
    u_short ks_mapblocks;   /* number of times blocked for kernel map */
    long    ks_maxused;     /* maximum number ever used */
    long    ks_limit;       /* most that are allowed to exist */
    long    ks_size;        /* sizes of this thing that are allocated */
    long    ks_spare;
};

/*
 * Kernel malloc header
 */
struct kmembuckets {
    caddr_t kb_next;    /* list of free blocks */
    caddr_t kb_last;    /* last free block */
    long    kb_calls;   /* total calls to allocate this size */
    long    kb_total;   /* total number of blocks allocated */
    long    kb_totalfree;   /* # of free items in this bucket */
    long    kb_elmpercl;    /* # of elements in this sized allocation */
    long    kb_highwat; /* high water mark */
    long    kb_couldfree;   /* over high water mark and could free */
};

#ifdef KERNEL

/*
 * Macro versions for the most common cases
 */
#define MALLOC(space, cast, size, type, flags) \
    (space) = (cast)malloc((unsigned long)(size), type, flags)

#define FREE(addr, type) \
    free((void *)(addr), type)

/*
 * Function prototypes
 */
void *malloc(unsigned long size, int type, int flags);
void free(void *addr, int type);
void *realloc(void *addr, unsigned long size, int type, int flags);

/*
 * Kernel statistics
 */
extern struct kmemstats kmemstats[];
extern struct kmembuckets bucket[];
extern char *kmembase;
extern char *kmemlimit;

#endif /* KERNEL */

#endif /* !_SYS_MALLOC_H_ */
