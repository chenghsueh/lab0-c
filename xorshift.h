#include <stddef.h>
#include <stdint.h>

struct xorshift64_state {
    uint64_t a;
};

extern void xor_random_bytes(void *buf,
                             size_t n,
                             struct xorshift64_state *state);