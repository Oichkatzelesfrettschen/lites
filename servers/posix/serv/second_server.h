/*
 * Configuration header for SECOND_SERVER feature
 *
 * SECOND_SERVER controls the secondary BSD server support
 * for running multiple BSD personalities
 */

#ifndef _SERV_SECOND_SERVER_H_
#define _SERV_SECOND_SERVER_H_

/*
 * SECOND_SERVER - Enable secondary server support
 *
 * Set to 0 for single server mode (default)
 * Set to 1 to enable multiple server support
 */
#ifndef SECOND_SERVER
#define SECOND_SERVER	0
#endif

#endif /* _SERV_SECOND_SERVER_H_ */
