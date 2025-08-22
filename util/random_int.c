#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Generate a pseudo-random integer modulo the supplied argument.
 *
 * The generator seeds itself using the current process identifier and a
 * secondary value derived from the C library `rand` implementation. The
 * result is printed to standard output.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments. The first element after the
 *             program name is interpreted as the modulus.
 * @return Zero on success, non-zero if the modulus argument is missing.
 */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <mod>\n", argv[0]);
        return 1;
    }

    int mod = atoi(argv[1]);

    srand(getpid());
    int rn = (rand() >> 16) & 0xffff;
    srand(rn);

    rn = rand() % mod;
    printf("%d", rn);
    return 0;
}
