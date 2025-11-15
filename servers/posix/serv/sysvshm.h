/*
 * Configuration header for SYSVSHM feature
 *
 * SYSVSHM controls System V shared memory support
 */

#ifndef _SERV_SYSVSHM_H_
#define _SERV_SYSVSHM_H_

/*
 * SYSVSHM - Enable System V shared memory
 *
 * Set to 0 to disable System V shared memory (default)
 * Set to 1 to enable System V shared memory support
 */
#ifndef SYSVSHM
#define SYSVSHM	0
#endif

#endif /* _SERV_SYSVSHM_H_ */
