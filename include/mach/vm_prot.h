/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_VM_PROT_H_
#define	_MACH_VM_PROT_H_

/*
 *	Virtual memory protection definitions.
 */

typedef int		vm_prot_t;

/*
 *	Protection values, defined as bits within the vm_prot_t type
 */

#define	VM_PROT_NONE	((vm_prot_t) 0x00)

#define VM_PROT_READ	((vm_prot_t) 0x01)	/* read permission */
#define VM_PROT_WRITE	((vm_prot_t) 0x02)	/* write permission */
#define VM_PROT_EXECUTE	((vm_prot_t) 0x04)	/* execute permission */

/*
 *	The default protection for newly-created virtual memory
 */

#define VM_PROT_DEFAULT	(VM_PROT_READ|VM_PROT_WRITE)

/*
 *	The maximum privileges possible, for parameter checking.
 */

#define VM_PROT_ALL	(VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE)

/*
 *	An invalid protection value.
 */

#define VM_PROT_NO_CHANGE	((vm_prot_t) 0x08)

/*
 *      When a caller finds that he cannot obtain write permission on a
 *      mapped entry, the following flag can be used. The entry will be
 *      made "needs copy" effectively copying the object (using COW),
 *      and write permission will be added to the maximum protections
 *      for the associated entry.
 */

#define VM_PROT_COPY		((vm_prot_t) 0x10)

/*
 *	Another invalid protection value.
 *	Used only by memory_object_lock_request to indicate no change
 *	to page locks.  Using -1 here is a bad idea because it
 *	looks like VM_PROT_ALL and then some.
 */

#define VM_PROT_NO_CHANGE_LEGACY  ((vm_prot_t) 0x80)

/*
 *      When a caller finds that he cannot obtain write permission on a
 *      mapped entry, the following flag can be used.  The entry will be
 *      made "needs copy" effectively copying the object (using COW),
 *      and write permission will be added to the maximum protections
 *      for the associated entry.
 */

#define VM_PROT_WANTS_COPY	((vm_prot_t) 0x10)

#endif	/* _MACH_VM_PROT_H_ */
