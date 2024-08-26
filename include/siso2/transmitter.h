#ifndef SISO2_TRANSMITTER_H
#define SISO2_TRANSMITTER_H

#include <stdint.h>
#include <stddef.h>
#include "siso2/net/data-source.h"

namespace SISO2
{

class Transmitter
{
public:
    virtual void enable() = 0;
    virtual bool enabled() = 0;
    virtual void disable() = 0;
    virtual size_t tx_queue_size() = 0;
    virtual void set_duty_cycle(float duty_cycle) = 0;
    virtual float get_duty_cycle() = 0;
    virtual size_t n_cached() { return 0; }

    struct Stats
    {
        uint32_t tx_samples = 0;
        uint32_t tx_queue_size = 0;
        uint32_t udp_queue_size = 0;
        size_t n_bursts = 0;
        size_t n_packets = 0;
        DataSource::Stats source_stats = {};
    };

    Stats stats = {};
};

};

#endif