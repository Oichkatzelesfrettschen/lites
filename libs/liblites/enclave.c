#include "enclave.h"

#include <stdio.h>

/**
 * @brief Instantiate a new enclave.
 *
 * @details This placeholder emits a diagnostic message and returns a
 * static handle. A complete implementation would allocate and initialize
 * a protected execution context, yielding a unique enclave identifier.
 *
 * @param[in] name Human-readable identifier for the enclave. Must not
 *                 be `NULL`.
 *
 * @return Positive handle to the newly created enclave.
 * @retval <0 Negative error code on failure.
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
 * @details This stub simulates attestation by printing a message. A real
 * implementation would verify cryptographic measurements to ensure the
 * enclave's code and data remain unmodified.
 *
 * @param[in] handle Handle returned by enclave_create identifying the
 *                   enclave subject to attestation.
 *
 * @return 0 when attestation succeeds.
 * @retval <0 Negative error code on failure.
 *
 * @todo Replace stub with real attestation logic.
 */
int enclave_attest(int handle) {
    printf("enclave %d attested\n", handle);
    return 0;
}
