#pragma once

#include <sys/types.h>
#include <sys/mman.h>

/* Register a memory region with the initial protection flags. */
void lites_track_region(void *addr, size_t len, int prot);

/* Remove a previously tracked region. */
void lites_untrack_region(void *addr, size_t len);

/* Return the current protection flags for a tracked region or -1 if unknown. */
int lites_get_prot(void *addr, size_t len);

/* Wrapper around mprotect that keeps internal state in sync. */
int lites_mprotect(void *addr, size_t len, int prot);

/* Convenience wrapper for msync using MS_SYNC. */
int lites_msync(void *addr, size_t len);

