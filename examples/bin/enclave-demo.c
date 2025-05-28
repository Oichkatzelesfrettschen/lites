/**
 * Lites repository license applies to this file; see the LICENSE file
 * in the project root for details.
 */

#include "enclave.h"

int main(void) {
    int h = enclave_create("demo");
    enclave_attest(h);
    return 0;
}
