/*-
 * Copyright (c) 1982, 1986, 1991, 1993
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
 *	@(#)kern_resource.c	8.5 (Berkeley) 1/21/94
 */

#include "compat_43.h"

#include <serv/server_defs.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/resourcevar.h>
#include <sys/malloc.h>
#include <sys/proc.h>

#include <vm/vm.h>

/*
 * Resource controls and accounting.
 */

struct getpriority_args {
	int	which;
	int	who;
};
getpriority(curp, uap, retval)
	struct proc *curp;
	register struct getpriority_args *uap;
	int *retval;
{
	register struct proc *p;
	register int low = PRIO_MAX + 1;

	switch (uap->which) {

	case PRIO_PROCESS:
		if (uap->who == 0)
			p = curp;
		else
			p = pfind(uap->who);
		if (p == 0)
			break;
		low = p->p_nice;
		break;

	case PRIO_PGRP: {
		register struct pgrp *pg;

		if (uap->who == 0)
			pg = curp->p_pgrp;
		else if ((pg = pgfind(uap->who)) == NULL)
			break;
		for (p = pg->pg_mem; p != NULL; p = p->p_pgrpnxt) {
			if (p->p_nice < low)
				low = p->p_nice;
		}
		break;
	}

	case PRIO_USER:
		if (uap->who == 0)
			uap->who = curp->p_ucred->cr_uid;
		for (p = (struct proc *)allproc; p != NULL; p = p->p_next) {
			if (p->p_ucred->cr_uid == uap->who &&
			    p->p_nice < low)
				low = p->p_nice;
		}
		break;

	default:
		return (EINVAL);
	}
	if (low == PRIO_MAX + 1)
		return (ESRCH);
	*retval = low;
	return (0);
}

struct setpriority_args {
	int	which;
	int	who;
	int	prio;
};
/* ARGSUSED */
setpriority(curp, uap, retval)
	struct proc *curp;
	register struct setpriority_args *uap;
	int *retval;
{
	register struct proc *p;
	int found = 0, error = 0;

	switch (uap->which) {

	case PRIO_PROCESS:
		if (uap->who == 0)
			p = curp;
		else
			p = pfind(uap->who);
		if (p == 0)
			break;
		error = donice(curp, p, uap->prio);
		found++;
		break;

	case PRIO_PGRP: {
		register struct pgrp *pg;
		 
		if (uap->who == 0)
			pg = curp->p_pgrp;
		else if ((pg = pgfind(uap->who)) == NULL)
			break;
		for (p = pg->pg_mem; p != NULL; p = p->p_pgrpnxt) {
			error = donice(curp, p, uap->prio);
			found++;
		}
		break;
	}

	case PRIO_USER:
		if (uap->who == 0)
			uap->who = curp->p_ucred->cr_uid;
		for (p = (struct proc *)allproc; p != NULL; p = p->p_next)
			if (p->p_ucred->cr_uid == uap->who) {
				error = donice(curp, p, uap->prio);
				found++;
			}
		break;

	default:
		return (EINVAL);
	}
	if (found == 0)
		return (ESRCH);
	return (error);
}

extern mach_error_t set_task_priority(task_t task, int prio);

mach_error_t donice(
	struct proc *curp,
	struct proc *chgp,
	int n)
{
	register struct pcred *pcred = curp->p_cred;
	int prio;

	if (n < 0)		/* XXX */
	    n = 0;		/* XXX should do thread_max_priority */
	if (pcred->pc_ucred->cr_uid && pcred->p_ruid &&
	    pcred->pc_ucred->cr_uid != chgp->p_ucred->cr_uid &&
	    pcred->p_ruid != chgp->p_ucred->cr_uid)
		return (EPERM);
	if (n > PRIO_MAX)
		n = PRIO_MAX;
	if (n < PRIO_MIN)
		n = PRIO_MIN;
	if (n < chgp->p_nice && suser(pcred->pc_ucred, &curp->p_acflag))
		return (EACCES);
	chgp->p_nice = n;

	/* Mach priorities are in the range 0 to 31.  Base is 12 */ 
	prio = 12 + (n >> 1);

	return set_task_priority(chgp->p_task, prio);
}

#if OSFMACH3
mach_error_t set_task_priority(task_t task, int prio)
{
	kern_return_t kr;
	struct policy_timeshare_base base = { prio };
	struct policy_timeshare_limit limit = { prio };

	kr = task_set_policy(task, default_processor_set, POLICY_TIMESHARE,
			     (policy_base_t)&base,
			     POLICY_TIMESHARE_BASE_COUNT,
			     (policy_limit_t)&limit,
			     POLICY_TIMESHARE_LIMIT_COUNT,
			     TRUE);
	assert(kr == KERN_SUCCESS);
}

