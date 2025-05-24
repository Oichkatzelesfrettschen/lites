#include <sys/param.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/audit.h>

#define AUDIT_LOG_SIZE 64

struct audit_entry {
        int pid;
        const char *op;
        int result;
};

static struct audit_entry audit_log[AUDIT_LOG_SIZE];
static int audit_pos;

void
audit_record(struct proc *p, const char *op, int result)
{
        audit_log[audit_pos].pid = p ? p->p_pid : -1;
        audit_log[audit_pos].op = op;
        audit_log[audit_pos].result = result;
        audit_pos = (audit_pos + 1) % AUDIT_LOG_SIZE;
}

