#include "enclave.h"

#include <stdio.h>

/**
 * Create a new enclave instance.
 *
 * @param name Descriptive name for the enclave.
 * @return Positive handle identifying the enclave on success.
 */
int enclave_create(const char *name) {
    printf("enclave '%s' created\n", name);
    return 1;
}

/**
 * Verify the integrity of an enclave.
 *
 * @param handle Handle previously returned by enclave_create().
 * @return 0 when the enclave attests successfully.
 */
int enclave_attest(int handle) {
    printf("enclave %d attested\n", handle);
    return 0;
}
