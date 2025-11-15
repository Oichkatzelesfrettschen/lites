/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_VM_INHERIT_H_
#define	_MACH_VM_INHERIT_H_

/*
 *	Virtual memory map inheritance definitions.
 */

typedef unsigned int	vm_inherit_t;	/* might want to change this */

/*
 *	Enumeration of valid values for vm_inherit_t.
 */

#define	VM_INHERIT_SHARE	((vm_inherit_t) 0)	/* share with child */
#define	VM_INHERIT_COPY		((vm_inherit_t) 1)	/* copy into child */
#define VM_INHERIT_NONE		((vm_inherit_t) 2)	/* absent from child */
#define	VM_INHERIT_DONATE_COPY	((vm_inherit_t) 3)	/* copy and delete */

#define VM_INHERIT_DEFAULT	VM_INHERIT_COPY
#define VM_INHERIT_LAST_VALID	VM_INHERIT_NONE

#endif	/* _MACH_VM_INHERIT_H_ */
