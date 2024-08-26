#ifndef SISO2_TUN_DEVICE_H
#define SISO2_TUN_DEVICE_H

#include "siso2/net/data-source.h"
#include "siso2/net/data-sink.h"
#include <stddef.h>
#include "siso2/buffer/vector-buffer.h"
#ifdef SISO2_ADRV
#include "siso2/buffer/axidma-buffer.h"
#endif
#include "siso2/buffer/buffer-pool.h"
#include <string>

namespace SISO2
{

class TunDevice: public DataSource, public DataSink
{
    int fd = -1;
    size_t mtu;

    BufferPool<uint8_t> data_buffer_pool;

public:
    std::string devname;

    TunDevice(std::string devname, size_t packet_size, std::string ip_str, std::string dest_ip = "0.0.0.0", std::string remote_subnet = "10.0.0.0/24");
    Buffer<uint8_t>* get_data() override;
    void unpack(const std::vector<uint8_t>& packet) override;
};

}

#endif