#include "enclave.h"

#include <stdio.h>

/**
 * @brief Create a new enclave.
 *
 * @details This stub merely logs creation of an enclave. A production
 * implementation would allocate and initialize a protected execution
 * context and return a unique handle representing it.
 *
 * @param[in] name Human-readable identifier used for logging and debugging.
 *                 Must not be `NULL`.
 *
 * @return Positive handle to the newly created enclave on success.
 * @return Negative error code on failure.
 *
 * @todo Replace stub with real enclave creation logic.
 */
int enclave_create(const char *name) {
    printf("enclave '%s' created\n", name);
    return 1;
}

/**
 * @brief Attest to the integrity of an enclave.
 *
 * @details This stub simulates verifying the enclave's measurement. A
 * complete implementation would perform cryptographic checks to ensure
 * the enclave's code and data have not been tampered with.
 *
 * @param[in] handle Handle returned by enclave_create() identifying the
 *                   enclave to attest.
 *
 * @return 0 when attestation succeeds.
 * @return Negative error code on failure.
 *
 * @todo Replace stub with real attestation logic.
 */
int enclave_attest(int handle) {
    printf("enclave %d attested\n", handle);
    return 0;
}
