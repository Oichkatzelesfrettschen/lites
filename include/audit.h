#ifndef LITES_AUDIT_H
#define LITES_AUDIT_H

#include <stdint.h>

typedef struct {
    uint32_t op;
    uint64_t obj;
    int result;
} audit_entry_t;

#define AUDIT_LOG_SIZE 32

extern audit_entry_t audit_log[AUDIT_LOG_SIZE];

void audit_record(uint32_t op, uint64_t obj, int result);

#endif /* LITES_AUDIT_H */
