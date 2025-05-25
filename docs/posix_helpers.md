# POSIX helpers

`src-lites-1.1-2025/liblites/posix.c` implements a tiny tracking layer for page
protections.  It exposes wrappers around `mprotect(2)` and `msync(2)` that keep
an internal record of the current protection flags for each mapped region.

```c
void lites_track_region(void *addr, size_t len, int prot);
void lites_untrack_region(void *addr, size_t len);
int  lites_get_prot(void *addr, size_t len);
int  lites_mprotect(void *addr, size_t len, int prot);
int  lites_msync(void *addr, size_t len);
```

`lites_track_region` should be called after mapping a new region so that future
protection changes can be queried via `lites_get_prot`.  The wrappers update the
stored state on success.  `lites_msync` simply invokes `msync(MS_SYNC)` for
convenience.

## Example program

`bin/posix-demo.c` demonstrates the helpers by mapping a single page,
changing its protection and syncing it back to memory.
