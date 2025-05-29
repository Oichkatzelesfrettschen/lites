#include <assert.h>
#include <stdlib.h>
#include "../../include/auth.h"
#include "../../include/audit.h"
#include "../../include/id128.h"
#include "posix_ipc.h"

int main(void)
{
    struct cap subject = {0};
    subject.rights = 0xff;
    subject.epoch = 1;

    acl_add(&subject, 1, id128_from_u64(42)); /* allow op 1 on obj 42 */

    /* This should fail and be logged */
    int ok = authorize(&subject, 2, id128_from_u64(43));
    assert(!ok);
    assert(audit_log[0].op == 2);
    assert(audit_log[0].result == 0);

    return 0;
}
