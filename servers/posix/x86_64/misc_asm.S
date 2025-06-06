/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: misc_asm.s,v $
# Revision 1.1.1.1  1995/03/02  21:49:43  mike
# Initial Lites release from hut.fi
#
 * 
 */

#include <x86_64/asm.h>

#ifdef	GPROF
	.globl	_gprof_do_profiling
	.globl	_mcountaux
	.globl	mcount
	.globl	_mcount
mcount:
_mcount:
	cmpq    $0,_gprof_do_profiling
	jz	1f
	pushq   (%rsp)
	pushq   8(%rbp)
	call	_mcountaux
	leaq    16(%rsp),%rsp
1:	ret
#endif

/*
 * bcmp(s1, s2, len)
 * char *s1, *s2;
 * int len
 */
ENTRY(bcmp)
	pushq   %rsi
	pushq   %rdi

	xorq    %rax, %rax
	movq    24(%rsp), %rsi
	movq    32(%rsp), %rdi
	movq    40(%rsp), %rcx

	cld
	 repe; cmpsb
	je	0f
	incq    %rax
	
0:	popq    %rdi
	popq    %rsi
	ret
