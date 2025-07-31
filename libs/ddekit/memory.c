#include <ddekit/memory.h>
#include <ddekit/pgtab.h>
#include <ddekit/ddekit.h>
#include <stddef.h>

struct vmalloc_region {
    void *vaddr;
    unsigned long size;
    unsigned long num_pages;
    ddekit_addr_t *paddrs;
    struct vmalloc_region *next;
};

static struct vmalloc_region *vmalloc_list_head = NULL;

// Placeholder for the physical page allocator
ddekit_addr_t ddekit_phys_pgalloc(void) {
    // This should be implemented by the underlying system.
    return 0;
}

void ddekit_phys_pgfree(ddekit_addr_t pa) {
    // This should be implemented by the underlying system.
}

#define PAGE_SIZE 4096

// A simple bump-pointer allocator for the vmalloc region.
// This is not a robust implementation, but it's a start.
// TODO: Replace with a proper virtual memory manager.
static unsigned long vmalloc_start = 0x0000100000000000;
static unsigned long next_vaddr = 0x0000100000000000;

void *ddekit_vmalloc(unsigned long size) {
    if (size == 0) {
        return NULL;
    }

    unsigned long aligned_size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    unsigned long num_pages = aligned_size / PAGE_SIZE;

    // Get a contiguous virtual address range
    void *vaddr = (void *)next_vaddr;
    next_vaddr += aligned_size;

    // TODO: Check for out of memory condition for the vmalloc region.

    // Allocate metadata structures
    struct vmalloc_region *region = ddekit_simple_malloc(sizeof(struct vmalloc_region));
    if (!region) {
        return NULL;
    }
    region->paddrs = ddekit_simple_malloc(sizeof(ddekit_addr_t) * num_pages);
    if (!region->paddrs) {
        ddekit_simple_free(region);
        return NULL;
    }

    // Allocate physical pages and map them
    for (unsigned long i = 0; i < num_pages; i++) {
        region->paddrs[i] = ddekit_phys_pgalloc();
        if (region->paddrs[i] == 0) {
            // Out of physical memory. Free what we've allocated so far.
            for (unsigned long j = 0; j < i; j++) {
                ddekit_phys_pgfree(region->paddrs[j]);
            }
            ddekit_simple_free(region->paddrs);
            ddekit_simple_free(region);
            return NULL;
        }
        void *current_vaddr = (void *)((unsigned long)vaddr + i * PAGE_SIZE);
        ddekit_pgtab_set_region(current_vaddr, region->paddrs[i], 1, PTE_TYPE_OTHER);
    }

    // Store metadata
    region->vaddr = vaddr;
    region->size = size;
    region->num_pages = num_pages;
    region->next = vmalloc_list_head;
    vmalloc_list_head = region;

    return vaddr;
}

void ddekit_vfree(void *addr) {
    if (addr == NULL) {
        return;
    }

    struct vmalloc_region **p = &vmalloc_list_head;
    struct vmalloc_region *current = *p;

    while (current) {
        if (current->vaddr == addr) {
            // Found the region. Unmap and free everything.
            for (unsigned long i = 0; i < current->num_pages; i++) {
                void *current_vaddr = (void *)((unsigned long)addr + i * PAGE_SIZE);
                ddekit_pgtab_clear_region(current_vaddr, PTE_TYPE_OTHER);
                ddekit_phys_pgfree(current->paddrs[i]);
            }

            // Remove from list
            *p = current->next;

            // Free metadata
            ddekit_simple_free(current->paddrs);
            ddekit_simple_free(current);

            // TODO: The virtual address space is not reused.
            return;
        }
        p = &current->next;
        current = *p;
    }
}

// Placeholders for existing DDEKit functions
// These are just to make the library build. The real implementation
// is somewhere else.
struct ddekit_slab * ddekit_slab_init(unsigned size, int contiguous) { return NULL; }
void ddekit_slab_destroy(struct ddekit_slab * slab) {}
void *ddekit_slab_alloc(struct ddekit_slab * slab) { return NULL; }
void ddekit_slab_free(struct ddekit_slab * slab, void *objp) {}
void ddekit_slab_set_data(struct ddekit_slab * slab, void *data) {}
void *ddekit_slab_get_data(struct ddekit_slab * slab) { return NULL; }
void ddekit_slab_setup_page_cache(unsigned pages) {}
void *ddekit_large_malloc(int size) { return NULL; }
void ddekit_large_free(void *p) {}
void *ddekit_contig_malloc(unsigned long size, unsigned long low,
        unsigned long high, unsigned long alignment, unsigned long boundary) { return NULL; }
void *ddekit_simple_malloc(unsigned size) { return NULL; }
void ddekit_simple_free(void *p) {}
