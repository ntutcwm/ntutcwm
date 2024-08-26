#ifndef SISO2_DATA_SOURCE_H
#define SISO2_DATA_SOURCE_H

#include <stdint.h>
#include <stddef.h>
#include "siso2/buffer/buffer.h"

namespace SISO2
{

class DataSource
{
public:
    struct Stats
    {
        size_t n_bursts = 0;
        size_t n_packets = 0;
        size_t header_bytes = 0;
        size_t data_bytes = 0;
        size_t total_bytes = 0;
    };

    Stats stats = {};
    virtual Buffer<uint8_t>* get_data() = 0;
};

};

#endif