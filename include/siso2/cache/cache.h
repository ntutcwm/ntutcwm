#ifndef SISO2_CACHE_H
#define SISO2_CACHE_H

#include "siso2/buffer/buffer.h"
#include <stdint.h>
#include <complex>

namespace SISO2
{

class Cache
{
public:
    virtual void push(Buffer<std::complex<int16_t>>* buffer) = 0;
    virtual Buffer<std::complex<int16_t>>* get_buffer() = 0;
    virtual Buffer<std::complex<int16_t>>* pop() = 0;
    virtual bool full() { return true; }
    virtual bool empty() { return true; }
    virtual size_t size() { return 0; }
};

}

#endif