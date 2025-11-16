/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 */
/*
 * Device interface definitions for Mach kernel
 */

#ifndef _DEVICE_DEVICE_H_
#define _DEVICE_DEVICE_H_

#include <mach/mach_types.h>
#include <mach/port.h>

/*
 * Device operations
 */
typedef struct device *device_t;

#define DEVICE_NULL ((device_t) 0)

/* Forward declarations for MIG-generated interfaces */
typedef char *dev_mode_t;
typedef char *dev_name_t;
typedef char *dev_status_t;
typedef char *dev_flavor_t;

#define DEV_GET_SIZE 1
#define DEV_GET_RECORDS 2

/* Device structure (kernel internal) */
struct device {
	struct device_ops *dev_ops;	/* device operations */
	char *dev_name;			/* device name */
	int dev_number;			/* device number */
	int ref_count;			/* reference count */
	void *dev_info;			/* device-specific data */
};

/* Device operations vector */
struct device_ops {
	int (*d_open)(dev_t dev, int flag, int devtype, struct proc *p);
	int (*d_close)(dev_t dev, int flag, int devtype, struct proc *p);
	int (*d_read)(dev_t dev, struct uio *uio, int flag);
	int (*d_write)(dev_t dev, struct uio *uio, int flag);
	int (*d_ioctl)(dev_t dev, u_long cmd, caddr_t data, int flag, struct proc *p);
	int (*d_mmap)(dev_t dev, vm_offset_t offset, int prot);
};

#endif /* _DEVICE_DEVICE_H_ */
