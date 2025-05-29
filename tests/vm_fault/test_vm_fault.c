#include "posix.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../libs/libos/vm.h"

extern kern_return_t vm_fault_entry(aspace_t *, vm_offset_t, vm_prot_t);
extern kern_return_t unmap_frame(aspace_t *, vm_offset_t);

int main(void) {
    int sv[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);

    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        close(sv[0]);
        char fdstr[16];
        snprintf(fdstr, sizeof(fdstr), "%d", sv[1]);
        const char *src = getenv("LITES_SRC_DIR");
        char path[256];
        if (src) {
            snprintf(path, sizeof(path), "%s/bin/user_pager/user_pager", src);
        } else {
            snprintf(path, sizeof(path), "bin/user_pager/user_pager");
        }
        execl(path, "user_pager", fdstr, NULL);
        perror("execl");
        _exit(1);
    }

    close(sv[1]);
    aspace_t as = {.pml_root = 0, .pager_cap = sv[0]};

    vm_offset_t addr = 0x1000000;
    assert(vm_fault_entry(&as, addr, VM_PROT_READ | VM_PROT_WRITE) == KERN_SUCCESS);

    int *p = (int *)addr;
    *p = 42;
    assert(*p == 42);

    assert(unmap_frame(&as, addr) == KERN_SUCCESS);

    close(sv[0]);
    posix_waitpid(pid, NULL, 0);
    return 0;
}
