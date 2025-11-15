/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 * File:	mach.h
 *
 * Master include file for Mach user-level programs.
 * Includes all standard Mach headers.
 */

#ifndef	_MACH_H_
#define	_MACH_H_

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/mach_types.h>
#include <mach/port.h>
#include <mach/vm_prot.h>
#include <mach/vm_inherit.h>
#include <mach/vm_statistics.h>
#include <mach/task_info.h>
#include <mach/thread_info.h>
#include <mach/thread_status.h>
#include <mach/thread_special_ports.h>
#include <mach/task_special_ports.h>
#include <mach/host_info.h>
#include <mach/processor_info.h>
#include <mach/memory_object.h>

/* MIG-generated interfaces */
#include <mach_interface.h>
#include <mach_port.h>
#include <mach_host.h>

/* Standard C types */
#include <sys/types.h>

#endif	/* _MACH_H_ */
