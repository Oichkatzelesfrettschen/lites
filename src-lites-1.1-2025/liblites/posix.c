#include "../include/posix_helpers.h"
#include <stdlib.h>
#include <string.h>

struct prot_entry {
    void *addr;
    size_t len;
    int prot;
    struct prot_entry *next;
};

static struct prot_entry *prot_list;

static struct prot_entry *find_region(void *addr, size_t len) {
    for (struct prot_entry *e = prot_list; e; e = e->next)
        if (e->addr == addr && e->len == len)
            return e;
    return NULL;
}

void lites_track_region(void *addr, size_t len, int prot) {
    struct prot_entry *e = find_region(addr, len);
    if (e) {
        e->prot = prot;
        return;
    }
    e = malloc(sizeof(*e));
    if (!e)
        return;
    e->addr = addr;
    e->len = len;
    e->prot = prot;
    e->next = prot_list;
    prot_list = e;
}

void lites_untrack_region(void *addr, size_t len) {
    struct prot_entry **pp = &prot_list;
    while (*pp) {
        if ((*pp)->addr == addr && (*pp)->len == len) {
            struct prot_entry *tmp = *pp;
            *pp = tmp->next;
            free(tmp);
            return;
        }
        pp = &(*pp)->next;
    }
}

int lites_get_prot(void *addr, size_t len) {
    struct prot_entry *e = find_region(addr, len);
    return e ? e->prot : -1;
}

int lites_mprotect(void *addr, size_t len, int prot) {
    if (mprotect(addr, len, prot) == -1)
        return -1;
    lites_track_region(addr, len, prot);
    return 0;
}

int lites_msync(void *addr, size_t len) {
    return msync(addr, len, MS_SYNC);
}

