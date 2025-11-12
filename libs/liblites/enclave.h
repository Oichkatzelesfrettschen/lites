/**
 * @file enclave.h
 * @brief Public interface for enclave management stubs.
 */

#ifndef LIBLITES_ENCLAVE_H
#define LIBLITES_ENCLAVE_H

/**
 * @brief Create a new enclave.
 *
 * @param[in] name Human-readable identifier used for logging.
 * @returns Positive handle to the newly created enclave on success; negative
 *          error code on failure.
 */
int enclave_create(const char *name);

/**
 * @brief Attest to the integrity of an enclave.
 *
 * @param[in] handle Handle returned by enclave_create() identifying the enclave
 *                   to attest.
 * @returns 0 when attestation succeeds; negative error code on failure.
 */
int enclave_attest(int handle);

#endif /* LIBLITES_ENCLAVE_H */
