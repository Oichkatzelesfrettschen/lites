#include "auth.h"
#include "audit.h"

#define MAX_ACL 64

static acl_entry_t acl_table[MAX_ACL];
static int acl_count;

void acl_add(const cap_t *subject, uint32_t op, id128_t obj)
{
    if (acl_count < MAX_ACL) {
        acl_table[acl_count].subject = subject;
        acl_table[acl_count].op = op;
        acl_table[acl_count].obj = obj;
        acl_count++;
    }
}

int authorize(const cap_t *subject, uint32_t op, id128_t obj)
{
    for (int i = 0; i < acl_count; ++i) {
        if (acl_table[i].subject == subject &&
            acl_table[i].op == op &&
            acl_table[i].obj == obj) {
            audit_record(op, obj, 1);
            return 1;
        }
    }
    audit_record(op, obj, 0);
    return 0;
}
