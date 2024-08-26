#ifndef SISO2_DECODER_H
#define SISO2_DECODER_H

#include "siso2/phy/phy.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <complex>
#include <vector>
#include <chrono>
#include "siso2/dev/iq-device.h"
#include <atomic_queue/atomic_queue.h>
#include "siso2/util/atomic_bool_with_wait.h"

namespace SISO2
{

class Decoder
{
protected:
    std::vector<std::complex<float>> constellation;

    void time_checkpoint(uint64_t& stat, std::chrono::time_point<std::chrono::system_clock>& last_time)
    {
        using namespace std::chrono_literals;
        auto current_time = std::chrono::system_clock::now();
        stat += (current_time - last_time)/1ns;
        last_time = current_time;
    }

public:
    struct Stats
    {
        size_t n_rs_error = 0;
        size_t n_rs_ok = 0;
    };

    phy_t phy;
    
    Stats stats = {};
    std::chrono::time_point<std::chrono::system_clock> last_time;

    atomic_queue::AtomicQueue<Buffer<std::complex<int16_t>>*, 1024> buffer_queue;
    atomic_bool_with_wait acq_signal {false};

    bool output_constellation = false;

    Decoder(phy_t phy) : phy(phy) {}

    virtual std::vector<uint8_t> decode(std::vector<std::complex<int32_t>> detect_out) = 0;
    std::vector<std::complex<float>> get_constellation() { return constellation; }
};

}

#endif