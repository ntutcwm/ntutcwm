#ifndef SISO2_ENCODER_HLS_H
#define SISO2_ENCODER_HLS_H

#include <complex>
#include <vector>
#include "siso2/dev/xilinx-axidma.h"
#include "siso2/codec/encoder.h"
#include <spdlog/spdlog.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include "siso2/phy/phy.h"
#include <spdlog/spdlog.h>
#include <queue>
#include "siso2/phy/symbol-mapping/64apsk-map.h"
#include "siso2/buffer/axidma-buffer.h"

namespace SISO2
{

class EncoderHLS: public Encoder, protected XilinxAXIDMA
{
    void* encoder_ctrl;

    AXIDMABuffer<std::complex<int16_t>> output_buffer;

public:
    EncoderHLS(phy_t phy, int input_channel = 0, int output_channel = 0, uint32_t base_addr = 0x40210000);
    ~EncoderHLS();
    void encode(DataBuffer* input_buffer, IQBuffer* output_buffer) override;
};

}

#endif

