#include <cap.h>
#include <stdlib.h>
#include "auth.h"


/*
 * Recursively verify invariants for capability @c and its subtree.  Children
 * must reference @c as their parent, may only have a subset of @c's rights and
 * must share the same epoch value.
 */
static int cap_check_node(const struct cap *c) {
    const struct cap *child;
    for (child = c->children; child; child = child->next_sibling) {
        if (child->parent != c)
            return 0;
        if ((child->rights & c->rights) != child->rights)
            return 0;
        if (child->epoch != c->epoch)
            return 0;
        if (!cap_check_node(child))
            return 0;
    }
    return 1;
}

/* Public wrapper that validates a capability tree starting at @cap. */
int cap_check(const struct cap *cap) {
    if (!cap)
        return 0;
    return cap_check_node(cap);
}

/*
 * Derive a new capability from @parent with a restricted rights mask.
 * Returns the newly allocated capability or NULL on error.
 */
struct cap *cap_refine(struct cap *parent, unsigned long rights, unsigned int flags) {
    struct cap *c;

    if (!parent)
        return NULL;
    if ((rights & parent->rights) != rights)
        return NULL;

    if (!authorize(parent, CAP_OP_REFINE, rights))
        return NULL;

    c = malloc(sizeof(*c));
    if (!c)
        return NULL;

    c->parent = parent;
    c->children = NULL;
    c->next_sibling = parent->children;
    parent->children = c;

    c->epoch = parent->epoch;
    c->rights = rights;
    c->flags = flags;

    return c;
}

/* Recursively free a capability subtree. */
void delete_subtree(struct cap *cap) {
    struct cap *child = cap->children;
    while (child) {
        struct cap *next = child->next_sibling;
        delete_subtree(child);
        child = next;
    }
    free(cap);
}

/*
 * Revoke a capability subtree by incrementing its epoch counter.  All
 * descendants receive the new epoch value which invalidates references
 * obtained prior to revocation.
 */
void revoke_capability(struct cap *cap) {
    struct cap *child;
    if (!authorize(cap, CAP_OP_REVOKE, 0))
        return;
    cap->epoch++;
    for (child = cap->children; child; child = child->next_sibling)
        revoke_capability(child);
}
