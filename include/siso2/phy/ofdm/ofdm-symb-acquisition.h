#ifndef SISO2_OFDM_SYMB_ACQUISITION_H
#define SISO2_OFDM_SYMB_ACQUISITION_H

#include "siso2/buffer/buffer.h"

namespace SISO2
{

using namespace std::complex_literals;

class OfdmSymbAcquisition
{
public:
    virtual void push(Buffer<std::complex<int16_t>>* in) = 0;
    virtual Buffer<std::complex<int16_t>>* pop() = 0;
};

}

#endif

