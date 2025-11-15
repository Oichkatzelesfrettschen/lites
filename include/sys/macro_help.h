/*
 * Mach Operating System
 * Copyright (c) 1994 Johannes Helander
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * JOHANNES HELANDER ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  JOHANNES HELANDER DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 */

#ifndef	_SYS_MACRO_HELP_H_
#define _SYS_MACRO_HELP_H_

/*
 * Macros to make C code look more like Pascal
 * Used for multi-statement macros that should behave as single statements
 */
#define		MACRO_BEGIN	do {
#define		MACRO_END	} while (0)

#endif	/* !_SYS_MACRO_HELP_H_ */
