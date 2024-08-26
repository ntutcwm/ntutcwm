#ifndef SISO2_DETECTOR_HLS_H
#define SISO2_DETECTOR_HLS_H

#include "siso2/codec/detector.h"
#include "siso2/dev/xilinx-axidma.h"
#include "siso2/buffer/axidma-buffer.h"
#include <future>

namespace SISO2
{

struct cfo_status_t
{
    int32_t CFO;
    uint32_t total_log;
    uint32_t noise_log;
};

class DetectorHLS: public Detector, protected XilinxAXIDMA
{
    void* ofdm_symb_acq_ctrl;
    void* fft_forward_ctrl;
    void* cfo_correction_ctrl;

    AXIDMABuffer<std::complex<int16_t>> input_buffer;
    AXIDMABuffer<std::complex<int32_t>> output_buffer;
    AXIDMABuffer<std::complex<int32_t>> spectrum_buffer;

    const int spectrum_channel = 5;

    std::future<void> spectrum_task;

public:
    DetectorHLS(phy_t phy);
    ~DetectorHLS();
    std::complex<int32_t>* get_spectrum() override;
    void push(Buffer<std::complex<int16_t>>* buffer) override;
    std::vector<std::complex<int32_t>> pop() override;
};

}

#endif