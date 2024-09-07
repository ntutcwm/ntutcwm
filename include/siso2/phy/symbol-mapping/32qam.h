#ifndef SISO2_THIRTYTWOQAM_MAP_H
#define SISO2_THIRTYTWOQAM_MAP_H

#include <stdint.h>
#include <complex>
#include <vector>
#include <cmath>
#include "siso2/phy/symbol-mapping/symbol-map.h"
#include "siso2/phy/phy.h"
#include "siso2/util/complex.h"

using namespace std::complex_literals;

namespace SISO2
{

class 32QAM : public SymbolMap
{
    double SYMBOL_SCALE;
public:
    QpskMap(double SYMBOL_SCALE) : SYMBOL_SCALE(SYMBOL_SCALE) {}

    static void generate_map(double symbol_scale, std::complex<int16_t>* _map)
    {
        _map[0b00] = complex_cast<int16_t>(symbol_scale*(1. + 1.i));
        _map[0b01] = complex_cast<int16_t>(symbol_scale*(-1. + 1.i));
        _map[0b10] = complex_cast<int16_t>(symbol_scale*(1. - 1.i));
        _map[0b11] = complex_cast<int16_t>(symbol_scale*(-1. - 1.i));
    };

    std::complex<int16_t> map(const uint8_t& symb) const
    {
        switch (symb) {
            case 0b00: return std::complex<int16_t>{(int16_t)SYMBOL_SCALE, (int16_t)SYMBOL_SCALE};
            case 0b01: return std::complex<int16_t>{(int16_t)-SYMBOL_SCALE, (int16_t)SYMBOL_SCALE};
            case 0b10: return std::complex<int16_t>{(int16_t)SYMBOL_SCALE, (int16_t)-SYMBOL_SCALE};
            case 0b11: return std::complex<int16_t>{(int16_t)-SYMBOL_SCALE, (int16_t)-SYMBOL_SCALE};
        }
        return 0;
    }

    std::vector<std::complex<int16_t>> map(const std::vector<uint8_t>& in) const override
    {
        std::vector<std::complex<int16_t>> out(in.size()*8/2);
        for (int i = 0, o = 0; i < in.size(); i++, o+=4) {
            out[o]   = map(in[i] & 0b11);
            out[o+1] = map((in[i] >> 2) & 0b11);
            out[o+2] = map((in[i] >> 4) & 0b11);
            out[o+3] = map((in[i] >> 6) & 0b11);
        }
        return out;
    }

    uint8_t demap(const std::complex<float>& in) const
    {
        uint8_t symb = 0;
        symb |= (in.real() < 0) << 0;
        symb |= (in.imag() < 0) << 1;
        return symb;
    }

    std::vector<uint8_t> demap(const std::vector<std::complex<float>>& in) const override
    {
        std::vector<uint8_t> out(in.size()*2/8);
        for (int i = 0, o = 0; i < in.size(); i+=4, o++) {
            uint8_t symb[4];
            for (int j = 0; j < sizeof(symb); j++) symb[j] = demap(in[i+j]);
            out[o] = symb[0] | symb[1] << 2 | symb[2] << 4 | symb[3] << 6;
        }
        return out;
    }

    size_t mapped_size(size_t demapped_size) const override { return demapped_size*8/2; }
    size_t demapped_size(size_t mapped_size) const override { return mapped_size*2/8; }
};

}

#endif
