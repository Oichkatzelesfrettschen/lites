# Security APIs

This document describes the keystore and enclave interfaces added to the modernised source tree.

## Keystore

The trivial keystore lives in `crypto/keystore.c`.

```c
int ks_generate_key(const char *path, size_t len);
int ks_encrypt(const char *key_path, const unsigned char *in, size_t in_len,
               unsigned char *out, size_t *out_len);
int ks_decrypt(const char *key_path, const unsigned char *in, size_t in_len,
               unsigned char *out, size_t *out_len);
```

`ks_generate_key` creates a random symmetric key and writes it to the specified
file.  `ks_encrypt` and `ks_decrypt` always use AES‑128 in CTR mode.  When the
OpenSSL library is available the implementation relies on it; otherwise a small
built‑in AES routine is used.  Either path provides real confidentiality.

## Enclave

`liblites/enclave.c` in the source tree provides example enclave hooks.

```c
int enclave_create(const char *name);
int enclave_attest(int handle);
```

The current implementation merely prints diagnostic messages but serves as a
placeholder for a real secure enclave backend.

## Command-line utilities

Two small utilities under `bin/` demonstrate the APIs:

- `keystore-demo` generates a key, encrypts a message and then decrypts it again.
- `enclave-demo` creates a dummy enclave and performs an attestation step.

They can be compiled manually, for example:

```sh
# link with -lcrypto when OpenSSL is available
cc -I "$LITES_SRC_DIR/include" crypto/keystore.c bin/keystore-demo.c -o keystore-demo -lcrypto
cc -I "$LITES_SRC_DIR/include" "$LITES_SRC_DIR/liblites/enclave.c" bin/enclave-demo.c -o enclave-demo
```
