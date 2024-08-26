#ifndef SISO2_OFDM_CP_INSERTION_H
#define SISO2_OFDM_CP_INSERTION_H

#include <complex>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace SISO2
{

class OfdmCpInsertion
{
    const size_t FFT_LEN;
    const size_t CP_LEN;
    const size_t SUB_CP_NUM;
    const size_t SUB_CP_LEN;
public:
    OfdmCpInsertion(size_t FFT_LEN, size_t CP_LEN, size_t SUB_CP_NUM = 2) : 
        FFT_LEN(FFT_LEN), CP_LEN(CP_LEN), SUB_CP_NUM(SUB_CP_NUM), SUB_CP_LEN(CP_LEN / SUB_CP_NUM) {}
    auto operator()(const std::vector<std::complex<int16_t>>& in) const
    {
        std::vector<std::complex<int16_t>> out(FFT_LEN+CP_LEN);
        for (int i = 0; i < SUB_CP_NUM; i++)
            memcpy((void*)&out[SUB_CP_LEN*i], (const void*)&in[FFT_LEN-SUB_CP_LEN], SUB_CP_LEN * sizeof(std::complex<int16_t>));
        memcpy((void*)&out[CP_LEN], (const void*)in.data(), FFT_LEN * sizeof(std::complex<int16_t>));
        return out;
    }
};

}

#endif

