#pragma once

/*
 * Basic capability descriptor.  Capabilities form a tree where each child
 * may only reduce the rights of its parent.  Revocation is implemented by
 * bumping the epoch counter of a subtree which invalidates all descendants.
 */
struct cap {
    /* parent in the capability tree (NULL for the root) */
    struct cap *parent;
    /* singly linked list of children */
    struct cap *children;
    /* link to the next sibling in the parent's children list */
    struct cap *next_sibling;

    /* generation counter used for revocation */
    unsigned long epoch;
    /* rights bitmask that may only decrease down the tree */
    unsigned long rights;
    /* user defined flags */
    unsigned int flags;
};

/* Rights bits for generic file objects */
#define CAP_RIGHT_READ 0x1
#define CAP_RIGHT_WRITE 0x2
#define CAP_RIGHT_EXEC 0x4

struct cap *cap_refine(struct cap *parent, unsigned long rights, unsigned int flags);
void revoke_capability(struct cap *cap);
int cap_check(const struct cap *cap);
