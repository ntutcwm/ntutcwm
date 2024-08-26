#ifndef SISO2_LFSR_H
#define SISO2_LFSR_H

#include <stdint.h>

typedef struct lfsr_t {
    uint16_t taps;
    uint16_t state;
} lfsr_t;

static inline uint8_t lfsr_next(lfsr_t* lfsr)
{
    uint8_t out = lfsr->state & 1;    /* Get LSB (i.e., the output bit). */
    lfsr->state >>= 1;                /* Shift register */
    if (out) lfsr->state ^= lfsr->taps;
    return out;
}

#endif