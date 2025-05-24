#ifndef _SYS_AUDIT_H_
#define _SYS_AUDIT_H_

#include <sys/proc.h>

void audit_record(struct proc *p, const char *op, int result);

#endif /* _SYS_AUDIT_H_ */

