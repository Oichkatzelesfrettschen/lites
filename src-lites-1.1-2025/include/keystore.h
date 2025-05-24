#ifndef KEYSTORE_H
#define KEYSTORE_H

#include <stddef.h>

typedef struct ks_key ks_key_t;

int ks_generate_key(const char *path);
ks_key_t *ks_open(const char *path);
void ks_close(ks_key_t *key);
int ks_encrypt(ks_key_t *key, const unsigned char *in, size_t in_len, unsigned char *out,
               size_t *out_len);
int ks_decrypt(ks_key_t *key, const unsigned char *in, size_t in_len, unsigned char *out,
               size_t *out_len);

#endif /* KEYSTORE_H */
