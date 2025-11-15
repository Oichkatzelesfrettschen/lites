/* kern/zalloc.h - Zone allocator interface */
#ifndef _KERN_ZALLOC_H_
#define _KERN_ZALLOC_H_

#include <sys/types.h>

/* Zone allocator stubs - disabled for this build */
typedef struct zone *zone_t;

#define ZONE_NULL	((zone_t) 0)

/* Stub macros */
#define zinit(size, max, alloc, name) 	((zone_t)0)
#define zalloc(zone)			((void *)0)
#define zfree(zone, addr)		do { } while (0)
#define zget(zone)			((void *)0)
#define zchange(zone, pageable, sleepable, exhaustible, collectable) do { } while (0)

#endif /* _KERN_ZALLOC_H_ */
