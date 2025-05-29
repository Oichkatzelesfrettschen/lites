#include "posix_ipc.h"
#include <assert.h>
#include <stdlib.h>
#include "../../include/auth.h"
#include "../../include/id128.h"


static void test_refine_basic(void) {
    struct cap root = {0};
    root.rights = 0xff;
    root.epoch = 1;

    acl_add(&root, CAP_OP_REFINE, id128_from_u64(0x0f));
    acl_add(&root, CAP_OP_REVOKE, id128_from_u64(0));
    struct cap *child = cap_refine(&root, 0x0f, 0);
    assert(child);
    acl_add(child, CAP_OP_REVOKE, id128_from_u64(0));
    assert(child->parent == &root);
    assert(child->rights == 0x0f);
    assert(child->epoch == root.epoch);
    assert(cap_check(&root));

    free(child);
}

static void test_refine_invalid_rights(void) {
    struct cap root = {0};
    root.rights = 0x0f;
    root.epoch = 3;

    struct cap *child = cap_refine(&root, 0xf0, 0);
    assert(child == NULL);
    assert(root.children == NULL);
    assert(cap_check(&root));
}

static void test_revoke_epoch_propagation(void) {
    struct cap root = {0};
    root.rights = 0xff;
    root.epoch = 10;

    acl_add(&root, CAP_OP_REFINE, id128_from_u64(0x0f));
    acl_add(&root, CAP_OP_REVOKE, id128_from_u64(0));

    struct cap *child = cap_refine(&root, 0x0f, 0);
    assert(child);
    acl_add(child, CAP_OP_REFINE, id128_from_u64(0x01));
    acl_add(child, CAP_OP_REVOKE, id128_from_u64(0));
    struct cap *grand = cap_refine(child, 0x01, 0);
    assert(grand);
    acl_add(grand, CAP_OP_REVOKE, id128_from_u64(0));

    revoke_capability(&root);
    assert(root.epoch == 11);
    assert(child->epoch == 11);
    assert(grand->epoch == 11);
    assert(cap_check(&root));

    free(grand);
    free(child);
}

int main(void) {
    test_refine_basic();
    test_refine_invalid_rights();
    test_revoke_epoch_propagation();
    return 0;
}
