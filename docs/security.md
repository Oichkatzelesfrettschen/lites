# Security APIs

This document describes the keystore and enclave interfaces added to the modernised source tree.

## Keystore

The keystore lives in `crypto/keystore.c` and now uses AES-256 from OpenSSL.

```c
typedef struct ks_key ks_key_t;
int ks_generate_key(const char *path);
ks_key_t *ks_open(const char *path);
void ks_close(ks_key_t *key);
int ks_encrypt(ks_key_t *key, const unsigned char *in, size_t in_len,
               unsigned char *out, size_t *out_len);
int ks_decrypt(ks_key_t *key, const unsigned char *in, size_t in_len,
               unsigned char *out, size_t *out_len);
```

`ks_generate_key` creates a random AES-256 key and writes it to the specified file.
Keys are loaded with `ks_open` and freed with `ks_close`.
`ks_encrypt` and `ks_decrypt` use AES-256-CTR with a random IV prepended to the ciphertext.

## Enclave

`src-lites-1.1-2025/liblites/enclave.c` provides example enclave hooks.

```c
int enclave_create(const char *name);
int enclave_attest(int handle, unsigned char *hash);
```

`enclave_create` stores metadata about the enclave and `enclave_attest` returns
a fake SHA-256 hash of its name.

## Command-line utilities

Two small utilities under `bin/` demonstrate the APIs:

- `keystore-demo` generates a key, encrypts a message and then decrypts it again.
- `enclave-demo` creates a dummy enclave and performs an attestation step.

They can be compiled manually, for example:

```sh
cc -I src-lites-1.1-2025/include -lcrypto \
   crypto/keystore.c bin/keystore-demo.c -o keystore-demo
cc -I src-lites-1.1-2025/include -lcrypto \
   src-lites-1.1-2025/liblites/enclave.c bin/enclave-demo.c -o enclave-demo
```
