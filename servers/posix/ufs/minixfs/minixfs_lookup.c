/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 * minixfs_lookup.c		(Csizmazia Balazs) 94,95 v0.1
 * from
 *	@(#)ufs_lookup.c	8.6 (Berkeley) 4/1/94
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

#include "diagnostic.h"

#include <sys/param.h>
#include <sys/namei.h>
#include <sys/buf.h>
#include <sys/file.h>
#include <sys/mount.h>
#include <sys/vnode.h>

#include <ufs/ufs/quota.h>
#include <ufs/ufs/inode.h>
#include <ufs/ufs/dir.h>
#include <ufs/ufs/ufsmount.h>
#include <ufs/ufs/ufs_extern.h>

#include <ufs/minixfs/minix_fs.h>

#define EXTERN extern
#include <ufs/minixfs/minixfs_vnops.h>
#include <ufs/minixfs/minixfs_vfsops.h>
#undef EXTERN
#define EXTERN
#include <ufs/minixfs/minixfs_lookup.h>
#undef EXTERN

struct	nchstats nchstats;
#if DIAGNOSTIC
int	minixdirchk = 0; /* 1 volt itt, de atirtam 0-ra */
#else
int	minixdirchk = 0;
#endif

#define FSFMT(vp)	((vp)->v_mount->mnt_maxsymlinklen <= 0)

#undef roundup
#define roundup(a,b) a

/*
 * Convert a component of a pathname into a pointer to a locked inode.
 * This is a very central and rather complicated routine.
 * If the file system is not maintained in a strict tree hierarchy,
 * this can result in a deadlock situation (see comments in code below).
 *
 * The cnp->cn_nameiop argument is LOOKUP, CREATE, RENAME, or DELETE depending
 * on whether the name is to be looked up, created, renamed, or deleted.
 * When CREATE, RENAME, or DELETE is specified, information usable in
 * creating, renaming, or deleting a directory entry may be calculated.
 * If flag has LOCKPARENT or'ed into it and the target of the pathname
 * exists, lookup returns both the target and its parent directory locked.
 * When creating or renaming and LOCKPARENT is specified, the target may
 * not be ".".  When deleting and LOCKPARENT is specified, the target may
 * be "."., but the caller must check to ensure it does an vrele and vput
 * instead of two vputs.
 *
 * Overall outline of ufs_lookup:
 *
 *	check accessibility of directory
 *	look for name in cache, if found, then if at end of path
 *	  and deleting or creating, drop it, else return name
 *	search for name in directory, to found or notfound
 * notfound:
 *	if creating, return locked directory, leaving info on available slots
 *	else return error
 * found:
 *	if at end of path and deleting, return information to allow delete
 *	if at end of path and rewriting (RENAME and LOCKPARENT), lock target
 *	  inode and return info to allow rewrite
 *	if not at end, add name to cache; if at end and neither creating
 *	  nor deleting, add name to cache
 */
int
minixfs_lookup(ap)
	struct vop_lookup_args /* {
		struct vnode *a_dvp;
		struct vnode **a_vpp;
		struct componentname *a_cnp;
	} */ *ap;
{
	register struct vnode *vdp;	/* vnode for directory being searched */
	register struct inode *dp;	/* inode for directory being searched */
	struct buf *bp;			/* a buffer of directory entries */
	register struct minix_direct *ep;	/* the current directory entry */
	int entryoffsetinblock;		/* offset of ep in bp's buffer */
	enum {NONE, COMPACT, FOUND} slotstatus;
	doff_t slotoffset;		/* offset of area with free space */
	int slotsize;			/* size of area at slotoffset */
	int slotfreespace;		/* amount of space free in slot */
	int slotneeded;			/* size of the entry we're seeking */
	int numdirpasses;		/* strategy for directory search */
	doff_t endsearch;		/* offset to end directory search */
	doff_t prevoff;			/* prev entry dp->i_offset */
	struct vnode *pdp;		/* saved dp during symlink work */
	struct vnode *tdp;		/* returned by VFS_VGET */
	doff_t enduseful;		/* pointer past last used dir slot */
	u_long bmask;			/* block offset mask */
	int lockparent;			/* 1 => lockparent flag is set */
	int wantparent;			/* 1 => wantparent or lockparent flag */
	int namlen, error;
	struct vnode **vpp = ap->a_vpp;
	struct componentname *cnp = ap->a_cnp;
	struct ucred *cred = cnp->cn_cred;
	int flags = cnp->cn_flags;
	int nameiop = cnp->cn_nameiop;

	int dentryrovidnamelen;

	bp = NULL;
	slotoffset = -1;
	*vpp = NULL;
	vdp = ap->a_dvp;
	dp = VTOI(vdp);
	lockparent = flags & LOCKPARENT;
	wantparent = flags & (LOCKPARENT|WANTPARENT);

/*
printf("minix_lookup: searching for: %s\n",cnp->cn_nameptr);
*/

	/*
	 * Check accessiblity of directory.
	 */
/* XXX!!! SKIP now.
	if ((((struct minix_inode *)(&dp->i_din))->minix_i_mode & IFMT) != IFDIR)
		return (ENOTDIR);
	if (error = VOP_ACCESS(vdp, VEXEC, cred, cnp->cn_proc))
		return (error);
*/

	/*
	 * We now have a segment name to search for, and a directory to search.
	 *
	 * Before tediously performing a linear scan of the directory,
	 * check the name cache to see if the directory/name pair
	 * we are looking for is known already.
	 */
	if (error = cache_lookup(vdp, vpp, cnp)) {
		int vpid;	/* capability number of vnode */

		if (error == ENOENT)
			return (error);
		/*
		 * Get the next vnode in the path.
		 * See comment below starting `Step through' for
		 * an explaination of the locking protocol.
		 */
		pdp = vdp;
		dp = VTOI(*vpp);
		vdp = *vpp;
		vpid = vdp->v_id;
		if (pdp == vdp) {   /* lookup on "." */
			VREF(vdp);
			error = 0;
		} else if (flags & ISDOTDOT) {
			VOP_UNLOCK(pdp);
			error = vget(vdp, 1);
			if (!error && lockparent && (flags & ISLASTCN))
				error = VOP_LOCK(pdp);
		} else {
			error = vget(vdp, 1);
			if (!lockparent || error || !(flags & ISLASTCN))
				VOP_UNLOCK(pdp);
		}
		/*
		 * Check that the capability number did not change
		 * while we were waiting for the lock.
		 */
		if (!error) {
			if (vpid == vdp->v_id)
				return (0);
			vput(vdp);
			if (lockparent && pdp != vdp && (flags & ISLASTCN))
				VOP_UNLOCK(pdp);
		}
		if (error = VOP_LOCK(pdp))
			return (error);
		vdp = pdp;
		dp = VTOI(pdp);
		*vpp = NULL;
	}

	/*
	 * Suppress search for slots unless creating
	 * file and at end of pathname, in which case
	 * we watch for a place to put the new file in
	 * case it doesn't already exist.
	 */
	slotstatus = FOUND;
	slotfreespace = slotsize = slotneeded = 0;
	if ((nameiop == CREATE || nameiop == RENAME) &&
	    (flags & ISLASTCN)) {
		slotstatus = NONE;
		slotneeded = (sizeof(struct minix_direct) - MAXNAMLEN +
			cnp->cn_namelen + 3) &~ 3; /* XXX? What is this??? */
	}

	/*
	 * If there is cached information on a previous search of
	 * this directory, pick up where we last left off.
	 * We cache only lookups as these are the most common
	 * and have the greatest payoff. Caching CREATE has little
	 * benefit as it usually must search the entire directory
	 * to determine that the entry does not exist. Caching the
	 * location of the last DELETE or RENAME has not reduced
	 * profiling time and hence has been removed in the interest
	 * of simplicity.
	 */
	bmask = VFSTOUFS(vdp->v_mount)->um_mountp->mnt_stat.f_iosize - 1;
	if (nameiop != LOOKUP || dp->i_diroff == 0 ||
	    dp->i_diroff > mi_size(dp)) {
		entryoffsetinblock = 0;
		dp->i_offset = 0;
		numdirpasses = 1;
	} else {
		dp->i_offset = dp->i_diroff;
		if ((entryoffsetinblock = dp->i_offset & bmask) &&
		    (error = VOP_BLKATOFF(vdp, (off_t)dp->i_offset, NULL, &bp)))
			return (error);
		numdirpasses = 2;
		nchstats.ncs_2passes++;
	}
	prevoff = dp->i_offset;
	endsearch = roundup(mi_size(dp), 16 /* DIRBLKSIZ */);
	enduseful = 0;


searchloop:
	while (dp->i_offset < endsearch) {
		/*
		 * If necessary, get the next directory block.
		 */
		if ((dp->i_offset & bmask) == 0) {
			if (bp != NULL)
				brelse(bp);
			if (error =
			    VOP_BLKATOFF(vdp, (off_t)dp->i_offset, NULL, &bp))
				return (error);
			entryoffsetinblock = 0;
		}
		/*
		 * If still looking for a slot, and at a MINIX_BLOCK_SIZE (formerly DIRBLKSIZE)
		 * boundary, have to start looking for free space again.
		 */
		if (slotstatus == NONE &&
		    (entryoffsetinblock & (MINIX_BLOCK_SIZE /*DIRBLKSIZ*/ - 1)) == 0) {
			slotoffset = -1;
			slotfreespace = 0;
		}
		/*
		 * Get pointer to next entry.
		 * Full validation checks are slow, so we only check
		 * enough to insure forward progress through the
		 * directory. Complete checks can be run by patching
		 * "minixdirchk" to be true.
		 */
		ep = (struct minix_direct *)((char *)bp->b_data + entryoffsetinblock);
/*
		if (ep->d_reclen == 0 ||
		    minixdirchk && kufs_dirbadentry(vdp, ep, entryoffsetinblock)) {
			int i;

			kufs_dirbad(dp, dp->i_offset, "mangled entry");
			i = MINIX_BLOCK_SIZE - (entryoffsetinblock & (MINIX_BLOCK_SIZE - 1));
			dp->i_offset += i;
			entryoffsetinblock += i;
			continue;
		}
*/

		/*
		 * If an appropriate sized slot has not yet been found,
		 * check to see if one is available. Also accumulate space
		 * in the current block so that we can determine if
		 * compaction is viable.
		 */
/* XXXX: csb -- we don't need it when used as a R/O fs.
		if (slotstatus != FOUND) {
			int size = ep->d_reclen;

			if (ep->d_ino != 0)
				size -= DIRSIZ(FSFMT(vdp), ep);
			if (size > 0) {
				if (size >= slotneeded) {
					slotstatus = FOUND;
					slotoffset = dp->i_offset;
					slotsize = ep->d_reclen;
				} else if (slotstatus == NONE) {
					slotfreespace += size;
					if (slotoffset == -1)
						slotoffset = dp->i_offset;
					if (slotfreespace >= slotneeded) {
						slotstatus = COMPACT;
						slotsize = dp->i_offset +
						      ep->d_reclen - slotoffset;
					}
				}
			}
		}
*/

		/*
		 * Check for a name match.
		 */
		if (ep->inode) { /* not zero -> isn't deleted */
			dentryrovidnamelen=0;
			while ((dentryrovidnamelen<14) && (ep->name[dentryrovidnamelen]!='\0')) dentryrovidnamelen++;
/* removed stuff
#			if (BYTE_ORDER == LITTLE_ENDIAN)
				if (vdp->v_mount->mnt_maxsymlinklen > 0)
					namlen = ep->d_namlen;
				else
					namlen = ep->d_type;
#			else
				namlen = ep->d_namlen;
#			endif
*/
			namlen = dentryrovidnamelen;
			if (namlen == cnp->cn_namelen &&
			    !memcmp(cnp->cn_nameptr, ep->name, (unsigned)namlen)) {
				/*
				 * Save directory entry's inode number and
				 * reclen in ndp->ni_ufs area, and release
				 * directory buffer.
				 */
				dp->i_ino = ep->inode;
				dp->i_reclen = 9999; /* XXX csb */
				brelse(bp);
				goto found;
			}
		}
		prevoff = dp->i_offset;
		dp->i_offset += 16;
		entryoffsetinblock += 16;
		if (ep->inode)
			enduseful = dp->i_offset;
	}
/* notfound: */
/* debug
printf("notfound INODE NR:%d\n",(ep->inode));
*/
	/*
	 * If we started in the middle of the directory and failed
	 * to find our target, we must check the beginning as well.
	 */
	if (numdirpasses == 2) {
		numdirpasses--;
		dp->i_offset = 0;
		endsearch = dp->i_diroff;
		goto searchloop;
	}
	if (bp != NULL)
		brelse(bp);
	/*
	 * If creating, and at end of pathname and current
	 * directory has not been removed, then can consider
	 * allowing file to be created.
	 */
	if ((nameiop == CREATE || nameiop == RENAME) &&
	    (flags & ISLASTCN) && mi_nlinks(dp) != 0) {
		/*
		 * Access for write is interpreted as allowing
		 * creation of files in the directory.
		 */
		if (error = VOP_ACCESS(vdp, VWRITE, cred, cnp->cn_proc))
			return (error);
		/*
		 * Return an indication of where the new directory
		 * entry should be put.  If we didn't find a slot,
		 * then set dp->i_count to 0 indicating
		 * that the new slot belongs at the end of the
		 * directory. If we found a slot, then the new entry
		 * can be put in the range from dp->i_offset to
		 * dp->i_offset + dp->i_count.
		 */
		if (slotstatus == NONE) {
			dp->i_offset = roundup(mi_size(dp), MINIX_BLOCK_SIZE /*DIRBLKSIZ*/);
			dp->i_count = 0;
			enduseful = dp->i_offset;
		} else {
			dp->i_offset = slotoffset;
			dp->i_count = slotsize;
			if (enduseful < slotoffset + slotsize)
				enduseful = slotoffset + slotsize;
		}
		dp->i_endoff = roundup(enduseful, MINIX_BLOCK_SIZE /* DIRBLKSIZ */);
		dp->i_flag |= IN_CHANGE | IN_UPDATE;
		/*
		 * We return with the directory locked, so that
		 * the parameters we set up above will still be
		 * valid if we actually decide to do a direnter().
		 * We return ni_vp == NULL to indicate that the entry
		 * does not currently exist; we leave a pointer to
		 * the (locked) directory inode in ndp->ni_dvp.
		 * The pathname buffer is saved so that the name
		 * can be obtained later.
		 *
		 * NB - if the directory is unlocked, then this
		 * information cannot be used.
		 */
		cnp->cn_flags |= SAVENAME;
		if (!lockparent)
			VOP_UNLOCK(vdp);
		return (EJUSTRETURN);
	}
	/*
	 * Insert name into cache (as non-existent) if appropriate.
	 */
	if ((cnp->cn_flags & MAKEENTRY) && nameiop != CREATE)
		cache_enter(vdp, *vpp, cnp);
	return (ENOENT);

found:
/* debug
printf("found INODE NR:%d\n",(ep->inode));
*/

	if (numdirpasses == 2)
		nchstats.ncs_pass2++;
	/*
	 * Check that directory length properly reflects presence
	 * of this entry.
	 */
/*
	if (entryoffsetinblock + DIRSIZ(FSFMT(vdp), ep) > mi_size(dp)) {
		ufs_dirbad(dp, dp->i_offset, "i_size too small");  XXX emergency
		dp->i_size = entryoffsetinblock + DIRSIZ(FSFMT(vdp), ep); XXX inode minixification
		dp->i_flag |= IN_CHANGE | IN_UPDATE;
	}
*/


	/*
	 * Found component in pathname.
	 * If the final component of path name, save information
	 * in the cache as to where the entry was found.
	 */
	if ((flags & ISLASTCN) && nameiop == LOOKUP)
		dp->i_diroff = dp->i_offset &~ (MINIX_BLOCK_SIZE /*DIRBLKSIZ*/ - 1);

	/*
	 * If deleting, and at end of pathname, return
	 * parameters which can be used to remove file.
	 * If the wantparent flag isn't set, we return only
	 * the directory (in ndp->ni_dvp), otherwise we go
	 * on and lock the inode, being careful with ".".
	 */
	if (nameiop == DELETE && (flags & ISLASTCN)) {
		/*
		 * Write access to directory required to delete files.
		 */
		if (error = VOP_ACCESS(vdp, VWRITE, cred, cnp->cn_proc))
			return (error);
		/*
		 * Return pointer to current entry in dp->i_offset,
		 * and distance past previous entry (if there
		 * is a previous entry in this block) in dp->i_count.
		 * Save directory inode pointer in ndp->ni_dvp for dirremove().
		 */
		if ((dp->i_offset & (MINIX_BLOCK_SIZE /*DIRBLKSIZ*/ - 1)) == 0)
			dp->i_count = 0;
		else
			dp->i_count = dp->i_offset - prevoff;
		if (dp->i_number == dp->i_ino) {
			VREF(vdp);
			*vpp = vdp;
			return (0);
		}
		if (error = VFS_VGET(vdp->v_mount, dp->i_ino, &tdp))
			return (error);
		/*
		 * If directory is "sticky", then user must own
		 * the directory, or the file in it, else she
		 * may not delete it (unless she's root). This
		 * implements append-only directories.
		 */
		if ((mi_mode(dp) & ISVTX) &&
		    cred->cr_uid != 0 &&
		    cred->cr_uid != mi_uid(dp) &&
		    mi_uid(VTOI(tdp)) != cred->cr_uid) {
			vput(tdp);
			return (EPERM);
		}
		*vpp = tdp;
		if (!lockparent)
			VOP_UNLOCK(vdp);
		return (0);
	}

	/*
	 * If rewriting (RENAME), return the inode and the
	 * information required to rewrite the present directory
	 * Must get inode of directory entry to verify it's a
	 * regular file, or empty directory.
	 */
	if (nameiop == RENAME && wantparent &&
	    (flags & ISLASTCN)) {
		if (error = VOP_ACCESS(vdp, VWRITE, cred, cnp->cn_proc))
			return (error);
		/*
		 * Careful about locking second inode.
		 * This can only occur if the target is ".".
		 */
		if (dp->i_number == dp->i_ino)
			return (EISDIR);
		if (error = VFS_VGET(vdp->v_mount, dp->i_ino, &tdp))
			return (error);
		*vpp = tdp;
		cnp->cn_flags |= SAVENAME;
		if (!lockparent)
			VOP_UNLOCK(vdp);
		return (0);
	}

	/*
	 * Step through the translation in the name.  We do not `vput' the
	 * directory because we may need it again if a symbolic link
	 * is relative to the current directory.  Instead we save it
	 * unlocked as "pdp".  We must get the target inode before unlocking
	 * the directory to insure that the inode will not be removed
	 * before we get it.  We prevent deadlock by always fetching
	 * inodes from the root, moving down the directory tree. Thus
	 * when following backward pointers ".." we must unlock the
	 * parent directory before getting the requested directory.
	 * There is a potential race condition here if both the current
	 * and parent directories are removed before the VFS_VGET for the
	 * inode associated with ".." returns.  We hope that this occurs
	 * infrequently since we cannot avoid this race condition without
	 * implementing a sophisticated deadlock detection algorithm.
	 * Note also that this simple deadlock detection scheme will not
	 * work if the file system has any hard links other than ".."
	 * that point backwards in the directory structure.
	 */
	pdp = vdp;
	if (flags & ISDOTDOT) {
		VOP_UNLOCK(pdp);	/* race to get the inode */
		if (error = VFS_VGET(vdp->v_mount, dp->i_ino, &tdp)) {
			VOP_LOCK(pdp);
			return (error);
		}
		if (lockparent && (flags & ISLASTCN) &&
		    (error = VOP_LOCK(pdp))) {
			vput(tdp);
			return (error);
		}
		*vpp = tdp;
	} else if (dp->i_number == dp->i_ino) {
		VREF(vdp);	/* we want ourself, ie "." */
		*vpp = vdp;
	} else {
		if (error = VFS_VGET(vdp->v_mount, dp->i_ino, &tdp))
			return (error);
		if (!lockparent || !(flags & ISLASTCN))
			VOP_UNLOCK(pdp);
		*vpp = tdp;
	}

	/*
	 * Insert name into cache if appropriate.
	 */
	if (cnp->cn_flags & MAKEENTRY)
		cache_enter(vdp, *vpp, cnp);
/* debug
printf("return .... lokkupbol!\n");
*/
	return (0);
}

void
minixfs_dirbad(ip, offset, how)
	struct inode *ip;
	doff_t offset;
	char *how;
{
	struct mount *mp;

	mp = ITOV(ip)->v_mount;
	(void)printf("%s: bad dir ino %d at offset %d: %s\n",
	    mp->mnt_stat.f_mntonname, ip->i_number, offset, how);
	if ((mp->mnt_stat.f_flags & MNT_RDONLY) == 0)
		panic("bad dir");
}

/*
 * Do consistency checking on a directory entry:
 *	record length must be multiple of 4
 *	entry must fit in rest of its MINIX_BLOCK_SIZE block
 *	record must be large enough to contain entry
 *	name is not longer than MAXNAMLEN
 *	name must be as long as advertised, and null terminated
 */
int
minixfs_dirbadentry(dp, ep, entryoffsetinblock)
	struct vnode *dp;
	register struct direct *ep;
	int entryoffsetinblock;
{
	register int i;
	int namlen;

#	if (BYTE_ORDER == LITTLE_ENDIAN)
		if (dp->v_mount->mnt_maxsymlinklen > 0)
			namlen = ep->d_namlen;
		else
			namlen = ep->d_type;
#	else
		namlen = ep->d_namlen;
#	endif
	if ((ep->d_reclen & 0x3) != 0 ||
	    ep->d_reclen > MINIX_BLOCK_SIZE /*DIRBLKSIZ*/ - (entryoffsetinblock & (MINIX_BLOCK_SIZE /*DIRBLKSIZ*/ - 1)) ||
	    ep->d_reclen < DIRSIZ(FSFMT(dp), ep) || namlen > MAXNAMLEN) {
		/*return (1); */
		printf("First bad\n");
		goto bad;
	}
	for (i = 0; i < namlen; i++)
		if (ep->d_name[i] == '\0') {
			/*return (1); */
			printf("Second bad\n");
			goto bad;
	}
	if (ep->d_name[i])
		goto bad;
	return (ep->d_name[i]);
bad:
	return(1);
}

/* The routines below are from the ufs_lookup() routines, and are not
 * necessary when used as a R/O fs. I don't delete them, but later they
 * will be rewritten for MINIX.
 */

/*
 * Write a directory entry after a call to namei, using the parameters
 * that it left in nameidata.  The argument ip is the inode which the new
 * directory entry will refer to.  Dvp is a pointer to the directory to
 * be written, which was left locked by namei. Remaining parameters
 * (dp->i_offset, dp->i_count) indicate how the space for the new
 * entry is to be obtained.
 */
int
minixfs_direnter(ip, dvp, cnp)
	struct inode *ip;
	struct vnode *dvp;
	register struct componentname *cnp;
{
	register struct direct *ep, *nep;
	register struct inode *dp;
	struct buf *bp;
	struct direct newdir;
	struct iovec aiov;
	struct uio auio;
	u_int dsize;
	int error, loc, newentrysize, spacefree;
	char *dirbuf;

#if DIAGNOSTIC
	if ((cnp->cn_flags & SAVENAME) == 0)
		panic("direnter: missing name");
#endif
	dp = VTOI(dvp);
	newdir.d_ino = ip->i_number;
	newdir.d_namlen = cnp->cn_namelen;
	memcpy(newdir.d_name, cnp->cn_nameptr, (unsigned)cnp->cn_namelen + 1);
	if (dvp->v_mount->mnt_maxsymlinklen > 0)
		newdir.d_type = IFTODT(ip->i_mode);
	else {
		newdir.d_type = 0;
#		if (BYTE_ORDER == LITTLE_ENDIAN)
			{ u_char tmp = newdir.d_namlen;
			newdir.d_namlen = newdir.d_type;
			newdir.d_type = tmp; }
#		endif
	}
	newentrysize = DIRSIZ(FSFMT(dvp), &newdir);
	if (dp->i_count == 0) {
		/*
		 * If dp->i_count is 0, then namei could find no
		 * space in the directory. Here, dp->i_offset will
		 * be on a directory block boundary and we will write the
		 * new entry into a fresh block.
		 */
		if (dp->i_offset & (MINIX_BLOCK_SIZE /*DIRBLKSIZ*/ - 1))
			panic("ufs_direnter: newblk");
		auio.uio_offset = dp->i_offset;
		newdir.d_reclen = MINIX_BLOCK_SIZE /*DIRBLKSIZ*/;
		auio.uio_resid = newentrysize;
		aiov.iov_len = newentrysize;
		aiov.iov_base = (caddr_t)&newdir;
		auio.uio_iov = &aiov;
		auio.uio_iovcnt = 1;
		auio.uio_rw = UIO_WRITE;
		auio.uio_segflg = UIO_SYSSPACE;
		auio.uio_procp = (struct proc *)0;
		error = VOP_WRITE(dvp, &auio, IO_SYNC, cnp->cn_cred);
		if (MINIX_BLOCK_SIZE /*DIRBLKSIZ*/ >
		    VFSTOUFS(dvp->v_mount)->um_mountp->mnt_stat.f_bsize)
			/* XXX should grow with balloc() */
			panic("ufs_direnter: frag size");
		else if (!error) {
			dp->i_size = roundup(dp->i_size, MINIX_BLOCK_SIZE /*DIRBLKSIZ*/);
			dp->i_flag |= IN_CHANGE;
		}
		return (error);
	}

	/*
	 * If dp->i_count is non-zero, then namei found space
	 * for the new entry in the range dp->i_offset to
	 * dp->i_offset + dp->i_count in the directory.
	 * To use this space, we may have to compact the entries located
	 * there, by copying them together towards the beginning of the
	 * block, leaving the free space in one usable chunk at the end.
	 */

	/*
	 * Increase size of directory if entry eats into new space.
	 * This should never push the size past a new multiple of
	 * MINIX_BLOCK_SIZE 
	 *
	 * N.B. - THIS IS AN ARTIFACT OF 4.2 AND SHOULD NEVER HAPPEN.
	 */
	if (dp->i_offset + dp->i_count > dp->i_size)
		dp->i_size = dp->i_offset + dp->i_count;
	/*
	 * Get the block containing the space for the new directory entry.
	 */
	if (error = VOP_BLKATOFF(dvp, (off_t)dp->i_offset, &dirbuf, &bp))
		return (error);
	/*
	 * Find space for the new entry. In the simple case, the entry at
	 * offset base will have the space. If it does not, then namei
	 * arranged that compacting the region dp->i_offset to
	 * dp->i_offset + dp->i_count would yield the
	 * space.
	 */
	ep = (struct direct *)dirbuf;
	dsize = DIRSIZ(FSFMT(dvp), ep);
	spacefree = ep->d_reclen - dsize;
	for (loc = ep->d_reclen; loc < dp->i_count; ) {
		nep = (struct direct *)(dirbuf + loc);
		if (ep->d_ino) {
			/* trim the existing slot */
			ep->d_reclen = dsize;
			ep = (struct direct *)((char *)ep + dsize);
		} else {
			/* overwrite; nothing there; header is ours */
			spacefree += dsize;
		}
		dsize = DIRSIZ(FSFMT(dvp), nep);
		spacefree += nep->d_reclen - dsize;
		loc += nep->d_reclen;
		memcpy((caddr_t)ep, (caddr_t)nep, dsize);
	}
	/*
	 * Update the pointer fields in the previous entry (if any),
	 * copy in the new entry, and write out the block.
	 */
	if (ep->d_ino == 0) {
		if (spacefree + dsize < newentrysize)
			panic("ufs_direnter: compact1");
		newdir.d_reclen = spacefree + dsize;
	} else {
		if (spacefree < newentrysize)
			panic("ufs_direnter: compact2");
		newdir.d_reclen = spacefree;
		ep->d_reclen = dsize;
		ep = (struct direct *)((char *)ep + dsize);
	}
	memcpy((caddr_t)ep, (caddr_t)&newdir, (u_int)newentrysize);
	error = VOP_BWRITE(bp);
	dp->i_flag |= IN_CHANGE | IN_UPDATE;
	if (!error && dp->i_endoff && dp->i_endoff < dp->i_size)
		error = VOP_TRUNCATE(dvp, (off_t)dp->i_endoff, IO_SYNC,
		    cnp->cn_cred, cnp->cn_proc);
	return (error);
}

/*
 * Remove a directory entry after a call to namei, using
 * the parameters which it left in nameidata. The entry
 * dp->i_offset contains the offset into the directory of the
 * entry to be eliminated.  The dp->i_count field contains the
 * size of the previous record in the directory.  If this
 * is 0, the first entry is being deleted, so we need only
 * zero the inode number to mark the entry as free.  If the
 * entry is not the first in the directory, we must reclaim
 * the space of the now empty record by adding the record size
 * to the size of the previous entry.
 */
int
minixfs_dirremove(dvp, cnp)
	struct vnode *dvp;
	struct componentname *cnp;
{
	register struct inode *dp;
	struct direct *ep;
	struct buf *bp;
	int error;

	dp = VTOI(dvp);
	if (dp->i_count == 0) {
		/*
		 * First entry in block: set d_ino to zero.
		 */
		if (error =
		    VOP_BLKATOFF(dvp, (off_t)dp->i_offset, (char **)&ep, &bp))
			return (error);
		ep->d_ino = 0;
		error = VOP_BWRITE(bp);
		dp->i_flag |= IN_CHANGE | IN_UPDATE;
		return (error);
	}
	/*
	 * Collapse new free space into previous entry.
	 */
	if (error = VOP_BLKATOFF(dvp, (off_t)(dp->i_offset - dp->i_count),
	    (char **)&ep, &bp))
		return (error);
	ep->d_reclen += dp->i_reclen;
	error = VOP_BWRITE(bp);
	dp->i_flag |= IN_CHANGE | IN_UPDATE;
	return (error);
}

/*
 * Rewrite an existing directory entry to point at the inode
 * supplied.  The parameters describing the directory entry are
 * set up by a call to namei.
 */
int
minixfs_dirrewrite(dp, ip, cnp)
	struct inode *dp, *ip;
	struct componentname *cnp;
{
	struct buf *bp;
	struct direct *ep;
	struct vnode *vdp = ITOV(dp);
	int error;

	if (error = VOP_BLKATOFF(vdp, (off_t)dp->i_offset, (char **)&ep, &bp))
		return (error);
	ep->d_ino = ip->i_number;
	if (vdp->v_mount->mnt_maxsymlinklen > 0)
		ep->d_type = IFTODT(ip->i_mode);
	error = VOP_BWRITE(bp);
	dp->i_flag |= IN_CHANGE | IN_UPDATE;
	return (error);
}

/*
 * Check if a directory is empty or not.
 * Inode supplied must be locked.
 *
 * Using a struct dirtemplate here is not precisely
 * what we want, but better than using a struct direct.
 *
 * NB: does not handle corrupted directories.
 */
int
minixfs_dirempty(ip, parentino, cred)
	register struct inode *ip;
	ino_t parentino;
	struct ucred *cred;
{
	register off_t off;
	struct dirtemplate dbuf;
	register struct direct *dp = (struct direct *)&dbuf;
	int error, count, namlen;
#define	MINDIRSIZ (sizeof (struct dirtemplate) / 2)

	for (off = 0; off < ip->i_size; off += dp->d_reclen) {
		error = vn_rdwr(UIO_READ, ITOV(ip), (caddr_t)dp, MINDIRSIZ, off,
		   UIO_SYSSPACE, IO_NODELOCKED, cred, &count, (struct proc *)0);
		/*
		 * Since we read MINDIRSIZ, residual must
		 * be 0 unless we're at end of file.
		 */
		if (error || count != 0)
			return (0);
		/* avoid infinite loops */
		if (dp->d_reclen == 0)
			return (0);
		/* skip empty entries */
		if (dp->d_ino == 0)
			continue;
		/* accept only "." and ".." */
#		if (BYTE_ORDER == LITTLE_ENDIAN)
			if (ITOV(ip)->v_mount->mnt_maxsymlinklen > 0)
				namlen = dp->d_namlen;
			else
				namlen = dp->d_type;
#		else
			namlen = dp->d_namlen;
#		endif
		if (namlen > 2)
			return (0);
		if (dp->d_name[0] != '.')
			return (0);
		/*
		 * At this point namlen must be 1 or 2.
		 * 1 implies ".", 2 implies ".." if second
		 * char is also "."
		 */
		if (namlen == 1)
			continue;
		if (dp->d_name[1] == '.' && dp->d_ino == parentino)
			continue;
		return (0);
	}
	return (1);
}

/*
 * Check if source directory is in the path of the target directory.
 * Target is supplied locked, source is unlocked.
 * The target is always vput before returning.
 */
int
minixfs_checkpath(source, target, cred)
	struct inode *source, *target;
	struct ucred *cred;
{
	struct vnode *vp;
	int error, rootino, namlen;
	struct dirtemplate dirbuf;

	vp = ITOV(target);
	if (target->i_number == source->i_number) {
		error = EEXIST;
		goto out;
	}
	rootino = ROOTINO;
	error = 0;
	if (target->i_number == rootino)
		goto out;

	for (;;) {
		if (vp->v_type != VDIR) {
			error = ENOTDIR;
			break;
		}
		error = vn_rdwr(UIO_READ, vp, (caddr_t)&dirbuf,
			sizeof (struct dirtemplate), (off_t)0, UIO_SYSSPACE,
			IO_NODELOCKED, cred, (int *)0, (struct proc *)0);
		if (error != 0)
			break;
#		if (BYTE_ORDER == LITTLE_ENDIAN)
			if (vp->v_mount->mnt_maxsymlinklen > 0)
				namlen = dirbuf.dotdot_namlen;
			else
				namlen = dirbuf.dotdot_type;
#		else
			namlen = dirbuf.dotdot_namlen;
#		endif
		if (namlen != 2 ||
		    dirbuf.dotdot_name[0] != '.' ||
		    dirbuf.dotdot_name[1] != '.') {
			error = ENOTDIR;
			break;
		}
		if (dirbuf.dotdot_ino == source->i_number) {
			error = EINVAL;
			break;
		}
		if (dirbuf.dotdot_ino == rootino)
			break;
		vput(vp);
		if (error = VFS_VGET(vp->v_mount, dirbuf.dotdot_ino, &vp)) {
			vp = NULL;
			break;
		}
	}

out:
	if (error == ENOTDIR)
		printf("checkpath: .. not a directory\n");
	if (vp != NULL)
		vput(vp);
	return (error);
}
