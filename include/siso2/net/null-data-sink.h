#ifndef SISO2_NULL_DATA_SINK_H
#define SISO2_NULL_DATA_SINK_H

#include "siso2/net/data-sink.h"

namespace SISO2
{

class NullDataSink: public DataSink
{
public:
    void unpack(const std::vector<uint8_t>& packet) override {}
};

}

#endif