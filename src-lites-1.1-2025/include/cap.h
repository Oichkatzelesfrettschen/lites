#ifndef _CAP_H_
#define _CAP_H_

struct cap {
    struct cap *parent;
    struct cap *children; /* linked list of children */
    struct cap *next_sibling;
    unsigned long epoch;
    unsigned long rights;
    unsigned int flags;
};

struct cap *cap_refine(struct cap *parent, unsigned long rights, unsigned int flags);
void revoke_capability(struct cap *cap);
int cap_check(const struct cap *cap);

#endif /* _CAP_H_ */
