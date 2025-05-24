#ifndef ENCLAVE_H
#define ENCLAVE_H

int enclave_create(const char *name);
int enclave_attest(int handle, unsigned char *hash);

#endif /* ENCLAVE_H */
