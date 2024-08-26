#ifndef SISO2_SIXTEENAPSK_MAP_H
#define SISO2_SIXTEENAPSK_MAP_H

#include <stdint.h>
#include <complex>
#include <vector>
#include "siso2/phy/symbol-mapping/symbol-map.h"
#include "siso2/util/complex.h"

using namespace std::complex_literals;

namespace SISO2
{

class _16ApskMap : public SymbolMap
{
public:
    double SYMBOL_SCALE = 180;
    _16ApskMap(double SYMBOL_SCALE) : SYMBOL_SCALE(SYMBOL_SCALE) {}

    std::complex<int16_t> map(const uint8_t& symb) const
    {
        switch (symb) {
            // R^2 = 1.28
            case 0b0000: return complex_cast<int16_t>(SYMBOL_SCALE*(0.800 + 0.800i));
            case 0b0001: return complex_cast<int16_t>(SYMBOL_SCALE*(0.800 - 0.800i));
            case 0b0010: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.800 + 0.800i));
            case 0b0011: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.800 - 0.800i));
            case 0b0100: return complex_cast<int16_t>(SYMBOL_SCALE*(1.093 + 0.293i));
            case 0b0101: return complex_cast<int16_t>(SYMBOL_SCALE*(1.093 - 0.293i));
            case 0b0110: return complex_cast<int16_t>(SYMBOL_SCALE*(-1.093 + 0.293i));
            case 0b0111: return complex_cast<int16_t>(SYMBOL_SCALE*(-1.093 - 0.293i));
            case 0b1000: return complex_cast<int16_t>(SYMBOL_SCALE*(0.293 + 1.093i));
            case 0b1001: return complex_cast<int16_t>(SYMBOL_SCALE*(0.293 - 1.093i));
            case 0b1010: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.293 + 1.093i));
            case 0b1011: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.293 - 1.093i));
            // R^2 = 0.1568
            case 0b1100: return complex_cast<int16_t>(SYMBOL_SCALE*(0.280 + 0.280i));
            case 0b1101: return complex_cast<int16_t>(SYMBOL_SCALE*(0.280 - 0.280i));
            case 0b1110: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.280 + 0.280i));
            case 0b1111: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.280 - 0.280i));
        }
        return 0;
    }
    
    std::vector<std::complex<int16_t>> map(const std::vector<uint8_t>& in) const override
    {
        std::vector<std::complex<int16_t>> out(in.size()*8/4);
        for (int i = 0, o = 0; i < in.size(); i++, o+=2) {
            out[o]   = map(in[i] & 0b1111);
            out[o+1] = map(in[i] >> 4);
        }
        return out;
    }

    uint8_t demap(const std::complex<float>& in) const
    {
        uint8_t symb = 0;
        auto R2 = std::norm(in);
        symb |= (in.imag() < 0) << 0;
        symb |= (in.real() < 0) << 1;
        if (R2 <= 0.5832) {
            symb |= 1 << 2;
            symb |= 1 << 3;
        }
        else {
            symb |= (fabs(in.real()) >= 1.732 * fabs(in.imag())) << 2;
            symb |= (fabs(in.imag()) >= 1.732 * fabs(in.real())) << 3;
        }
        return symb;
    }
    
    std::vector<uint8_t> demap(const std::vector<std::complex<float>>& in) const override
    {
        std::vector<uint8_t> out(in.size()*4/8);
        for (int i = 0, o = 0; i < in.size(); i+=2, o++) {
            uint8_t symb[2];
            for (int j = 0; j < sizeof(symb); j++) symb[j] = demap(in[i+j]);
            out[o] = symb[0] | symb[1] << 4;
        }
        return out;
    }

    size_t mapped_size(size_t demapped_size) const override { return demapped_size*8/4; }
    size_t demapped_size(size_t mapped_size) const override { return mapped_size*4/8; }
};

}

#endif