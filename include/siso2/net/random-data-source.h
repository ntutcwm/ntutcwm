#ifndef SISO2_RANDOM_DATA_SOURCE_H
#define SISO2_RANDOM_DATA_SOURCE_H

#include "siso2/net/data-source.h"
#include <stddef.h>
#include "siso2/buffer/vector-buffer.h"
#ifdef SISO2_ADRV
#include "siso2/buffer/vector-buffer.h"
#endif
#include "siso2/buffer/buffer-pool.h"
#include "siso2/net/packet.h"
#include <stdlib.h>

namespace SISO2
{

class RandomDataSource: public DataSource
{
    BufferPool<uint8_t> data_buffer_pool;

public:
    RandomDataSource(size_t packet_size) : 
#ifdef SISO2_ADRV
    data_buffer_pool((AXIDMABuffer<uint8_t>*)nullptr, 1024, packet_size)
#else
    data_buffer_pool((VectorBuffer<uint8_t>*)nullptr, 1024, packet_size)
#endif
    {}
    Buffer<uint8_t>* get_data() override {
        static uint16_t sequence = 0;
        auto data_packet = data_buffer_pool.get_buffer();
        auto packet_header = (PacketHeader*)data_packet->data;
        auto section_header = (SectionHeader*)(data_packet->data + sizeof(PacketHeader));
        for (int i = sizeof(PacketHeader) + sizeof(SectionHeader); i < data_packet->size; i++) data_packet->data[i] = rand();
        packet_header->sequence = sequence++;
        section_header->channel = 0xFF;
        section_header->size = 0;
        stats.data_bytes += 0;
        stats.header_bytes += sizeof(PacketHeader) + sizeof(SectionHeader);
        return data_packet;
    }
};

}

#endif