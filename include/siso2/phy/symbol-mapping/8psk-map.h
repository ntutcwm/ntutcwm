#ifndef SISO2_EIGHTPSK_MAP_H
#define SISO2_EIGHTPSK_MAP_H

#include <stdint.h>
#include <complex>
#include <vector>
#include "siso2/phy/symbol-mapping/symbol-map.h"
#include "siso2/util/complex.h"

using namespace std::complex_literals;

namespace SISO2
{

class _8PskMap : public SymbolMap
{
public:
    double SYMBOL_SCALE = 203;
    _8PskMap(double SYMBOL_SCALE) : SYMBOL_SCALE(SYMBOL_SCALE) {}

    std::complex<int16_t> map(const uint8_t& symb) const
    {
        switch (symb) {
            case 0b000: return complex_cast<int16_t>(SYMBOL_SCALE*(0.925 + 0.385i));
            case 0b001: return complex_cast<int16_t>(SYMBOL_SCALE*(0.385 + 0.925i));
            case 0b010: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.925 + 0.385i));
            case 0b011: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.385 + 0.925i));
            case 0b100: return complex_cast<int16_t>(SYMBOL_SCALE*(0.925 - 0.385i));
            case 0b101: return complex_cast<int16_t>(SYMBOL_SCALE*(0.385 - 0.925i));
            case 0b110: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.925 - 0.385i));
            case 0b111: return complex_cast<int16_t>(SYMBOL_SCALE*(-0.385 - 0.925i));
        }
        return 0;
    }

    std::vector<std::complex<int16_t>> map(const std::vector<uint8_t>& in) const override
    {
        // 111|111|11 1|111|111|1 11|111|111
        std::vector<std::complex<int16_t>> out(in.size()*8/3);
        for (int i = 0, o = 0; i < in.size(); i+=3, o+=8) {
            out[o]   = map(in[i] & 0b111);
            out[o+1] = map((in[i] >> 3) & 0b111);
            out[o+2] = map(in[i] >> 6 | (in[i+1] & 0b1) << 2);
            out[o+3] = map((in[i+1] >> 1) & 0b111);
            out[o+4] = map((in[i+1] >> 4) & 0b111);
            out[o+5] = map(in[i+1] >> 7 | (in[i+2] & 0b11) << 1);
            out[o+6] = map((in[i+2] >> 2) & 0b111);
            out[o+7] = map(in[i+2] >> 5);
        }
        return out;
    }

    uint8_t demap(const std::complex<float>& in) const
    {
        uint8_t symb = 0;
        symb |= (fabs(in.real()) < fabs(in.imag())) << 0;
        symb |= (in.real() < 0) << 1;
        symb |= (in.imag() < 0) << 2;
        return symb;
    }

    std::vector<uint8_t> demap(const std::vector<std::complex<float>>& in) const override
    {
        std::vector<uint8_t> out(in.size()*3/8);
        for (int i = 0, o = 0; i < in.size(); i+=8, o+=3) {
            uint8_t symb[8];
            for (int j = 0; j < sizeof(symb); j++) symb[j] = demap(in[i+j]);
            out[o]   = symb[0] | symb[1] << 3 | ((symb[2] << 6) & 0b11000000);
            out[o+1] = symb[2] >> 2 | symb[3] << 1 | symb[4] << 4 | ((symb[5] << 7) & 0b10000000);
            out[o+2] = symb[5] >> 1 | symb[6] << 2 | symb[7] << 5;
        }
        return out;
    }

    size_t mapped_size(size_t demapped_size) const override { return demapped_size*8/3; }
    size_t demapped_size(size_t mapped_size) const override { return mapped_size*3/8; }
};

}

#endif