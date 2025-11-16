/*
 * Top-level errno.h header
 * Includes the system error number definitions
 */

#ifndef _ERRNO_H_
#define _ERRNO_H_

#include <sys/errno.h>

#ifndef KERNEL
extern int errno;
#endif

#endif /* _ERRNO_H_ */
