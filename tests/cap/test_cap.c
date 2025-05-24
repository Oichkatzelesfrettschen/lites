#include "../../src-lites-1.1-2025/include/cap.h"
#include <assert.h>
#include <stdlib.h>

static void test_refine_basic(void) {
    struct cap root = {0};
    root.rights = 0xff;
    root.epoch = 1;

    struct cap *child = cap_refine(&root, 0x0f, 0);
    assert(child);
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

    struct cap *child = cap_refine(&root, 0x0f, 0);
    assert(child);
    struct cap *grand = cap_refine(child, 0x01, 0);
    assert(grand);

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
