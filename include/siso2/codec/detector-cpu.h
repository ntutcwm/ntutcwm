#ifndef SISO2_DETECTOR_CPU_H
#define SISO2_DETECTOR_CPU_H

#include "siso2/codec/detector.h"
#include "siso2/phy/ofdm/ofdm-symb-acquisition-cpu.h"
#include "siso2/phy/ofdm/kiss-fft.h"
#include "siso2/phy/ofdm/cfo-correction.h"
#include "siso2/buffer/buffer.h"
#include "siso2/util/atomic_bool_with_wait.h"
#include <atomic_queue/atomic_queue.h>
#include <future>

namespace SISO2
{

class DetectorCPU: public Detector
{
    atomic_queue::AtomicQueue<Buffer<std::complex<int16_t>>*, 32768> buffer_queue;
    OfdmSymbAcquisitionCPU ofdm_symb_acquisition;
    KissFFT fft;
    CfoCorrection cfo_correction;
    std::future<void> acq_task;

public:
    DetectorCPU(phy_t phy);
    void push(Buffer<std::complex<int16_t>>* buffer) override;
    std::vector<std::complex<int32_t>> pop() override;
    std::complex<int32_t>* get_spectrum() override { return spectrum.data(); }
};

}

#endif