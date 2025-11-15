/*
 * OSF Mach 3.0 Configuration Header
 * For Lites 1.1.u3 build
 *
 * This header provides OSF Mach 3.0 compatibility definitions.
 * OSF/1 was the Open Software Foundation's Unix implementation
 * that ran on Mach 3.0 microkernel.
 */

#ifndef _OSFMACH3_H_
#define _OSFMACH3_H_

/*
 * Enable OSF Mach 3.0 compatibility mode.
 * Lites is designed to run on CMU Mach 3.0, which is compatible
 * with OSF Mach 3.0 in most respects.
 */
#define OSFMACH3 1

/*
 * Common OSF Mach 3.0 configuration options
 */
#define MACH_IPC_COMPAT 1       /* Enable Mach IPC compatibility */
#define MACH_LDEBUG 0           /* Mach lock debugging (disabled for production) */

#endif /* _OSFMACH3_H_ */
