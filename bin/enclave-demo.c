#include "enclave.h"

int main(void) {
    int h = enclave_create("demo");
    enclave_attest(h);
    return 0;
}
