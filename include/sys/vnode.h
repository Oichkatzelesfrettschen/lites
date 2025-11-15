/*
 * Copyright (c) 1989, 1993
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
 *	@(#)vnode.h	8.17 (Berkeley) 5/20/95
 */

#ifndef _SYS_VNODE_H_
#define _SYS_VNODE_H_

#include <sys/types.h>
#include <sys/time.h>

/*
 * The vnode is the focus of all file activity in UNIX.  There is a
 * unique vnode allocated for each active file, each current directory,
 * each mounted-on file, text file, and the root.
 */

/*
 * Vnode types.  VNON means no type.
 */
enum vtype { VNON, VREG, VDIR, VBLK, VCHR, VLNK, VSOCK, VFIFO, VBAD };

/*
 * Vnode tag types.
 * These are for the benefit of external programs only (e.g., pstat)
 * and should NEVER be inspected by the kernel.
 */
enum vtagtype {
    VT_NON, VT_UFS, VT_NFS, VT_MFS, VT_MSDOSFS, VT_LFS, VT_LOFS, VT_FDESC,
    VT_PORTAL, VT_NULL, VT_UMAP, VT_KERNFS, VT_PROCFS, VT_AFS, VT_ISOFS,
    VT_UNION, VT_ADOSFS
};

/*
 * Each underlying filesystem allocates its own private area and hangs
 * it from v_data.  If non-null, this area is freed in getnewvnode().
 */

/* Forward declaration */
struct buf;

/* Simple list head structure for buffer lists */
struct buflists {
    struct buf *lh_first;   /* first element */
};

struct vnode {
    u_long      v_flag;             /* vnode flags (see below) */
    long        v_usecount;         /* reference count of users */
    long        v_writecount;       /* reference count of writers */
    long        v_holdcnt;          /* page & buffer references */
    daddr_t     v_lastr;            /* last read (read-ahead) */
    u_long      v_id;               /* capability identifier */
    struct mount *v_mount;          /* ptr to vfs we are in */
    struct vnodeops *v_op;          /* vnode operations vector */
    struct vnode *v_freelist;       /* vnode freelist */
    struct vnode *v_mntvnodes;      /* vnodes for mount point */
    struct buflists v_cleanblkhd;   /* clean blocklist head */
    struct buflists v_dirtyblkhd;   /* dirty blocklist head */
    long        v_numoutput;        /* num of writes in progress */
    enum vtype  v_type;             /* vnode type */
    union {
        struct mount    *vu_mountedhere;/* ptr to mounted vfs (VDIR) */
        struct socket   *vu_socket;     /* unix ipc (VSOCK) */
        struct specinfo *vu_specinfo;   /* device (VCHR, VBLK) */
        struct fifoinfo *vu_fifoinfo;   /* fifo (VFIFO) */
    } v_un;
    enum vtagtype v_tag;            /* type of underlying data */
    void        *v_data;            /* private data for fs */
};

#define v_mountedhere   v_un.vu_mountedhere
#define v_socket        v_un.vu_socket
#define v_specinfo      v_un.vu_specinfo
#define v_fifoinfo      v_un.vu_fifoinfo

/*
 * Vnode flags.
 */
#define VROOT       0x0001  /* root of its file system */
#define VTEXT       0x0002  /* vnode is a pure text prototype */
#define VSYSTEM     0x0004  /* vnode being used by kernel */
#define VISTTY      0x0008  /* vnode represents a tty */
#define VXLOCK      0x0100  /* vnode is locked to change underlying type */
#define VXWANT      0x0200  /* process is waiting for vnode */
#define VBWAIT      0x0400  /* waiting for output to complete */
#define VALIASED    0x0800  /* vnode has an alias */
#define VDIROP      0x1000  /* LFS: vnode is involved in a directory op */

/*
 * Vnode attributes.  A field value of VNOVAL represents a field whose value
 * is unavailable (getattr) or which is not to be changed (setattr).
 */
struct vattr {
    enum vtype  va_type;        /* vnode type (for create) */
    u_short     va_mode;        /* files access mode and type */
    short       va_nlink;       /* number of references to file */
    uid_t       va_uid;         /* owner user id */
    gid_t       va_gid;         /* owner group id */
    long        va_fsid;        /* file system id (dev for now) */
    long        va_fileid;      /* file id */
    u_quad_t    va_size;        /* file size in bytes */
    long        va_blocksize;   /* blocksize preferred for i/o */
    struct timespec va_atime;   /* time of last access */
    struct timespec va_mtime;   /* time of last modification */
    struct timespec va_ctime;   /* time file changed */
    u_long      va_gen;         /* generation number of file */
    u_long      va_flags;       /* flags defined for file */
    dev_t       va_rdev;        /* device the special file represents */
    u_quad_t    va_bytes;       /* bytes of disk space held by file */
    u_quad_t    va_filerev;     /* file modification number */
    u_int       va_vaflags;     /* operations flags, see below */
    long        va_spare;       /* remain quad aligned */
};

