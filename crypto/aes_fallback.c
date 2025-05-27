#include "aes_fallback.h"
#include <string.h>

/* Minimal AES-128 CTR mode implementation for use when OpenSSL is unavailable. */

#define AES_BLOCK_SIZE 16
#define AES_KEYLEN 16
#define AES_ROUND_KEY_SIZE 176
static uint8_t xtime(uint8_t x) { return (uint8_t)((x << 1) ^ ((x & 0x80) ? 0x1b : 0)); }

static uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t res = 0;
    while (b) {
        if (b & 1)
            res ^= a;
        uint8_t hi = a & 0x80;
        a <<= 1;
        if (hi)
            a ^= 0x1b;
        b >>= 1;
    }
    return res;
}

static uint8_t gf_pow(uint8_t a, uint8_t e) {
    uint8_t r = 1;
    while (e) {
        if (e & 1)
            r = gf_mul(r, a);
        a = gf_mul(a, a);
        e >>= 1;
    }
    return r;
}

static uint8_t gf_inv(uint8_t a) {
    if (a == 0)
        return 0;
    return gf_pow(a, 254); /* a^(2^8-2) */
}

static void aes_init_sbox(uint8_t sbox[256], uint8_t rsbox[256]) {
    for (int i = 0; i < 256; ++i) {
        uint8_t inv = gf_inv((uint8_t)i);
        uint8_t x = inv;
        x ^= (uint8_t)((inv << 1) | (inv >> 7));
        x ^= (uint8_t)((inv << 2) | (inv >> 6));
        x ^= (uint8_t)((inv << 3) | (inv >> 5));
        x ^= (uint8_t)((inv << 4) | (inv >> 4));
        x ^= 0x63;
        sbox[i] = x;
        rsbox[x] = (uint8_t)i;
    }
}

struct aes_ctx {
    uint8_t round_key[AES_ROUND_KEY_SIZE];
};

static void key_expansion(struct aes_ctx *ctx, const uint8_t key[16], const uint8_t sbox[256]) {
    memcpy(ctx->round_key, key, AES_KEYLEN);
    uint8_t rcon = 1;
    uint8_t *rk = ctx->round_key;
    for (int i = 16, j = 1; i < AES_ROUND_KEY_SIZE; i += 4) {
        uint8_t tmp[4];
        memcpy(tmp, &rk[i - 4], 4);
        if (i % 16 == 0) {
            uint8_t t = tmp[0];
            tmp[0] = sbox[tmp[1]] ^ rcon;
            tmp[1] = sbox[tmp[2]];
            tmp[2] = sbox[tmp[3]];
            tmp[3] = sbox[t];
            rcon = gf_mul(rcon, 2);
        }
        for (int k = 0; k < 4; ++k) {
            rk[i + k] = rk[i - 16 + k] ^ tmp[k];
        }
    }
}

static void sub_bytes(uint8_t state[16], const uint8_t sbox[256]) {
    for (int i = 0; i < 16; ++i)
        state[i] = sbox[state[i]];
}

static void shift_rows(uint8_t state[16]) {
    uint8_t tmp;
    /* row 1 shift left 1 */
    tmp = state[1];
    state[1] = state[5];
    state[5] = state[9];
    state[9] = state[13];
    state[13] = tmp;
    /* row 2 shift left 2 */
    tmp = state[2];
    uint8_t tmp2 = state[6];
    state[2] = state[10];
    state[6] = state[14];
    state[10] = tmp;
    state[14] = tmp2;
    /* row 3 shift left 3 */
    tmp = state[3];
    tmp2 = state[7];
    uint8_t tmp3 = state[11];
    state[3] = state[15];
    state[7] = tmp;
    state[11] = tmp2;
    state[15] = tmp3;
}

static void mix_columns(uint8_t state[16]) {
    for (int i = 0; i < 4; ++i) {
        uint8_t *col = &state[i * 4];
        uint8_t t = col[0] ^ col[1] ^ col[2] ^ col[3];
        uint8_t tmp = col[0];
        col[0] ^= t ^ xtime(col[0] ^ col[1]);
        col[1] ^= t ^ xtime(col[1] ^ col[2]);
        col[2] ^= t ^ xtime(col[2] ^ col[3]);
        col[3] ^= t ^ xtime(col[3] ^ tmp);
    }
}

static void add_round_key(uint8_t state[16], const uint8_t *rk) {
    for (int i = 0; i < 16; ++i)
        state[i] ^= rk[i];
}

static void aes_encrypt_block(struct aes_ctx *ctx, const uint8_t sbox[256], const uint8_t in[16],
                              uint8_t out[16]) {
    uint8_t state[16];
    memcpy(state, in, 16);
    add_round_key(state, ctx->round_key);
    for (int round = 1; round <= 9; ++round) {
        sub_bytes(state, sbox);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, ctx->round_key + round * 16);
    }
    sub_bytes(state, sbox);
    shift_rows(state);
    add_round_key(state, ctx->round_key + 160);
    memcpy(out, state, 16);
}

void aes128_ctr_encrypt(const uint8_t key[16], uint8_t iv[16], const uint8_t *in, uint8_t *out,
                        size_t len) {
    uint8_t sbox[256], rsbox[256];
    aes_init_sbox(sbox, rsbox);
    struct aes_ctx ctx;
    key_expansion(&ctx, key, sbox);

    uint8_t stream[AES_BLOCK_SIZE];
    size_t offset = 0;
    while (offset < len) {
        aes_encrypt_block(&ctx, sbox, iv, stream);
        for (int i = AES_BLOCK_SIZE - 1; i >= 0; --i) {
            if (++iv[i])
                break;
        }
        size_t todo = len - offset < AES_BLOCK_SIZE ? len - offset : AES_BLOCK_SIZE;
        for (size_t j = 0; j < todo; ++j) {
            out[offset + j] = in[offset + j] ^ stream[j];
        }
        offset += todo;
    }
}
