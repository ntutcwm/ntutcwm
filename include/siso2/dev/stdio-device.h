#ifndef SISO2_STDIO_DEVICE_H
#define SISO2_STDIO_DEVICE_H

#include "siso2/dev/iq-device.h"
#include "siso2/util/complex.h"
#include <stdio.h>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include "siso2/phy/phy.h"
#include "siso2/buffer/vector-buffer.h"
#include "siso2/buffer/buffer-pool.h"

namespace SISO2
{

class StdioDevice: public IqDevice
{
    uint32_t tx_rate;

    int in_fd;
    int out_fd;

    BufferPool<std::complex<int16_t>> tx_buffer_pool;
    BufferPool<std::complex<int16_t>> rx_buffer_pool;

public:
    StdioDevice(size_t tx_block_size, size_t rx_block_size, int in_fd = 0, int out_fd = 1);

    std::string get_name() override { return "stdio"; }

    /* TX */
    void set_tx_rate(double rate) override { tx_rate = (uint32_t)rate; }
    double get_tx_rate() override { return tx_rate; }
    std::string get_tx_port() override { return "stdout"; }
    std::vector<std::string> get_tx_ports_available() override { return { "stdout" }; }
    BufferPool<std::complex<int16_t>>* get_tx_buffer_pool() override{ return &tx_buffer_pool; }
    int write(IQBuffer* in) override;

    /* RX */
    std::string get_rx_port() override { return "stdin"; }
    std::vector<std::string> get_rx_ports_available() override { return { "stdin" }; }
    IQBuffer* read() override;
};

}

#endif