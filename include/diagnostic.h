/*
 * BSD Diagnostic Macros
 * Minimal stub for Lites 1.1.u3 build
 *
 * Provides KASSERT and DIAGNOSTIC mode checking macros
 * Based on BSD 4.4-Lite conventions
 */

#ifndef _DIAGNOSTIC_H_
#define _DIAGNOSTIC_H_

/* Diagnostic mode flag - set to 1 to enable extra checks */
#ifndef DIAGNOSTIC
#define DIAGNOSTIC 0
#endif

/* Kernel assertions - similar to assert() but for kernel code */
#ifdef DIAGNOSTIC
#define KASSERT(exp, msg) \
    do { \
        if (!(exp)) \
            panic msg; \
    } while (0)
#else
#define KASSERT(exp, msg) ((void)0)
#endif

/* Additional diagnostic macros can be added here as needed */

#endif /* _DIAGNOSTIC_H_ */
