#ifndef SISO2_ENCODER_TB2_H
#define SISO2_ENCODER_TB2_H

#include "siso2/codec/encoder.h"
#include "siso2/util/testbench.h"
#include <chrono>
#include <memory>
#include "siso2/buffer/vector-buffer.h"

namespace SISO2
{

class EncoderTB2 : public Encoder
{
    std::unique_ptr<Testbench> energy_dispersal;
    std::unique_ptr<Testbench> reed_solomon_encoder;
    std::unique_ptr<Testbench> convolutional_encoder;
    std::unique_ptr<Testbench> symbol_mapper;
    std::unique_ptr<Testbench> ofdm_modulator;
    std::unique_ptr<Testbench> fft_reverse;
    std::unique_ptr<Testbench> ofdm_cp_insertion;

public:
    EncoderTB2(phy_t phy);
    void encode(DataBuffer* input_buffer, IQBuffer* output_buffer) override;
};

}

#endif