/*
 * Configuration header for PTY feature
 *
 * PTY controls pseudo-terminal support
 */

#ifndef _SERV_PTY_H_
#define _SERV_PTY_H_

/*
 * NPTY - Number of pseudo-terminal pairs
 *
 * Set to 0 to disable PTY support
 * Set to N for N pseudo-terminal pairs
 */
#ifndef NPTY
#define NPTY	16
#endif

#endif /* _SERV_PTY_H_ */
