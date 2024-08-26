#ifndef SISO2_DATA_SINK_H
#define SISO2_DATA_SINK_H

#include <stddef.h>
#include <stdint.h>
#include <vector>

namespace SISO2
{

class DataSink
{
public:
    struct Stats
    {
        size_t header_bytes = 0;
        size_t data_bytes = 0;
        size_t total_bytes = 0;
    };

    Stats stats = {};
    virtual void unpack(const std::vector<uint8_t>& packet) = 0;
};

};

#endif