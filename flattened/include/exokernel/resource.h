#ifndef EXOKERNEL_RESOURCE_H
#define EXOKERNEL_RESOURCE_H

#include <stddef.h>

typedef struct resource {
    void *start;
    size_t size;
    int type;
} resource_t;

void *resource_alloc(size_t size);
void resource_free(void *ptr);

#endif /* EXOKERNEL_RESOURCE_H */
