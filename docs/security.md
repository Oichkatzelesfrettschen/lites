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
file.  `ks_encrypt` and `ks_decrypt` perform a simple XOR-based transformation
with that key.  **This method provides no real security and is intended purely
for demonstration.**  Future work may replace the implementation with a
standard cipher such as AES, potentially using the OpenSSL library.

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
cc -I "$LITES_SRC_DIR/include" crypto/keystore.c bin/keystore-demo.c -o keystore-demo
cc -I "$LITES_SRC_DIR/include" "$LITES_SRC_DIR/liblites/enclave.c" bin/enclave-demo.c -o enclave-demo
```
