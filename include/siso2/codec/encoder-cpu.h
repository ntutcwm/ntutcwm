#ifndef SISO2_ENCODER_CPU_H
#define SISO2_ENCODER_CPU_H

#include "siso2/codec/encoder.h"
#include "siso2/phy/energy-dispersal.h"
#include "siso2/phy/block-interleaver.h"
#include "siso2/phy/fec/reed-solomon.h"
#include "siso2/phy/fec/convolutional-codec.h"
#include "siso2/phy/symbol-mapping/qpsk-map.h"
#include "siso2/phy/symbol-mapping/8psk-map.h"
#include "siso2/phy/symbol-mapping/16apsk-map.h"
#include "siso2/phy/symbol-mapping/64apsk-map.h"
#include "siso2/phy/ofdm/ofdm-modulator.h"
#include "siso2/phy/ofdm/kiss-fft.h"
#include "siso2/phy/ofdm/ofdm-cp-insertion.h"
#include <chrono>
#include <memory>
#include "siso2/buffer/vector-buffer.h"

namespace SISO2
{

class EncoderCPU : public Encoder
{
    EnergyDispersal energy_dispersal;
    ReedSolomon reed_solomon;
    ConvolutionalCodec convolutional_codec;
    std::unique_ptr<SymbolMap> symbol_map;
    OfdmModulator ofdm_modulator;
    KissFFT fft;
    OfdmCpInsertion ofdm_cp_insertion;

public:
    EncoderCPU(phy_t phy);
    void time_checkpoint(uint64_t& stat, std::chrono::time_point<std::chrono::system_clock>& last_time);
    bool detect_overflow(const std::vector<std::complex<int16_t>>& packet);
    void encode(DataBuffer* input_buffer, IQBuffer* output_buffer) override;
};

}

#endif