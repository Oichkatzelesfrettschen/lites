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

/**
 * @brief Read all data from a file descriptor.
 *
 * Repeatedly reads until the buffer is filled or an unrecoverable error
 * occurs.
 *
 * @param fd   File descriptor to read from.
 * @param buf  Destination buffer.
 * @param len  Number of bytes to read.
 * @return 0 on success, -1 on failure with errno set.
 */
static int fd_read_all(int fd, unsigned char *buf, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t r = read(fd, buf + off, len - off);
        if (r < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (r == 0) {
            errno = EIO;
            return -1;
        }
        off += (size_t)r;
    }
    return 0;
}

/**
 * @brief Write all data to a file descriptor.
 *
 * @param fd   File descriptor to write to.
 * @param buf  Data to write.
 * @param len  Number of bytes to write.
 * @return 0 on success, -1 on failure with errno set.
 */
static int fd_write_all(int fd, const unsigned char *buf, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t w = write(fd, buf + off, len - off);
        if (w < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (w == 0) {
            errno = EIO;
            return -1;
        }
        off += (size_t)w;
    }
    return 0;
}

/**
 * @brief Read a key file into memory.
 *
 * The caller must free the returned buffer.
 *
 * @param path Path to the key file.
 * @param key  [out] Allocated buffer with key bytes.
 * @param len  [out] Length of @p key.
 * @return 0 on success, -1 on error.
 */
static int read_key(const char *path, unsigned char **key, size_t *len) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    off_t sz = lseek(fd, 0, SEEK_END);
    if (sz <= 0 || lseek(fd, 0, SEEK_SET) < 0) {
        close(fd);
        return -1;
    }
    *key = malloc((size_t)sz);
    if (!*key) {
        close(fd);
        return -1;
    }
    if (fd_read_all(fd, *key, (size_t)sz) < 0) {
        free(*key);
        close(fd);
        return -1;
    }
    close(fd);
    *len = (size_t)sz;
    return 0;
}

/**
 * @brief Generate a random key and store it to disk.
 *
 * @param path Destination path for the generated key.
 * @param len  Desired key length in bytes.
 * @return 0 on success, -1 on error.
 */
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
    if (fd_read_all(fd, buf, len) < 0) {
        close(fd);
        free(buf);
        return -1;
    }
    close(fd);

    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) {
        free(buf);
        return -1;
    }
    int ret = fd_write_all(fd, buf, len);
    close(fd);
    free(buf);
    return ret;
}

/**
 * @brief Encrypt a buffer using AES-128 CTR mode.
 *
 * @param key_path Path to the symmetric key file.
 * @param in       Plaintext input buffer.
 * @param in_len   Size of the plaintext in bytes.
 * @param out      Buffer receiving the ciphertext.
 * @param out_len  [out] Number of bytes written to @p out.
 * @return 0 on success, -1 on failure.
 */
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

/**
 * @brief Decrypt a buffer using AES-128 CTR mode.
 *
 * This operation mirrors @ref ks_encrypt since CTR mode uses the same
 * primitive for both directions.
 *
 * @param key_path Path to the symmetric key file.
 * @param in       Ciphertext input buffer.
 * @param in_len   Size of the ciphertext in bytes.
 * @param out      Buffer receiving the plaintext.
 * @param out_len  [out] Number of bytes written to @p out.
 * @return 0 on success, -1 on failure.
 */
int ks_decrypt(const char *key_path, const unsigned char *in, size_t in_len, unsigned char *out,
               size_t *out_len) {
    return ks_encrypt(key_path, in, in_len, out, out_len);
}
