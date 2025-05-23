#ifndef LITES_AUTH_H
#define LITES_AUTH_H

#include <stdint.h>
#include "../src-lites-1.1-2025/include/cap.h"

typedef struct cap cap_t;

typedef struct {
    const cap_t *subject;
    uint32_t op;
    uint64_t obj;
} acl_entry_t;

void acl_add(const cap_t *subject, uint32_t op, uint64_t obj);
int authorize(const cap_t *subject, uint32_t op, uint64_t obj);

#define CAP_OP_REFINE 1
#define CAP_OP_REVOKE 2

#endif /* LITES_AUTH_H */
