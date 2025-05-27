/**
 * Lites repository license applies to this file; see the LICENSE file
 * in the project root for details.
 */

#include "keystore.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if KS_HAVE_OPENSSL
#include <openssl/aes.h>
#else
#include "aes_fallback.h"
#endif

static int read_key(const char *path, unsigned char **key, size_t *len) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    off_t sz = lseek(fd, 0, SEEK_END);
    if (sz <= 0) {
        close(fd);
        return -1;
    }
    if (lseek(fd, 0, SEEK_SET) < 0) {
        close(fd);
        return -1;
    }
    *key = malloc(sz);
    if (!*key) {
        close(fd);
        return -1;
    }
    size_t off = 0;
    while (off < (size_t)sz) {
        ssize_t r = read(fd, *key + off, (size_t)sz - off);
        if (r < 0) {
            if (errno == EINTR) {
                continue;
            }
            free(*key);
            close(fd);
            return -1;
        }
        if (r == 0) {
            /* unexpected EOF */
            free(*key);
            close(fd);
            errno = EIO;
            return -1;
        }
        off += (size_t)r;
    }
    close(fd);
    *len = sz;
    return 0;
}

int ks_generate_key(const char *path, size_t len) {
    unsigned char *buf = malloc(len);
    if (!buf) {
        return -1;
    }
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        free(buf);
        return -1;
    }
    size_t off = 0;
    while (off < len) {
        ssize_t r = read(fd, buf + off, len - off);
        if (r < 0) {
            if (errno == EINTR) {
                continue;
            }
            close(fd);
            free(buf);
            return -1;
        }
        if (r == 0) {
            /* unexpected EOF */
            close(fd);
            free(buf);
            errno = EIO;
            return -1;
        }
        off += (size_t)r;
    }
    close(fd);
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        free(buf);
        return -1;
    }
    off = 0;
    while (off < len) {
        ssize_t w = write(fd, buf + off, len - off);
        if (w < 0) {
            if (errno == EINTR) {
                continue;
            }
            close(fd);
            free(buf);
            return -1;
        }
        if (w == 0) {
            /* unexpected short write */
            close(fd);
            free(buf);
            errno = EIO;
            return -1;
        }
        off += (size_t)w;
    }
    close(fd);
    free(buf);
    return 0;
}

int ks_encrypt(const char *key_path, const unsigned char *in, size_t in_len, unsigned char *out,
               size_t *out_len) {
    unsigned char *key;
    size_t key_len;
    if (read_key(key_path, &key, &key_len) < 0) {
        return -1;
    }
#if KS_HAVE_OPENSSL
    unsigned char aes_keybuf[16];
    for (size_t i = 0; i < sizeof(aes_keybuf); i++) {
        aes_keybuf[i] = key[i % key_len];
    }
    AES_KEY aes;
    if (AES_set_encrypt_key(aes_keybuf, 128, &aes) != 0) {
        free(key);
        return -1;
    }
    unsigned char ivec[AES_BLOCK_SIZE] = {0};
    unsigned char ecount[AES_BLOCK_SIZE] = {0};
    unsigned int num = 0;
    AES_ctr128_encrypt(in, out, in_len, &aes, ivec, ecount, &num);
#else
    unsigned char aes_keybuf[16];
    for (size_t i = 0; i < sizeof(aes_keybuf); i++) {
        aes_keybuf[i] = key[i % key_len];
    }
    unsigned char ivec[16] = {0};
    aes128_ctr_encrypt(aes_keybuf, ivec, in, out, in_len);
#endif
    *out_len = in_len;
    free(key);
    return 0;
}

int ks_decrypt(const char *key_path, const unsigned char *in, size_t in_len, unsigned char *out,
               size_t *out_len) {
    return ks_encrypt(key_path, in, in_len, out, out_len);
}
