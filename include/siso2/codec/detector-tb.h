#ifndef SISO2_DETECTOR_TB_H
#define SISO2_DETECTOR_TB_H

#include <chrono>
#include "siso2/codec/detector.h"
#include <memory>
#include <mutex>
#include "siso2/util/process.h"
#include "siso2/util/testbench.h"

namespace SISO2
{

class DetectorTB: public Detector
{
    std::unique_ptr<Testbench> ofdm_symb_acq;
    std::unique_ptr<Testbench> fft_forward;
    std::unique_ptr<Testbench> cfo_correction;

    int spectrum_fd;

    std::vector<std::complex<int16_t>> buffer;

public:
    DetectorTB(phy_t phy);
    void push(Buffer<std::complex<int16_t>>* buffer) override;
    std::vector<std::complex<int32_t>> pop() override;
    std::complex<int32_t>* get_spectrum() override;
};

}

#endif