#else /* OSFMACH3 */

/* XXX Currently implements just positive nice values */
mach_error_t set_task_priority(task_t task, int prio)
{
	mach_error_t kr;
	kr = task_priority(task, prio, TRUE);
	if (kr == KERN_FAILURE)
	    kr = EACCES;
	return kr;
}
#endif /* OSFMACH3 */

#if COMPAT_43 || defined(COMPAT_SUNOS)
struct setrlimit_args {
	u_int	which;
	struct	orlimit *lim;
};
/* ARGSUSED */
osetrlimit(p, uap, retval)
	struct proc *p;
	register struct setrlimit_args *uap;
	int *retval;
{
	struct orlimit olim;
	struct rlimit lim;
	int error;

	if (error =
	    copyin((caddr_t)uap->lim, (caddr_t)&olim, sizeof (struct orlimit)))
		return (error);
	lim.rlim_cur = olim.rlim_cur;
	lim.rlim_max = olim.rlim_max;
	return (dosetrlimit(p, uap->which, &lim));
}

struct getrlimit_args {
	u_int	which;
	struct	orlimit *rlp;
};
/* ARGSUSED */
ogetrlimit(p, uap, retval)
	struct proc *p;
	register struct getrlimit_args *uap;
	int *retval;
{
	struct orlimit olim;

	if (uap->which >= RLIM_NLIMITS)
		return (EINVAL);
	olim.rlim_cur = p->p_rlimit[uap->which].rlim_cur;
	if (olim.rlim_cur == -1)
		olim.rlim_cur = 0x7fffffff;
	olim.rlim_max = p->p_rlimit[uap->which].rlim_max;
	if (olim.rlim_max == -1)
		olim.rlim_max = 0x7fffffff;
	return (copyout((caddr_t)&olim, (caddr_t)uap->rlp, sizeof(olim)));
}
#endif /* COMPAT_43 || COMPAT_SUNOS */

struct __setrlimit_args {
	u_int	which;
	struct	rlimit *lim;
};
/* ARGSUSED */
s_setrlimit(p, uap, retval)
	struct proc *p;
	register struct __setrlimit_args *uap;
	int *retval;
{
	struct rlimit alim;
	int error;

	if (error =
	    copyin((caddr_t)uap->lim, (caddr_t)&alim, sizeof (struct rlimit)))
		return (error);
	return (dosetrlimit(p, uap->which, &alim));
}

dosetrlimit(p, which, limp)
	struct proc *p;
	u_int which;
	struct rlimit *limp;
{
	register struct rlimit *alimp;
	extern unsigned maxdmap;
	int error;

	if (which >= RLIM_NLIMITS)
		return (EINVAL);
	alimp = &p->p_rlimit[which];
	if (limp->rlim_cur > alimp->rlim_max || 
	    limp->rlim_max > alimp->rlim_max)
		if (error = suser(p->p_ucred, &p->p_acflag))
			return (error);
	if (limp->rlim_cur > limp->rlim_max)
		limp->rlim_cur = limp->rlim_max;
	if (p->p_limit->p_refcnt > 1 &&
	    (p->p_limit->p_lflags & PL_SHAREMOD) == 0) {
		p->p_limit->p_refcnt--;
		p->p_limit = limcopy(p->p_limit);
		alimp = &p->p_rlimit[which];
	}

	switch (which) {

	case RLIMIT_DATA:
		if (limp->rlim_cur > maxdmap)
			limp->rlim_cur = maxdmap;
		if (limp->rlim_max > maxdmap)
			limp->rlim_max = maxdmap;
		break;

	case RLIMIT_STACK:
		if (limp->rlim_cur > maxdmap)
			limp->rlim_cur = maxdmap;
		if (limp->rlim_max > maxdmap)
			limp->rlim_max = maxdmap;
		/*
		 * Stack is allocated to the max at exec time with only
		 * "rlim_cur" bytes accessible.  If stack limit is going
		 * up make more accessible, if going down make inaccessible.
		 */
		if (limp->rlim_cur != alimp->rlim_cur) {
			vm_offset_t addr;
			vm_size_t size;
			vm_prot_t prot;

			if (limp->rlim_cur > alimp->rlim_cur) {
				prot = VM_PROT_ALL;
				size = limp->rlim_cur - alimp->rlim_cur;
				addr = USRSTACK - limp->rlim_cur;
			} else {
				prot = VM_PROT_NONE;
				size = alimp->rlim_cur - limp->rlim_cur;
				addr = USRSTACK - alimp->rlim_cur;
			}
#ifndef LITES
			addr = trunc_page(addr);
			size = round_page(size);
			(void) vm_map_protect(&p->p_vmspace->vm_map,
					      addr, addr+size, prot, FALSE);
#endif /* LITES */
		}
		break;

	case RLIMIT_NOFILE:
		if (limp->rlim_cur > maxfiles)
			limp->rlim_cur = maxfiles;
		if (limp->rlim_max > maxfiles)
			limp->rlim_max = maxfiles;
		break;

	case RLIMIT_NPROC:
		if (limp->rlim_cur > maxproc)
			limp->rlim_cur = maxproc;
		if (limp->rlim_max > maxproc)
			limp->rlim_max = maxproc;
		break;
	}
	*alimp = *limp;
	return (0);
}

