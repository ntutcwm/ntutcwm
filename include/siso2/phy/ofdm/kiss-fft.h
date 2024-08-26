#ifndef SISO2_KISS_FFT_H
#define SISO2_KISS_FFT_H

#define FIXED_POINT 32
#include <kiss_fft.h>
#include <stdint.h>
#include <stdlib.h>
#include <complex>
#include <vector>
#include "siso2/util/complex.h"
#include "siso2/buffer/buffer.h"

namespace SISO2
{

class KissFFT
{
    const size_t N;
    const bool forward;
    kiss_fft_cfg cfg;

public:
    KissFFT(size_t N, bool forward): N(N), forward(forward)
    {
        cfg = kiss_fft_alloc(N, !forward, nullptr, nullptr);
    }

    ~KissFFT()
    {
        kiss_fft_free(cfg);
    }

    auto fft_reverse(const std::vector<std::complex<int16_t>>& in)
    {
        std::vector<std::complex<int32_t>> in32(N);
        std::vector<std::complex<int32_t>> out32(N);
        std::vector<std::complex<int16_t>> out(N);
        for (int i = 0; i < N; i++) in32[i] = in[i];
        kiss_fft(cfg, (kiss_fft_cpx*)in32.data(), (kiss_fft_cpx*)out32.data());
        for (int i = 0; i < N; i++) {
            int sign = i & 1 ? -1 : 1;
            out[i] = sign * out32[i] / (int32_t)512;
        }
        return out;
    }

    auto fft_forward(Buffer<std::complex<int16_t>>* in)
    {
        std::vector<std::complex<int32_t>> out(N);
        std::vector<std::complex<int32_t>> in32(N);
        for (int i = 0; i < N; i++) in32[i] = in->data[i];
        kiss_fft(cfg, (kiss_fft_cpx*)in32.data(), (kiss_fft_cpx*)out.data());
        return out;
    }
};

}

#endif