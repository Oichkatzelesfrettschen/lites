/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
 * All Rights Reserved.
 */

#ifndef	_MACH_EXCEPTION_H_
#define	_MACH_EXCEPTION_H_

/*
 * This header includes the machine-dependent exception definitions.
 */

#include <mach/machine/exception.h>

/*
 * Exception behaviors
 */
typedef int exception_behavior_t;

#define	EXCEPTION_DEFAULT	1
#define EXCEPTION_STATE		2
#define EXCEPTION_STATE_IDENTITY 3

#endif	/* _MACH_EXCEPTION_H_ */
