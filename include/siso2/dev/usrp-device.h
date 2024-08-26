#ifndef SISO2_USRP_DEVICE_H
#define SISO2_USRP_DEVICE_H

#include "siso2/dev/iq-device.h"
#include <stdio.h>
#include <uhd.h>
#include <string.h>
#include "siso2/buffer/buffer-pool.h"
#include <mutex>
#include <atomic_queue/atomic_queue.h>

namespace SISO2
{

class UsrpDevice: public IqDevice
{
    size_t channel = 0;

    uhd_usrp_handle usrp;
    
    uhd_tx_streamer_handle tx_streamer;
    uhd_tx_metadata_handle tx_metadata;

    uhd_rx_streamer_handle rx_streamer;
    uhd_rx_metadata_handle rx_metadata;

    BufferPool<std::complex<int16_t>> tx_buffer_pool;
    BufferPool<std::complex<int16_t>> rx_buffer_pool;

    std::mutex rx_mutex;

public:
    UsrpDevice(size_t tx_block_size, size_t rx_block_size);
    ~UsrpDevice();
    std::string get_name() override { return "USRP"; }
    std::string get_model() override;

    /* TX */
    void set_tx_rate(double rate) override;
    void set_tx_bw(double bw) override;
    void set_tx_freq(double freq) override;
    void set_tx_gain(double gain) override;
    void set_tx_port(std::string port) override;
    double get_tx_rate() override;
    double get_tx_bw() override;
    double get_tx_freq() override;
    double get_tx_gain() override;
    std::string get_tx_port() override;
    std::vector<std::string> get_tx_ports_available() override;

    /* RX */
    void set_rx_rate(double rate) override;
    void set_rx_bw(double bw) override;
    void set_rx_freq(double freq) override;
    void set_rx_gain(double gain) override;
    void set_rx_port(std::string port) override;
    double get_rx_rate() override;
    double get_rx_bw() override;
    double get_rx_freq() override;
    double get_rx_gain() override;
    std::string get_rx_port() override;
    std::vector<std::string> get_rx_ports_available() override;
    void tx_init() override;
    void tx_start() override;
    void rx_init() override;
    void rx_start() override;
    int write(IQBuffer* in) override;
    IQBuffer* read() override;
    BufferPool<std::complex<int16_t>>* get_tx_buffer_pool() override;
};

}

#endif