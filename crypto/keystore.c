#include "keystore.h"

#include <errno.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct ks_key {
    unsigned char key[32];
};

static int load_key(const char *path, ks_key_t **out) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    ks_key_t *k = malloc(sizeof(*k));
    if (!k) {
        close(fd);
        return -1;
    }
    ssize_t r = read(fd, k->key, sizeof(k->key));
    close(fd);
    if (r != (ssize_t)sizeof(k->key)) {
        free(k);
        return -1;
    }
    *out = k;
    return 0;
}

int ks_generate_key(const char *path) {
    unsigned char key[32];
    if (RAND_bytes(key, sizeof(key)) != 1) {
        return -1;
    }

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        return -1;
    }
    ssize_t w = write(fd, key, sizeof(key));
    close(fd);
    if (w != (ssize_t)sizeof(key)) {
        return -1;
    }
    return 0;
}

ks_key_t *ks_open(const char *path) {
    ks_key_t *k;
    if (load_key(path, &k) < 0) {
        return NULL;
    }
    return k;
}

void ks_close(ks_key_t *key) {
    if (key) {
        memset(key, 0, sizeof(*key));
        free(key);
    }
}

int ks_encrypt(ks_key_t *key, const unsigned char *in, size_t in_len, unsigned char *out,
               size_t *out_len) {
    if (!key || !in || !out || !out_len) {
        return -1;
    }

    unsigned char iv[16];
    if (RAND_bytes(iv, sizeof(iv)) != 1) {
        return -1;
    }
    memcpy(out, iv, sizeof(iv));

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return -1;
    }
    int len = 0, total = 16;
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key->key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    if (EVP_EncryptUpdate(ctx, out + 16, &len, in, (int)in_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    total += len;
    if (EVP_EncryptFinal_ex(ctx, out + total, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    total += len;
    EVP_CIPHER_CTX_free(ctx);
    *out_len = (size_t)total;
    return 0;
}

int ks_decrypt(ks_key_t *key, const unsigned char *in, size_t in_len, unsigned char *out,
               size_t *out_len) {
    if (!key || !in || in_len < 16 || !out || !out_len) {
        return -1;
    }
    const unsigned char *iv = in;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return -1;
    }
    int len = 0, total = 0;
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key->key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    if (EVP_DecryptUpdate(ctx, out, &len, in + 16, (int)(in_len - 16)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    total += len;
    if (EVP_DecryptFinal_ex(ctx, out + total, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    total += len;
    EVP_CIPHER_CTX_free(ctx);
    *out_len = (size_t)total;
    return 0;
}
