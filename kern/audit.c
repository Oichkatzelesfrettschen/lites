#include "audit.h"

audit_entry_t audit_log[AUDIT_LOG_SIZE];
static int audit_pos;

void audit_record(uint32_t op, id128_t obj, int result)
{
    audit_log[audit_pos].op = op;
    audit_log[audit_pos].obj = obj;
    audit_log[audit_pos].result = result;
    audit_pos = (audit_pos + 1) % AUDIT_LOG_SIZE;
}