/*
 * Flags for va_vaflags.
 */
#define VA_UTIMES_NULL  0x01    /* utimes argument was NULL */
#define VA_EXCLUSIVE    0x02    /* exclusive create request */

/*
 * Flags for ioflag.
 */
#define IO_UNIT     0x0001      /* do I/O as atomic unit */
#define IO_APPEND   0x0002      /* append write to end */
#define IO_SYNC     0x0004      /* do I/O synchronously */
#define IO_NODELOCKED 0x0008    /* underlying node already locked */
#define IO_NDELAY   0x0010      /* FNDELAY flag set in file table */

/*
 * Token indicating no attribute value yet assigned.
 */
#define VNOVAL  (-1)

/*
 * Convert between vnode types and inode formats
 */
extern enum vtype iftovt_tab[];
extern int vttoif_tab[];
#define IFTOVT(mode)    (iftovt_tab[((mode) & S_IFMT) >> 12])
#define VTTOIF(indx)    (vttoif_tab[(int)(indx)])
#define MAKEIMODE(indx, mode)   (int)(VTTOIF(indx) | (mode))

/*
 * Flags to various vnode operations.
 */
#define SKIPSYSTEM  0x0001      /* vflush: skip vnodes marked VSYSTEM */
#define FORCECLOSE  0x0002      /* vflush: force file closeure */
#define WRITECLOSE  0x0004      /* vflush: only close writeable files */
#define DOCLOSE     0x0008      /* vclean: close active files */
#define V_SAVE      0x0001      /* vinvalbuf: sync file first */
#define V_SAVEMETA  0x0002      /* vinvalbuf: save indirect blocks */

#define REVOKEALL   0x0001      /* revoke: all aliases of vnode */

/*
 * Vnode operation vector structure.
 */
struct vnodeops {
    int (*vop_lookup)(void *);
    int (*vop_create)(void *);
    int (*vop_mknod)(void *);
    int (*vop_open)(void *);
    int (*vop_close)(void *);
    int (*vop_access)(void *);
    int (*vop_getattr)(void *);
    int (*vop_setattr)(void *);
    int (*vop_read)(void *);
    int (*vop_write)(void *);
    int (*vop_ioctl)(void *);
    int (*vop_select)(void *);
    int (*vop_mmap)(void *);
    int (*vop_fsync)(void *);
    int (*vop_seek)(void *);
    int (*vop_remove)(void *);
    int (*vop_link)(void *);
    int (*vop_rename)(void *);
    int (*vop_mkdir)(void *);
    int (*vop_rmdir)(void *);
    int (*vop_symlink)(void *);
    int (*vop_readdir)(void *);
    int (*vop_readlink)(void *);
    int (*vop_abortop)(void *);
    int (*vop_inactive)(void *);
    int (*vop_reclaim)(void *);
    int (*vop_lock)(void *);
    int (*vop_unlock)(void *);
    int (*vop_bmap)(void *);
    int (*vop_strategy)(void *);
    int (*vop_print)(void *);
    int (*vop_islocked)(void *);
    int (*vop_pathconf)(void *);
    int (*vop_advlock)(void *);
    int (*vop_blkatoff)(void *);
    int (*vop_valloc)(void *);
    int (*vop_vfree)(void *);
    int (*vop_truncate)(void *);
    int (*vop_update)(void *);
    int (*vop_bwrite)(void *);
};

#ifdef KERNEL

/*
 * Generic file operations
 */
struct file;
struct proc;
struct uio;
struct ucred;
struct componentname;
struct nameidata;

/* Vnode manipulation functions */
int     vget(struct vnode *vp, int lockflag);
void    vref(struct vnode *vp);
void    vrele(struct vnode *vp);
void    vput(struct vnode *vp);
void    vhold(struct vnode *vp);
void    holdrele(struct vnode *vp);

int     getnewvnode(enum vtagtype tag, struct mount *mp,
            struct vnodeops *vops, struct vnode **vpp);
void    insmntque(struct vnode *vp, struct mount *mp);

void    vclean(struct vnode *vp, int flags);
void    vgone(struct vnode *vp);
int     vflush(struct mount *mp, struct vnode *skipvp, int flags);
void    vflushbuf(struct vnode *vp, int sync);
void    vwakeup(struct vnode *vp);

int     vinvalbuf(struct vnode *vp, int flags, struct ucred *cred,
            struct proc *p, int slpflag, int slptimeo);

void    vprint(char *label, struct vnode *vp);

