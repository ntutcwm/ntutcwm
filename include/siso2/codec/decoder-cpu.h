#ifndef SISO2_DECODER_CPU_H
#define SISO2_DECODER_CPU_H

#include "siso2/phy/ofdm/cfo-correction.h"
#include "siso2/phy/ofdm/kiss-fft.h"
#include "siso2/phy/ofdm/reference-signal.h"
#include "siso2/phy/symbol-mapping/qpsk-map.h"
#include "siso2/phy/symbol-mapping/8psk-map.h"
#include "siso2/phy/symbol-mapping/16apsk-map.h"
#include "siso2/phy/symbol-mapping/64apsk-map.h"
#include "siso2/phy/fec/convolutional-codec.h"
#include "siso2/phy/fec/reed-solomon.h"
#include "siso2/phy/block-interleaver.h"
#include "siso2/phy/energy-dispersal.h"
#include "siso2/codec/decoder.h"
#include <memory>
#include <mutex>

namespace SISO2
{

class DecoderCPU: public Decoder
{
    ReferenceSignal reference_signal;
    std::unique_ptr<SymbolMap> symbol_map;
    ConvolutionalCodec convolutional_codec;
    ReedSolomon reed_solomon;
    EnergyDispersal energy_dispersal;

public:
    DecoderCPU(phy_t phy) : Decoder(phy), reference_signal(phy) {
        switch(phy.SYMBOL_MAP) {
            case Constellation::QPSK:
                symbol_map = std::make_unique<QpskMap>(phy.SYMBOL_SCALE);
                break;
            case Constellation::_8PSK:
                symbol_map = std::make_unique<_8PskMap>(phy.SYMBOL_SCALE);
                break;
            case Constellation::_16APSK:
                symbol_map = std::make_unique<_16ApskMap>(phy.SYMBOL_SCALE);
                break;
            case Constellation::_64APSK:
                symbol_map = std::make_unique<_64ApskMap>(phy.SYMBOL_SCALE);
                break;
        }
    }

    std::vector<uint8_t> decode(std::vector<std::complex<int32_t>> detect_out) override;
};

}

#endif