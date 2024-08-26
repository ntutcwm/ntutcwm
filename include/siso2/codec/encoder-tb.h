#ifndef SISO2_ENCODER_TB_H
#define SISO2_ENCODER_TB_H

#include "siso2/codec/encoder.h"
#include "siso2/util/testbench.h"
#include <chrono>
#include <memory>
#include "siso2/buffer/vector-buffer.h"

namespace SISO2
{

class EncoderTB : public Encoder
{
    std::unique_ptr<Testbench> encoder;

public:
    EncoderTB(phy_t phy);
    void encode(DataBuffer* input_buffer, IQBuffer* output_buffer) override;
};

}

#endif