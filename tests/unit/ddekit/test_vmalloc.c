#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ddekit/memory.h>
#include <ddekit/pgtab.h>

// Mock implementations for DDEKit functions

// 1. Mock physical page allocator
#define NUM_PHYS_PAGES 1024
static char phys_mem_pool[NUM_PHYS_PAGES][4096];
static int phys_page_used[NUM_PHYS_PAGES] = {0};

ddekit_addr_t ddekit_phys_pgalloc(void) {
    for (int i = 0; i < NUM_PHYS_PAGES; i++) {
        if (!phys_page_used[i]) {
            phys_page_used[i] = 1;
            return (ddekit_addr_t)&phys_mem_pool[i];
        }
    }
    return 0; // Out of memory
}

void ddekit_phys_pgfree(ddekit_addr_t pa) {
    for (int i = 0; i < NUM_PHYS_PAGES; i++) {
        if ((ddekit_addr_t)&phys_mem_pool[i] == pa) {
            phys_page_used[i] = 0;
            return;
        }
    }
}

// 2. Mock page table functions
void ddekit_pgtab_set_region(void *virt, ddekit_addr_t phys, int pages, int type) {
    (void)virt;
    (void)phys;
    (void)pages;
    (void)type;
    // In a real test, we might want to record these calls and verify them.
    // For now, we do nothing.
}

void ddekit_pgtab_clear_region(void *virt, int type) {
    (void)virt;
    (void)type;
    // Do nothing.
}

// 3. Mock simple allocator
void *ddekit_simple_malloc(unsigned size) {
    return malloc(size);
}

void ddekit_simple_free(void *p) {
    free(p);
}


// Test cases
void test_alloc_and_free_single() {
    printf("Running test: test_alloc_and_free_single\n");
    void *mem = ddekit_vmalloc(100);
    assert(mem != NULL);
    // You could try to write to mem here, but since the pgtab functions are
    // mocks, it wouldn't be a real test of the mapping.
    ddekit_vfree(mem);
    printf("Test passed.\n");
}

void test_alloc_zero_bytes() {
    printf("Running test: test_alloc_zero_bytes\n");
    void *mem = ddekit_vmalloc(0);
    assert(mem == NULL);
    printf("Test passed.\n");
}

void test_free_null() {
    printf("Running test: test_free_null\n");
    ddekit_vfree(NULL);
    // Should not crash
    printf("Test passed.\n");
}

void test_multiple_allocs() {
    printf("Running test: test_multiple_allocs\n");
    void *mem1 = ddekit_vmalloc(100);
    void *mem2 = ddekit_vmalloc(200);
    assert(mem1 != NULL);
    assert(mem2 != NULL);
    assert(mem1 != mem2);
    ddekit_vfree(mem1);
    ddekit_vfree(mem2);
    printf("Test passed.\n");
}

int main(void) {
    test_alloc_and_free_single();
    test_alloc_zero_bytes();
    test_free_null();
    test_multiple_allocs();

    printf("All tests passed!\n");
    return 0;
}
