#ifndef MICROKERNEL_THREAD_H
#define MICROKERNEL_THREAD_H

#include <stddef.h>

struct mk_thread;

typedef void (*mk_thread_entry_t)(void *arg);

int mk_thread_create(struct mk_thread **out, mk_thread_entry_t entry, void *arg,
                     size_t stack_size);
void mk_thread_exit(void);

#endif /* MICROKERNEL_THREAD_H */
