#ifndef SISO2_UDP_DEVICE_H
#define SISO2_UDP_DEVICE_H

#include "siso2/dev/iq-device.h"
#include <stdint.h>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <arpa/inet.h>
#include "siso2/buffer/vector-buffer.h"
#include "siso2/buffer/buffer-pool.h"

#define MAX_UDP_PKT_SIZE 65536ul

namespace SISO2
{

class UdpDevice: public IqDevice
{    
    int socket_fd;
    struct sockaddr_in remote;

    uint32_t tx_rate;

    BufferPool<std::complex<int16_t>> tx_buffer_pool;
    BufferPool<std::complex<int16_t>> rx_buffer_pool;

public:
    UdpDevice(size_t tx_block_size, size_t rx_block_size, uint16_t port, std::string remote_addr);
    ~UdpDevice();

    std::string get_name() override { return "udp"; }

    /* TX */
    void set_tx_rate(double rate) override { tx_rate = (uint32_t)rate; }
    double get_tx_rate() override { return tx_rate; }
    std::string get_tx_port() override { return "udp"; }
    std::vector<std::string> get_tx_ports_available() override { return { "udp" }; }
    BufferPool<std::complex<int16_t>>* get_tx_buffer_pool() override{ return &tx_buffer_pool; }
    int write(IQBuffer* in) override;

    /* RX */
    std::string get_rx_port() override { return "udp"; }
    std::vector<std::string> get_rx_ports_available() override { return { "udp" }; }
    IQBuffer* read() override;
};

}

#endif