/**
 * Lites repository license applies to this file; see the LICENSE file
 * in the project root for details.
 */

#include "posix_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
    size_t psz = (size_t)sysconf(_SC_PAGESIZE);
    void *p = mmap(NULL, psz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    lites_track_region(p, psz, PROT_READ | PROT_WRITE);
    printf("initial prot: %d\n", lites_get_prot(p, psz));

    if (lites_mprotect(p, psz, PROT_READ) != 0) {
        perror("mprotect");
        return 1;
    }
    printf("after mprotect: %d\n", lites_get_prot(p, psz));

    if (lites_msync(p, psz) != 0) {
        perror("msync");
        return 1;
    }

    lites_untrack_region(p, psz);
    munmap(p, psz);
    return 0;
}
