/*
 * CMU Queue macros
 * Carnegie Mellon University queue manipulation macros
 * Similar to BSD queue.h but CMU-specific implementation
 */

#ifndef _SYS_CMU_QUEUE_H_
#define _SYS_CMU_QUEUE_H_

/* Simple queue - singly linked */
#define queue_init(q)       ((q)->next = (q)->prev = (q))
#define queue_empty(q)      ((q)->next == (q))
#define queue_first(q)      ((q)->next)
#define queue_last(q)       ((q)->prev)
#define queue_next(elt)     ((elt)->next)
#define queue_prev(elt)     ((elt)->prev)
#define queue_end(q, elt)   ((q) == (elt))

/* Queue element structure */
struct queue_entry {
    struct queue_entry  *next;
    struct queue_entry  *prev;
};

typedef struct queue_entry  queue_t;
typedef struct queue_entry  queue_head_t;
typedef struct queue_entry  *queue_entry_t;

/* Queue operations */
#define enqueue(q, elt) do {                            \
    (elt)->next = (q);                                  \
    (elt)->prev = (q)->prev;                            \
    (q)->prev->next = (elt);                            \
    (q)->prev = (elt);                                  \
} while (0)

#define dequeue(elt) do {                               \
    (elt)->next->prev = (elt)->prev;                    \
    (elt)->prev->next = (elt)->next;                    \
} while (0)

#define remqueue(q, elt) do {                           \
    if (queue_empty(q)) {                               \
        elt = 0;                                        \
    } else {                                            \
        elt = queue_first(q);                           \
        dequeue(elt);                                   \
    }                                                   \
} while (0)

#define insque(entry, pred)  do {                       \
    (entry)->next = (pred)->next;                       \
    (entry)->prev = (pred);                             \
    (pred)->next->prev = (entry);                       \
    (pred)->next = (entry);                             \
} while (0)

#define remque(entry) do {                              \
    (entry)->next->prev = (entry)->prev;                \
    (entry)->prev->next = (entry)->next;                \
} while (0)

#endif /* _SYS_CMU_QUEUE_H_ */
