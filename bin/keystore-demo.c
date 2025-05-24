#include "keystore.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <keyfile> <message>\n", argv[0]);
        return 1;
    }

    if (ks_generate_key(argv[1]) != 0) {
        perror("ks_generate_key");
        return 1;
    }

    ks_key_t *key = ks_open(argv[1]);
    if (!key) {
        perror("ks_open");
        return 1;
    }

    unsigned char enc[512];
    size_t enc_len;
    ks_encrypt(key, (const unsigned char *)argv[2], strlen(argv[2]), enc, &enc_len);

    printf("ciphertext: ");
    for (size_t i = 0; i < enc_len; i++) {
        printf("%02x", enc[i]);
    }
    printf("\n");

    unsigned char dec[512];
    size_t dec_len;
    ks_decrypt(key, enc, enc_len, dec, &dec_len);
    dec[dec_len] = '\0';
    printf("decrypted: %s\n", dec);
    ks_close(key);
    return 0;
}
