/*
 * Configuration header for MACHID_REGISTER feature
 *
 * MACHID_REGISTER controls Mach ID registration support
 * for debugging and monitoring
 */

#ifndef _SERV_MACHID_REGISTER_H_
#define _SERV_MACHID_REGISTER_H_

/*
 * MACHID_REGISTER - Enable Mach ID registration
 *
 * Set to 0 to disable Mach ID registration (default)
 * Set to 1 to enable Mach ID registration for debugging
 */
#ifndef MACHID_REGISTER
#define MACHID_REGISTER	0
#endif

#endif /* _SERV_MACHID_REGISTER_H_ */
