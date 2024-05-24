#include "xorshift.h"
#include <stdint.h>



static uint64_t xorshift64(struct xorshift64_state *state)
{
    uint64_t x = state->a;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state->a = x;
}

void xor_random_bytes(void *buf, size_t n, struct xorshift64_state *state)
{
    size_t offset = 0;
    while (n > 0) {
        *((char *) buf + offset) = xorshift64(state);
        n -= sizeof(uint64_t);
        offset += sizeof(uint64_t);
    }
}
