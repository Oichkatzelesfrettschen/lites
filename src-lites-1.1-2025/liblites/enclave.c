#include "enclave.h"

#include <stdio.h>

int enclave_create(const char *name) {
    printf("enclave '%s' created\n", name);
    return 1;
}

int enclave_attest(int handle) {
    printf("enclave %d attested\n", handle);
    return 0;
}
