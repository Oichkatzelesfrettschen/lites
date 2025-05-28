#include <sys/param.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/auth.h>
#include <string.h>

#define ACL_MAX 16

static struct acl_entry acl_table[ACL_MAX] = {
        {0, "sosend", 1},
        {0, "soreceive", 1},
        {0, "capability", 1},
};
static int acl_count = 3;

void
acl_add(uid_t uid, const char *op, int allow)
{
        if (acl_count >= ACL_MAX)
                return;
        acl_table[acl_count].ae_uid = uid;
        acl_table[acl_count].ae_op = op;
        acl_table[acl_count].ae_allow = allow;
        acl_count++;
}

int
authorize(struct proc *p, const char *op)
{
        uid_t uid = p ? p->p_ucred->cr_uid : 0;
        int i;

        for (i = 0; i < acl_count; i++) {
                if (acl_table[i].ae_uid == uid &&
                    strcmp(acl_table[i].ae_op, op) == 0)
                        return acl_table[i].ae_allow;
        }
        return 1;       /* default allow */
}

