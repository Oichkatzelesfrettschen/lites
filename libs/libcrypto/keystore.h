#ifndef LITES_KEYSTORE_H
#define LITES_KEYSTORE_H

#include <stddef.h>

int ks_generate_key(const char *path, size_t len);
int ks_encrypt(const char *key_path, const unsigned char *in, size_t in_len,
               unsigned char *out, size_t *out_len);
int ks_decrypt(const char *key_path, const unsigned char *in, size_t in_len,
               unsigned char *out, size_t *out_len);

#endif /* LITES_KEYSTORE_H */
