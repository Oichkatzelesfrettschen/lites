#include <assert.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../src-lites-1.1-2025/include/vm.h"
#include "../../src-lites-1.1-2025/include/posix_helpers.h"

extern kern_return_t vm_fault_entry(aspace_t *, vm_offset_t, vm_prot_t);
extern kern_return_t unmap_frame(aspace_t *, vm_offset_t);

int main(void)
{
    int sv[2];
    assert(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == 0);

    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        close(sv[0]);
        char fdstr[16];
        snprintf(fdstr, sizeof(fdstr), "%d", sv[1]);
        execl("./src-lites-1.1-2025/bin/user_pager/user_pager", "user_pager", fdstr, NULL);
        perror("execl");
        _exit(1);
    }

    close(sv[1]);
    aspace_t as = { .pml_root = 0, .pager_cap = sv[0] };

    vm_offset_t addr = 0x1000000;
    size_t psz = (size_t)sysconf(_SC_PAGESIZE);
    assert(vm_fault_entry(&as, addr, VM_PROT_READ | VM_PROT_WRITE) == KERN_SUCCESS);
    lites_track_region((void *)addr, psz, PROT_READ | PROT_WRITE);

    int *p = (int *)addr;
    *p = 42;
    assert(*p == 42);

    assert(lites_get_prot((void *)addr, psz) == (PROT_READ | PROT_WRITE));
    assert(lites_mprotect((void *)addr, psz, PROT_READ) == 0);
    assert(lites_get_prot((void *)addr, psz) == PROT_READ);
    assert(lites_msync((void *)addr, psz) == 0);

    lites_untrack_region((void *)addr, psz);
    assert(unmap_frame(&as, addr) == KERN_SUCCESS);

    close(sv[0]);
    waitpid(pid, NULL, 0);
    return 0;
}

