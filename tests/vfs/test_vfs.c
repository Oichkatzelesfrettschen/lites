#include "../../include/auth.h"
#include "../../src-lites-1.1-2025/include/posix_fs.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    struct cap root = {0};
    root.rights = CAP_RIGHT_READ | CAP_RIGHT_WRITE;
    root.epoch = 1;

    acl_add(&root, CAP_OP_OPEN, CAP_RIGHT_READ | CAP_RIGHT_WRITE);
    acl_add(&root, CAP_OP_READ, 0);
    acl_add(&root, CAP_OP_WRITE, 0);
    acl_add(&root, CAP_OP_CLOSE, 0);

    const char *tmp = "vfs_test.tmp";
    FILE *f = fopen(tmp, "w");
    assert(f);
    fclose(f);

    file_handle_t *h = vfs_open(&root, tmp, O_RDWR);
    assert(h);
    assert(vfs_write(h, "hi", 2) == 2);
    assert(vfs_close(h) == 0);

    h = vfs_open(&root, tmp, O_RDONLY);
    assert(h);
    char buf[3];
    assert(vfs_read(h, buf, 2) == 2);
    buf[2] = '\0';
    assert(strcmp(buf, "hi") == 0);
    assert(vfs_close(h) == 0);

    struct cap read_only = {0};
    read_only.rights = CAP_RIGHT_READ;
    read_only.epoch = 1;
    acl_add(&read_only, CAP_OP_OPEN, CAP_RIGHT_READ);
    acl_add(&read_only, CAP_OP_READ, 0);
    acl_add(&read_only, CAP_OP_CLOSE, 0);

    assert(vfs_open(&read_only, tmp, O_WRONLY) == NULL);

    unlink(tmp);
    return 0;
}
