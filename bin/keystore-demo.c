#include "keystore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <keyfile> <message>\n", argv[0]);
        return 1;
    }

    if (ks_generate_key(argv[1], 16) != 0) {
        perror("ks_generate_key");
        return 1;
    }

    size_t msg_len = strlen(argv[2]);
    unsigned char *enc = malloc(msg_len);
    unsigned char *dec = malloc(msg_len + 1);
    if (!enc || !dec) {
        fprintf(stderr, "Memory allocation failure\n");
        free(enc);
        free(dec);
        return 1;
    }

    size_t enc_len;
    ks_encrypt(argv[1], (const unsigned char *)argv[2], msg_len, enc, &enc_len);

    printf("ciphertext: ");
    for (size_t i = 0; i < enc_len; i++) {
        printf("%02x", enc[i]);
    }
    printf("\n");

    size_t dec_len;
    ks_decrypt(argv[1], enc, enc_len, dec, &dec_len);
    dec[dec_len] = '\0';
    printf("decrypted: %s\n", dec);

    free(enc);
    free(dec);
    return 0;
}
