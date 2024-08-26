#ifndef SISO2_SYMBOL_MAP_H
#define SISO2_SYMBOL_MAP_H

#include <stdint.h>
#include <stdlib.h>
#include <complex>
#include <vector>

namespace SISO2
{

class SymbolMap
{
public:
    virtual std::vector<std::complex<int16_t>> map(const std::vector<uint8_t>& in) const = 0;
    virtual std::vector<uint8_t> demap(const std::vector<std::complex<float>>& in) const = 0;
    virtual size_t mapped_size(size_t demapped_size) const = 0;
    virtual size_t demapped_size(size_t mapped_size) const = 0;
    virtual ~SymbolMap() = default;
};

}

#endif