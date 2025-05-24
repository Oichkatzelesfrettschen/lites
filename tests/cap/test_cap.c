#include <assert.h>
#include <stdlib.h>
#include "../../src-lites-1.1-2025/include/cap.h"

int main(void)
{
    struct cap root = {0};
    root.rights = 0xff;
    root.epoch = 1;

    struct cap *child = cap_refine(&root, 0x0f, 0);
    assert(child);
    assert(child->parent == &root);
    assert(child->rights == 0x0f);
    assert(cap_check(&root));

    revoke_capability(&root);
    assert(root.epoch == 2);
    assert(child->epoch == 2);
    assert(cap_check(&root));

    free(child);
    return 0;
}