struct __getrlimit_args {
	u_int	which;
	struct	rlimit *rlp;
};
/* ARGSUSED */
getrlimit(p, uap, retval)
	struct proc *p;
	register struct __getrlimit_args *uap;
	int *retval;
{

	if (uap->which >= RLIM_NLIMITS)
		return (EINVAL);
	return (copyout((caddr_t)&p->p_rlimit[uap->which], (caddr_t)uap->rlp,
	    sizeof (struct rlimit)));
}

/*
 * Transform the running time and tick information in proc p into user,
 * system, and interrupt time usage.
 */
calcru(p, up, sp, ip)
	register struct proc *p;
	register struct timeval *up;
	register struct timeval *sp;
	register struct timeval *ip;
{
#ifdef LITES
#else
	register u_quad_t u, st, ut, it, tot;
	register u_long sec, usec;
	register int s;
	struct timeval tv;

	s = splstatclock();
	st = p->p_sticks;
	ut = p->p_uticks;
	it = p->p_iticks;
	splx(s);

	tot = st + ut + it;
	if (tot == 0) {
		up->tv_sec = up->tv_usec = 0;
		sp->tv_sec = sp->tv_usec = 0;
		if (ip != NULL)
			ip->tv_sec = ip->tv_usec = 0;
		return;
	}

	sec = p->p_rtime.tv_sec;
	usec = p->p_rtime.tv_usec;
	if (p == curproc) {
		/*
		 * Adjust for the current time slice.  This is actually fairly
		 * important since the error here is on the order of a time
		 * quantum, which is much greater than the sampling error.
		 */
		microtime(&tv);
		sec += tv.tv_sec - runtime.tv_sec;
		usec += tv.tv_usec - runtime.tv_usec;
	}
	u = sec * 1000000 + usec;
	st = (u * st) / tot;
	sp->tv_sec = st / 1000000;
	sp->tv_usec = st % 1000000;
	ut = (u * ut) / tot;
	up->tv_sec = ut / 1000000;
	up->tv_usec = ut % 1000000;
	if (ip != NULL) {
		it = (u * it) / tot;
		ip->tv_sec = it / 1000000;
		ip->tv_usec = it % 1000000;
	}
#endif
}

#ifndef LITES			/* in serv_syscalls.c */
struct getrusage_args {
	int	who;
	struct	rusage *rusage;
};
/* ARGSUSED */
getrusage(p, uap, retval)
	register struct proc *p;
	register struct getrusage_args *uap;
	int *retval;
{
	register struct rusage *rup;

	switch (uap->who) {

	case RUSAGE_SELF:
		rup = &p->p_stats->p_ru;
		calcru(p, &rup->ru_utime, &rup->ru_stime, NULL);
		break;

	case RUSAGE_CHILDREN:
		rup = &p->p_stats->p_cru;
		break;

	default:
		return (EINVAL);
	}
	return (copyout((caddr_t)rup, (caddr_t)uap->rusage,
	    sizeof (struct rusage)));
}
#endif /* LITES */

ruadd(ru, ru2)
	register struct rusage *ru, *ru2;
{
	register long *ip, *ip2;
	register int i;

	timevaladd(&ru->ru_utime, &ru2->ru_utime);
	timevaladd(&ru->ru_stime, &ru2->ru_stime);
	if (ru->ru_maxrss < ru2->ru_maxrss)
		ru->ru_maxrss = ru2->ru_maxrss;
	ip = &ru->ru_first; ip2 = &ru2->ru_first;
	for (i = &ru->ru_last - &ru->ru_first; i >= 0; i--)
		*ip++ += *ip2++;
}

/*
 * Make a copy of the plimit structure.
 * We share these structures copy-on-write after fork,
 * and copy when a limit is changed.
 */
struct plimit *
limcopy(lim)
	struct plimit *lim;
{
	register struct plimit *copy;

	MALLOC(copy, struct plimit *, sizeof(struct plimit),
	    M_SUBPROC, M_WAITOK);
	memcpy(copy->pl_rlimit, lim->pl_rlimit, sizeof(struct rlimit) * RLIM_NLIMITS);
	copy->p_lflags = 0;
	copy->p_refcnt = 1;
	return (copy);
}
