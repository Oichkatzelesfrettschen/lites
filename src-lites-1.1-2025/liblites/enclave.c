#include "enclave.h"

#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct enclave {
    int handle;
    char name[64];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    struct enclave *next;
};

static struct enclave *head;
static int next_handle = 1;

int enclave_create(const char *name) {
    struct enclave *e = malloc(sizeof(*e));
    if (!e) {
        return -1;
    }
    e->handle = next_handle++;
    strncpy(e->name, name, sizeof(e->name) - 1);
    e->name[sizeof(e->name) - 1] = '\0';
    SHA256((const unsigned char *)e->name, strlen(e->name), e->hash);
    e->next = head;
    head = e;
    printf("enclave '%s' created\n", e->name);
    return e->handle;
}

int enclave_attest(int handle, unsigned char *out_hash) {
    for (struct enclave *e = head; e; e = e->next) {
        if (e->handle == handle) {
            if (out_hash) {
                memcpy(out_hash, e->hash, SHA256_DIGEST_LENGTH);
            }
            printf("enclave %d attested\n", handle);
            return 0;
        }
    }
    fprintf(stderr, "enclave %d not found\n", handle);
    return -1;
}
