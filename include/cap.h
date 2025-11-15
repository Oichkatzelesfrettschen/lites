/*
 * Capability Security Header
 * Minimal stub for Lites 1.1.u3
 *
 * Provides capability-based security primitives
 */

#ifndef _CAP_H_
#define _CAP_H_

/* Capability structure forward declaration */
struct capability;
typedef struct capability *cap_t;

/* Capability structure for capability-based security */
struct cap {
    struct cap *parent;       /* Parent capability */
    struct cap *children;     /* First child in linked list */
    struct cap *next_sibling; /* Next sibling in parent's child list */
    unsigned long rights;     /* Rights bitmask */
    unsigned int flags;       /* Capability flags */
    unsigned int epoch;       /* Revocation epoch counter */
};

/* Basic capability operations (stubs for now) */
#define CAP_NONE    0x00
#define CAP_READ    0x01
#define CAP_WRITE   0x02
#define CAP_EXECUTE 0x04

/* Capability operations for authorization */
#define CAP_OP_REFINE   1     /* Refine capability to narrower rights */
#define CAP_OP_REVOKE   2     /* Revoke capability subtree */

#endif /* _CAP_H_ */
