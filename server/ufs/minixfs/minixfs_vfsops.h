/*
 * Copyright (c) 1989, 1991, 1993, 1994
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
 *	@(#)ffs_vfsops.c	8.8 (Berkeley) 4/18/94
 */
/* 
 * Minix filesystem for LITES
 * Copyright (c) 1994,95 Csizmazia Balazs
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CSIZMAZIA BALAZS ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CSIZMAZIA BALAZS DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 */

EXTERN minixfs_mountroot();
EXTERN int minixfs_blkatoff (struct vop_blkatoff_args *ap);
EXTERN minixfs_start (struct mount *mp, int flags, struct proc *p);
EXTERN minixfs_quotactl (struct mount *mp, int cmd, uid_t uid, caddr_t arg, struct proc *p);
EXTERN minixfs_sync (struct mount *mp, int waitfor);
EXTERN int minixfs_root (struct mount *mp, struct vnode **vpp);
EXTERN minixfs_vget ( struct mount *mp, ino_t ino, struct vnode **vpp);
EXTERN minixfs_fhtovp ( struct mount *mp, struct fid *fhp, int setgen, struct vnode **vpp);
EXTERN minixfs_vptofh ( struct vnode *vp, struct fid *fhp);
EXTERN int minixfs_mount (struct mount *, char *, caddr_t, struct nameidata *, struct proc *);
EXTERN int minixfs_mountfs (struct vnode *, struct mount *, struct proc *p);
EXTERN int minixfs_unmount (struct mount *mp, int mntflags, struct proc *p);
EXTERN int minixfs_statfs ( struct mount *mp, struct statfs *sbp, struct proc *p);
EXTERN int minixfs_vinit (struct mount *, int (**specops)(), int (**fifoops)(), struct vnode **);
EXTERN int minixfs_sbupdate (struct ufsmount *, int);
EXTERN struct vfsops minixfs_vfsops;
