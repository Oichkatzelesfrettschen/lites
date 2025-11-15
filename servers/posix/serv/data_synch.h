/*
 * Configuration header for DATA_SYNCH feature
 *
 * DATA_SYNCH controls advanced data synchronization mechanisms
 * for process wakeup and signal delivery in a multi-threaded
 * server environment.
 *
 * When enabled (1): Use advanced synchronization for process wakeup
 * When disabled (0): Traditional wakeup mechanism
 */

#ifndef _SERV_DATA_SYNCH_H_
#define _SERV_DATA_SYNCH_H_

/*
 * DATA_SYNCH - Enable advanced data synchronization
 *
 * Set to 0 for traditional synchronization (default)
 * Set to 1 to enable advanced data synchronization features
 */
#ifndef DATA_SYNCH
#define DATA_SYNCH	0
#endif

#endif /* _SERV_DATA_SYNCH_H_ */