/* Vnode operation helper macros */
#define VOP_LOOKUP(dvp, vpp, cnp)       (*((dvp)->v_op->vop_lookup))(dvp, vpp, cnp)
#define VOP_CREATE(dvp, vpp, cnp, vap)  (*((dvp)->v_op->vop_create))(dvp, vpp, cnp, vap)
#define VOP_OPEN(vp, mode, cred, p)     (*((vp)->v_op->vop_open))(vp, mode, cred, p)
#define VOP_CLOSE(vp, fflag, cred, p)   (*((vp)->v_op->vop_close))(vp, fflag, cred, p)
#define VOP_ACCESS(vp, mode, cred, p)   (*((vp)->v_op->vop_access))(vp, mode, cred, p)
#define VOP_GETATTR(vp, vap, cred, p)   (*((vp)->v_op->vop_getattr))(vp, vap, cred, p)
#define VOP_SETATTR(vp, vap, cred, p)   (*((vp)->v_op->vop_setattr))(vp, vap, cred, p)
#define VOP_READ(vp, uio, ioflag, cred) (*((vp)->v_op->vop_read))(vp, uio, ioflag, cred)
#define VOP_WRITE(vp, uio, ioflag, cred) (*((vp)->v_op->vop_write))(vp, uio, ioflag, cred)
#define VOP_IOCTL(vp, cmd, data, fflag, cred, p) \
    (*((vp)->v_op->vop_ioctl))(vp, cmd, data, fflag, cred, p)
#define VOP_SELECT(vp, which, fflags, cred, p) \
    (*((vp)->v_op->vop_select))(vp, which, fflags, cred, p)
#define VOP_FSYNC(vp, cred, waitfor, p) (*((vp)->v_op->vop_fsync))(vp, cred, waitfor, p)
#define VOP_REMOVE(dvp, vp, cnp)        (*((dvp)->v_op->vop_remove))(dvp, vp, cnp)
#define VOP_LINK(tdvp, vp, cnp)         (*((tdvp)->v_op->vop_link))(tdvp, vp, cnp)
#define VOP_RENAME(fdvp, fvp, fcnp, tdvp, tvp, tcnp) \
    (*((fdvp)->v_op->vop_rename))(fdvp, fvp, fcnp, tdvp, tvp, tcnp)
#define VOP_MKDIR(dvp, vpp, cnp, vap)   (*((dvp)->v_op->vop_mkdir))(dvp, vpp, cnp, vap)
#define VOP_RMDIR(dvp, vp, cnp)         (*((dvp)->v_op->vop_rmdir))(dvp, vp, cnp)
#define VOP_SYMLINK(dvp, vpp, cnp, vap, target) \
    (*((dvp)->v_op->vop_symlink))(dvp, vpp, cnp, vap, target)
#define VOP_READDIR(vp, uio, cred, eofflag) \
    (*((vp)->v_op->vop_readdir))(vp, uio, cred, eofflag)
#define VOP_READLINK(vp, uio, cred)     (*((vp)->v_op->vop_readlink))(vp, uio, cred)
#define VOP_INACTIVE(vp, p)             (*((vp)->v_op->vop_inactive))(vp, p)
#define VOP_RECLAIM(vp)                 (*((vp)->v_op->vop_reclaim))(vp)
#define VOP_LOCK(vp)                    (*((vp)->v_op->vop_lock))(vp)
#define VOP_UNLOCK(vp)                  (*((vp)->v_op->vop_unlock))(vp)
#define VOP_BMAP(vp, bn, vpp, bnp)      (*((vp)->v_op->vop_bmap))(vp, bn, vpp, bnp)
#define VOP_STRATEGY(bp)                (*((bp)->b_vp->v_op->vop_strategy))(bp)
#define VOP_PRINT(vp)                   (*((vp)->v_op->vop_print))(vp)
#define VOP_ISLOCKED(vp)                (*((vp)->v_op->vop_islocked))(vp)
#define VOP_PATHCONF(vp, name, retval)  (*((vp)->v_op->vop_pathconf))(vp, name, retval)
#define VOP_ADVLOCK(vp, id, op, fl, flags) \
    (*((vp)->v_op->vop_advlock))(vp, id, op, fl, flags)
#define VOP_TRUNCATE(vp, len, flags, cred, p) \
    (*((vp)->v_op->vop_truncate))(vp, len, flags, cred, p)
#define VOP_UPDATE(vp, ta, tm, waitfor) (*((vp)->v_op->vop_update))(vp, ta, tm, waitfor)
#define VOP_BWRITE(bp)                  (*((bp)->b_vp->v_op->vop_bwrite))(bp)

/* Global vnode data */
extern struct vnode *rootvnode;     /* root vnode */

#endif /* KERNEL */

#endif /* !_SYS_VNODE_H_ */
