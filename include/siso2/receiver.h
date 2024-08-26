#ifndef SISO2_RECEIVER_H
#define SISO2_RECEIVER_H

namespace SISO2
{

class Receiver
{
public:
    virtual void enable() = 0;
    virtual bool enabled() = 0;
    virtual void disable() = 0;
    virtual size_t sample_queue_size() = 0;
    virtual size_t recv_queue_size() = 0;
    virtual size_t decode_queue_size() = 0;

    struct Stats
    {
        uint32_t rx_samples = 0;
        float rssi = 0;
        float snr = 0;
        int32_t cfo = 0;
        uint32_t n_rs_error = 0;
        uint32_t n_rs_ok = 0;
        uint32_t header_bytes = 0;
        uint32_t data_bytes = 0;
        uint32_t total_bytes = 0;
        uint64_t sample_queue_size = 0;
        uint32_t decode_queue_size = 0;
        double rx_gain = 0;
    };

    Stats stats = {};
};

};

#endif