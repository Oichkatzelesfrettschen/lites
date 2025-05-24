#ifndef ENCLAVE_H
#define ENCLAVE_H

int enclave_create(const char *name);
int enclave_attest(int handle);

#endif /* ENCLAVE_H */
