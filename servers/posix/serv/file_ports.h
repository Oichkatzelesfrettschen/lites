/*
 * Configuration header for FILE_PORTS feature
 *
 * FILE_PORTS controls whether file descriptors are implemented
 * using Mach ports for IPC-based file operations.
 *
 * When enabled (1): File descriptors backed by Mach ports
 * When disabled (0): Traditional file descriptor implementation
 */

#ifndef _SERV_FILE_PORTS_H_
#define _SERV_FILE_PORTS_H_

/*
 * FILE_PORTS - Enable Mach port-based file descriptors
 *
 * Set to 0 for traditional file descriptors (default)
 * Set to 1 to enable port-based file descriptors
 */
#ifndef FILE_PORTS
#define FILE_PORTS	0
#endif

#endif /* _SERV_FILE_PORTS_H_ */
