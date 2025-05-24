#ifndef LITES_VM_H
#define LITES_VM_H

#include <serv/import_mach.h>

/*
 * Address space descriptor used by the modernised VM layer.
 */
typedef struct aspace {
    vm_offset_t pml_root;  /* page map level root */
    mach_port_t pager_cap; /* capability for associated pager */
    mutex_t vm_lock;       /* protects mappings */
} aspace_t;

#endif /* LITES_VM_H */
