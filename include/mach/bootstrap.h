/*
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 */
/*
 * Bootstrap server interface
 */

#ifndef _MACH_BOOTSTRAP_H_
#define _MACH_BOOTSTRAP_H_

#include <mach/mach.h>

/* Bootstrap port */
extern mach_port_t bootstrap_port;

/* Bootstrap service name */
typedef char name_t[128];
typedef char cmd_t[512];

#define BOOTSTRAP_MAX_NAME_LEN 128
#define BOOTSTRAP_MAX_CMD_LEN  512

/* Bootstrap server functions */
kern_return_t bootstrap_check_in(
	mach_port_t bootstrap_port,
	name_t service_name,
	mach_port_t *service_port);

kern_return_t bootstrap_look_up(
	mach_port_t bootstrap_port,
	name_t service_name,
	mach_port_t *service_port);

kern_return_t bootstrap_register(
	mach_port_t bootstrap_port,
	name_t service_name,
	mach_port_t service_port);

kern_return_t bootstrap_status(
	mach_port_t bootstrap_port,
	name_t service_name,
	int *service_active);

#endif /* _MACH_BOOTSTRAP_H_ */
