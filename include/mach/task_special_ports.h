/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_TASK_SPECIAL_PORTS_H_
#define	_MACH_TASK_SPECIAL_PORTS_H_

/*
 *	Defines codes for access to "special" ports owned by the task.
 */

#define TASK_KERNEL_PORT	1	/* Represents the task to the outside
					   world. */

#define TASK_BOOTSTRAP_PORT	4	/* Bootstrap environment for task. */

#define task_get_bootstrap_port(task, port)	\
	(task_get_special_port((task), TASK_BOOTSTRAP_PORT, (port)))

#define task_set_bootstrap_port(task, port)	\
	(task_set_special_port((task), TASK_BOOTSTRAP_PORT, (port)))

#endif	/* _MACH_TASK_SPECIAL_PORTS_H_ */
