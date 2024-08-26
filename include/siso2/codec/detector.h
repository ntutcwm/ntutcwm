#ifndef SISO2_DETECTOR_H
#define SISO2_DETECTOR_H

#include "siso2/phy/phy.h"
#include <chrono>
#include <memory>
#include <mutex>
#include <complex>
#include <vector>
#include <chrono>
#include <atomic_queue/atomic_queue.h>
#include "siso2/util/atomic_bool_with_wait.h"
#include "siso2/buffer/buffer.h"

namespace SISO2
{

class Detector
{

public:
    struct Stats
    {
        int cfo = 0;
        double rssi = 0;
        double snr = 0;
    };

    phy_t phy;

    std::mutex spectrum_mutex;
    std::vector<std::complex<int32_t>> spectrum;
    
    Stats stats = {};

    Detector(phy_t phy) : phy(phy) {}

    virtual void push(Buffer<std::complex<int16_t>>* buffer) = 0;
    virtual std::vector<std::complex<int32_t>> pop() = 0;
    virtual std::complex<int32_t>* get_spectrum() = 0;
};

}

#endif