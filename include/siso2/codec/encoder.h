#ifndef SISO2_ENCODER_H
#define SISO2_ENCODER_H

#include <stdint.h>
#include <vector>
#include <complex>
#include "siso2/phy/phy.h"
#include <span>
#include <queue>
#include <mutex>
#include <spdlog/spdlog.h>
#include "siso2/buffer/buffer.h"

namespace SISO2
{

typedef Buffer<uint8_t> DataBuffer;
typedef Buffer<std::complex<int16_t>> IQBuffer;

class Encoder
{
public:
    struct Stats
    {
        uint64_t energy_dispersal_time_sum = 0;
        uint64_t reed_solomon_time_sum = 0;
        uint64_t interleaver_1_time_sum = 0;
        uint64_t convolutional_encoder_time_sum = 0;
        uint64_t interleaver_2_time_sum = 0;
        uint64_t symbol_map_time_sum = 0;
        uint64_t interleaver_3_time_sum = 0;
        uint64_t ofdm_modulator_time_sum = 0;
        uint64_t fft_time_sum = 0;
        uint64_t ofdm_cp_insertion_time_sum = 0;
        uint64_t total_time_sum = 0;
        uint32_t iterations = 0;
    };

    phy_t phy;
    Stats stats = {};
    Encoder(phy_t phy) : phy(phy) {}
    virtual ~Encoder() = default;

    virtual void encode(DataBuffer* input_buffer, IQBuffer* output_buffer) = 0;
};

}

#endif