#pragma once
#include <stddef.h>
#include <stdint.h>

void aes128_ctr_encrypt(const uint8_t key[16], uint8_t iv[16], const uint8_t *in, uint8_t *out,
                        size_t len);
