#pragma once

#include <stdint.h>
#include <sys/types.h>

/* Minimal Mach compatible types used by the modernised VM layer. */
typedef int            kern_return_t;
typedef uintptr_t      vm_offset_t;
typedef int            vm_prot_t;
typedef int            mach_port_t;

typedef struct { int dummy; } mutex_t;

#ifndef KERN_SUCCESS
#define KERN_SUCCESS 0
#endif

#ifndef KERN_FAILURE
#define KERN_FAILURE 1
#endif

/* Protection bits */
#ifndef VM_PROT_READ
#define VM_PROT_READ     0x1
#define VM_PROT_WRITE    0x2
#define VM_PROT_EXECUTE  0x4
#endif

/* Address space descriptor used by the modernised VM layer. */
typedef struct aspace {
    vm_offset_t pml_root;  /* page map level root */
    mach_port_t pager_cap; /* capability for associated pager */
    mutex_t vm_lock;       /* protects mappings */
} aspace_t;

/* Information passed to the user level pager when a page fault occurs. */
typedef struct pf_info {
    vm_offset_t addr;  /* faulting address */
    vm_prot_t   prot;  /* access that caused the fault */
    int         flags; /* future use */
} pf_info_t;

/* Shareable process virtual address space (BSD compatibility). */
struct vmspace {
    int     vm_refcnt;      /* number of references */
    void    *vm_shm;        /* SYS5 shared memory private data XXX */
/* we copy from vm_startcopy to the end of the structure on fork */
#define vm_startcopy vm_rssize
    segsz_t vm_rssize;      /* current resident set size in pages */
    segsz_t vm_swrss;       /* resident set size before last swap */
    segsz_t vm_tsize;       /* text size (pages) XXX */
    segsz_t vm_dsize;       /* data size (pages) XXX */
    segsz_t vm_ssize;       /* stack size (pages) */
    caddr_t vm_taddr;       /* user virtual address of text XXX */
    caddr_t vm_daddr;       /* user virtual address of data XXX */
    caddr_t vm_maxsaddr;    /* user VA at max stack growth */
};
