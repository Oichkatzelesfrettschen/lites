#include <stdlib.h>
#include "../../include/cap.h"

static int cap_check_node(const struct cap *c)
{
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

int cap_check(const struct cap *cap)
{
    if (!cap)
        return 0;
    return cap_check_node(cap);
}

struct cap *
cap_refine(struct cap *parent, unsigned long rights, unsigned int flags)
{
    struct cap *c;

    if (!parent)
        return NULL;
    if ((rights & parent->rights) != rights)
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

void
delete_subtree(struct cap *cap)
{
    struct cap *child = cap->children;
    while (child) {
        struct cap *next = child->next_sibling;
        delete_subtree(child);
        child = next;
    }
    free(cap);
}

void
revoke_capability(struct cap *cap)
{
    struct cap *child;
    cap->epoch++;
    for (child = cap->children; child; child = child->next_sibling)
        revoke_capability(child);
}
