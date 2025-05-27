#include <string.h>
/*
 * Copyright (c) 1993 Jan-Simon Pendry
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry.
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
 *	@(#)procfs.h	8.6 (Berkeley) 2/3/94
 *
 * From:
 *	$Id: procfs.h,v 1.1.1.1 1995/03/02 21:49:50 mike Exp $
 */

/*
 * The different types of node in a procfs filesystem
 */
typedef enum {
	Proot,		/* the filesystem root */
	Pproc,		/* a process-specific sub-directory */
	Pfile,		/* the executable file */
	Pmem,		/* the process's memory image */
	Pregs,		/* the process's register set */
	Pfpregs,	/* the process's FP register set */
	Pctl,		/* process control */
	Pstatus,	/* process status */
	Pnote,		/* process notifier */
	Pnotepg		/* process group notifier */
} pfstype;

/*
 * control data for the proc file system.
 */
struct pfsnode {
	struct pfsnode	*pfs_next;	/* next on list */
	struct vnode	*pfs_vnode;	/* vnode associated with this pfsnode */
	pfstype		pfs_type;	/* type of procfs node */
	pid_t		pfs_pid;	/* associated process */
	u_short		pfs_mode;	/* mode bits for stat() */
	u_long		pfs_flags;	/* open flags */
	u_long		pfs_fileno;	/* unique file id */
};

#define PROCFS_NOTELEN	64	/* max length of a note (/proc/$pid/note) */
#define PROCFS_CTLLEN 	8	/* max length of a ctl msg (/proc/$pid/ctl */

/*
 * Kernel stuff follows
 */
#ifdef KERNEL
#define CNEQ(cnp, s, len) \
	 ((cnp)->cn_namelen == (len) && \
	  (memcmp((s), (cnp)->cn_nameptr, (len)) == 0))

/*
 * Format of a directory entry in /proc, ...
 * This must map onto struct dirent (see <dirent.h>)
 */
#define PROCFS_NAMELEN 8
struct pfsdent {
	u_long	d_fileno;
	u_short	d_reclen;
	u_char	d_type;
	u_char	d_namlen;
	char	d_name[PROCFS_NAMELEN];
};
#define UIO_MX sizeof(struct pfsdent)
#define PROCFS_FILENO(pid, type) \
	(((type) == Proot) ? \
			2 : \
			((((pid)+1) << 3) + ((int) (type))))

/*
 * Convert between pfsnode vnode
 */
#define VTOPFS(vp)	((struct pfsnode *)(vp)->v_data)
#define PFSTOV(pfs)	((pfs)->pfs_vnode)

typedef struct vfs_namemap vfs_namemap_t;
struct vfs_namemap {
	const char *nm_name;
	int nm_val;
};

extern int vfs_getuserstr (struct uio *, char *, int *);
extern vfs_namemap_t *vfs_findname (vfs_namemap_t *, char *, int);

/* <machine/reg.h> */
struct reg;
struct fpreg;

#define PFIND(pid) ((pid) ? pfind(pid) : &proc0)
extern int procfs_freevp (struct vnode *);
extern int procfs_allocvp (struct mount *, struct vnode **, long, pfstype);
extern struct vnode *procfs_findtextvp (struct proc *);
extern int procfs_sstep (struct proc *);
extern void procfs_fix_sstep (struct proc *);
extern int procfs_read_regs (struct proc *, struct reg *);
extern int procfs_write_regs (struct proc *, struct reg *);
extern int procfs_read_fpregs (struct proc *, struct fpreg *);
extern int procfs_write_fpregs (struct proc *, struct fpreg *);
extern int procfs_donote (struct proc *, struct proc *, struct pfsnode *pfsp, struct uio *uio);
extern int procfs_doregs (struct proc *, struct proc *, struct pfsnode *pfsp, struct uio *uio);
extern int procfs_dofpregs (struct proc *, struct proc *, struct pfsnode *pfsp, struct uio *uio);
extern int procfs_domem (struct proc *, struct proc *, struct pfsnode *pfsp, struct uio *uio);
extern int procfs_doctl (struct proc *, struct proc *, struct pfsnode *pfsp, struct uio *uio);
extern int procfs_dostatus (struct proc *, struct proc *, struct pfsnode *pfsp, struct uio *uio);

#define PROCFS_LOCKED	0x01
#define PROCFS_WANT	0x02

extern int (**procfs_vnodeop_p)();
extern struct vfsops procfs_vfsops;

/*
 * Prototypes for procfs vnode ops
 */
int	procfs_badop();	/* varargs */
int	procfs_rw (struct vop_read_args *);
int	procfs_lookup (struct vop_lookup_args *);
#define procfs_create ((int (*) (struct vop_create_args *)) procfs_badop)
#define procfs_mknod ((int (*) (struct vop_mknod_args *)) procfs_badop)
int	procfs_open (struct vop_open_args *);
int	procfs_close (struct vop_close_args *);
int	procfs_access (struct vop_access_args *);
int	procfs_getattr (struct vop_getattr_args *);
int	procfs_setattr (struct vop_setattr_args *);
#define	procfs_read procfs_rw
#define	procfs_write procfs_rw
int	procfs_ioctl (struct vop_ioctl_args *);
#define procfs_select ((int (*) (struct vop_select_args *)) procfs_badop)
#define procfs_mmap ((int (*) (struct vop_mmap_args *)) procfs_badop)
#define procfs_fsync ((int (*) (struct vop_fsync_args *)) procfs_badop)
#define procfs_seek ((int (*) (struct vop_seek_args *)) procfs_badop)
#define procfs_remove ((int (*) (struct vop_remove_args *)) procfs_badop)
#define procfs_link ((int (*) (struct vop_link_args *)) procfs_badop)
#define procfs_rename ((int (*) (struct vop_rename_args *)) procfs_badop)
#define procfs_mkdir ((int (*) (struct vop_mkdir_args *)) procfs_badop)
#define procfs_rmdir ((int (*) (struct vop_rmdir_args *)) procfs_badop)
#define procfs_symlink ((int (*) (struct vop_symlink_args *)) procfs_badop)
int	procfs_readdir (struct vop_readdir_args *);
#define procfs_readlink ((int (*) (struct vop_readlink_args *)) procfs_badop)
int	procfs_abortop (struct vop_abortop_args *);
int	procfs_inactive (struct vop_inactive_args *);
int	procfs_reclaim (struct vop_reclaim_args *);
#define procfs_lock ((int (*) (struct vop_lock_args *)) nullop)
#define procfs_unlock ((int (*) (struct vop_unlock_args *)) nullop)
int	procfs_bmap (struct vop_bmap_args *);
#define	procfs_strategy ((int (*) (struct vop_strategy_args *)) procfs_badop)
int	procfs_print (struct vop_print_args *);
#define procfs_islocked ((int (*) (struct vop_islocked_args *)) nullop)
#define procfs_advlock ((int (*) (struct vop_advlock_args *)) procfs_badop)
#define procfs_blkatoff ((int (*) (struct vop_blkatoff_args *)) procfs_badop)
#define procfs_valloc ((int (*) (struct vop_valloc_args *)) procfs_badop)
#define procfs_vfree ((int (*) (struct vop_vfree_args *)) nullop)
#define procfs_truncate ((int (*) (struct vop_truncate_args *)) procfs_badop)
#define procfs_update ((int (*) (struct vop_update_args *)) nullop)
#endif /* KERNEL */
