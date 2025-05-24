#include "enclave.h"

#include <stdio.h>

int main(void) {
    int h = enclave_create("demo");
    unsigned char hash[32];
    enclave_attest(h, hash);
    printf("attestation hash: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
    return 0;
}
