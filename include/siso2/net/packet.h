#ifndef SISO2_PACKET_H
#define SISO2_PACKET_H

#include <stdint.h>
#include <time.h>

namespace SISO2
{
struct PacketHeader
{
    uint16_t sequence;
    uint64_t timestamp: 48;
} __attribute__((packed));

struct SectionHeader
{
    uint8_t size;
    uint8_t channel;
} __attribute__((packed));

static inline uint64_t time_us()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000ull + ts.tv_nsec/1000ull;
}

}

#endif