#ifndef SISO2_SIXTEENFOURAPSK_MAP_H
#define SISO2_SIXTEENFOURAPSK_MAP_H

#include <stdint.h>
#include <complex>
#include <vector>
#include "siso2/phy/symbol-mapping/symbol-map.h"
#include "siso2/util/complex.h"

using namespace std::complex_literals;

namespace SISO2
{

class _64ApskMap : public SymbolMap
{
    std::complex<int16_t> _map[64];

public:
    _64ApskMap(double symbol_scale) { generate_map(symbol_scale, _map); }

    static void generate_map(double symbol_scale, std::complex<int16_t>* _map)
    {
        // R = 1, R^2 = 1 (0000, 0001, 0010)
        // 45 deg
        _map[0b000000] = complex_cast<int16_t>(symbol_scale*(0.707106781186548 + 0.707106781186548i));
        _map[0b000001] = complex_cast<int16_t>(symbol_scale*(0.707106781186547 - 0.707106781186548i));
        _map[0b000010] = complex_cast<int16_t>(symbol_scale*(-0.707106781186548 + 0.707106781186548i));
        _map[0b000011] = complex_cast<int16_t>(symbol_scale*(-0.707106781186547 - 0.707106781186548i));
        // 83.57 deg
        _map[0b000100] = complex_cast<int16_t>(symbol_scale*(0.111964476103308 + 0.993712209893243i));
        _map[0b000101] = complex_cast<int16_t>(symbol_scale*(0.111964476103308 - 0.993712209893243i));
        _map[0b000110] = complex_cast<int16_t>(symbol_scale*(-0.111964476103308 + 0.993712209893243i));
        _map[0b000111] = complex_cast<int16_t>(symbol_scale*(-0.111964476103308 - 0.993712209893243i));
        // 6.43 deg
        _map[0b001000] = complex_cast<int16_t>(symbol_scale*(0.993712209893243 + 0.111964476103308i));
        _map[0b001001] = complex_cast<int16_t>(symbol_scale*(0.993712209893243 - 0.111964476103308i));
        _map[0b001010] = complex_cast<int16_t>(symbol_scale*(-0.993712209893243 + 0.111964476103308i));
        _map[0b001011] = complex_cast<int16_t>(symbol_scale*(-0.993712209893243 - 0.111964476103308i));

        // R = 0.14285714285, R^2 = 0.02040816326 (0011)
        _map[0b001100] = complex_cast<int16_t>(symbol_scale*(0.101015254455221 + 0.101015254455221i));
        _map[0b001101] = complex_cast<int16_t>(symbol_scale*(0.101015254455221 - 0.101015254455221i));
        _map[0b001110] = complex_cast<int16_t>(symbol_scale*(-0.101015254455221 + 0.101015254455221i));
        _map[0b001111] = complex_cast<int16_t>(symbol_scale*(-0.101015254455221 - 0.101015254455221i));

        // R = 1, R^2 = 1 (0100, 0101)
        // 57.86 deg
        _map[0b010000] = complex_cast<int16_t>(symbol_scale*(0.532032076515337 + 0.846724199228284i));
        _map[0b010001] = complex_cast<int16_t>(symbol_scale*(0.532032076515336 - 0.846724199228284i));
        _map[0b010010] = complex_cast<int16_t>(symbol_scale*(-0.532032076515337 + 0.846724199228284i));
        _map[0b010011] = complex_cast<int16_t>(symbol_scale*(-0.532032076515337 - 0.846724199228284i));
        // 70.71 deg
        _map[0b010100] = complex_cast<int16_t>(symbol_scale*(0.330279061955167 + 0.943883330308368i));
        _map[0b010101] = complex_cast<int16_t>(symbol_scale*(0.330279061955166 - 0.943883330308368i));
        _map[0b010110] = complex_cast<int16_t>(symbol_scale*(-0.330279061955167 + 0.943883330308368i));
        _map[0b010111] = complex_cast<int16_t>(symbol_scale*(-0.330279061955167 - 0.943883330308368i));

        // R = 0.61428571428, R^2 = 0.37734693876 (0110)
        // 9 deg
        _map[0b011000] = complex_cast<int16_t>(symbol_scale*(0.606722837794156 + 0.0960954570961418i));
        _map[0b011001] = complex_cast<int16_t>(symbol_scale*(0.606722837794156 - 0.0960954570961420i));
        _map[0b011010] = complex_cast<int16_t>(symbol_scale*(-0.606722837794156 + 0.0960954570961419i));
        _map[0b011011] = complex_cast<int16_t>(symbol_scale*(-0.606722837794156 - 0.0960954570961417i));

        // R = 0.34285714285, R^2=0.1175510204 (0111)
        // 15 deg
        _map[0b011100] = complex_cast<int16_t>(symbol_scale*(0.331174569013395 + 0.0887379583208642i));
        _map[0b011101] = complex_cast<int16_t>(symbol_scale*(0.331174569013395 - 0.0887379583208645i));
        _map[0b011110] = complex_cast<int16_t>(symbol_scale*(-0.331174569013395 + 0.0887379583208642i));
        _map[0b011111] = complex_cast<int16_t>(symbol_scale*(-0.331174569013395 - 0.0887379583208643i));

        // R = 1, R^2 = 1 (1000)
        // 32.14 deg
        _map[0b100000] = complex_cast<int16_t>(symbol_scale*(0.846724199228284 + 0.532032076515337i));
        _map[0b100001] = complex_cast<int16_t>(symbol_scale*(0.846724199228284 - 0.532032076515337i));
        _map[0b100010] = complex_cast<int16_t>(symbol_scale*(-0.846724199228284 + 0.532032076515337i));
        _map[0b100011] = complex_cast<int16_t>(symbol_scale*(-0.846724199228284 - 0.532032076515337i));

        // R = 0.61428571428, R^2 = 0.37734693876 (1001)
        // 81 deg
        _map[0b100100] = complex_cast<int16_t>(symbol_scale*(0.0960954570961418 + 0.606722837794156i));
        _map[0b100101] = complex_cast<int16_t>(symbol_scale*(0.0960954570961417 - 0.606722837794156i));
        _map[0b100110] = complex_cast<int16_t>(symbol_scale*(-0.0960954570961418 + 0.606722837794156i));
        _map[0b100111] = complex_cast<int16_t>(symbol_scale*(-0.0960954570961419 - 0.606722837794156i));

        // R = 1, R^2 = 1 (1010)
        // 19.29 deg
        _map[0b101000] = complex_cast<int16_t>(symbol_scale*(0.943883330308368 + 0.330279061955167i));
        _map[0b101001] = complex_cast<int16_t>(symbol_scale*(0.943883330308368 - 0.330279061955167i));
        _map[0b101010] = complex_cast<int16_t>(symbol_scale*(-0.943883330308368 + 0.330279061955167i));
        _map[0b101011] = complex_cast<int16_t>(symbol_scale*(-0.943883330308368 - 0.330279061955167i));

        // R = 0.34285714285, R^2=0.1175510204 (1011)
        // 75 deg
        _map[0b101100] = complex_cast<int16_t>(symbol_scale*(0.0887379583208643 + 0.331174569013395i));
        _map[0b101101] = complex_cast<int16_t>(symbol_scale*(0.0887379583208641 - 0.331174569013395i));
        _map[0b101110] = complex_cast<int16_t>(symbol_scale*(-0.0887379583208642 + 0.331174569013395i));
        _map[0b101111] = complex_cast<int16_t>(symbol_scale*(-0.0887379583208645 - 0.331174569013395i));

        // R = 0.61428571428, R^2 = 0.37734693876 (1100, 1101, 1110)
        // 45 deg
        _map[0b110000] = complex_cast<int16_t>(symbol_scale*(0.434365594157451 + 0.434365594157451i));
        _map[0b110001] = complex_cast<int16_t>(symbol_scale*(0.434365594157450 - 0.434365594157451i));
        _map[0b110010] = complex_cast<int16_t>(symbol_scale*(-0.434365594157451 + 0.434365594157451i));
        _map[0b110011] = complex_cast<int16_t>(symbol_scale*(-0.434365594157451 - 0.434365594157451i));
        // 63 deg
        _map[0b110100] = complex_cast<int16_t>(symbol_scale*(0.278879878411436 + 0.547332579144283i));
        _map[0b110101] = complex_cast<int16_t>(symbol_scale*(0.278879878411436 - 0.547332579144283i));
        _map[0b110110] = complex_cast<int16_t>(symbol_scale*(-0.278879878411436 + 0.547332579144283i));
        _map[0b110111] = complex_cast<int16_t>(symbol_scale*(-0.278879878411436 - 0.547332579144283i));
        // 27 deg
        _map[0b111000] = complex_cast<int16_t>(symbol_scale*(0.547332579144283 + 0.278879878411436i));
        _map[0b111001] = complex_cast<int16_t>(symbol_scale*(0.547332579144283 - 0.278879878411436i));
        _map[0b111010] = complex_cast<int16_t>(symbol_scale*(-0.547332579144283 + 0.278879878411436i));
        _map[0b111011] = complex_cast<int16_t>(symbol_scale*(-0.547332579144283 - 0.278879878411436i));

        // R = 0.34285714285, R^2=0.1175510204 (1111)
        // 45 deg
        _map[0b111100] = complex_cast<int16_t>(symbol_scale*(0.242436610692531 + 0.242436610692531i));
        _map[0b111101] = complex_cast<int16_t>(symbol_scale*(0.242436610692530 - 0.242436610692531i));
        _map[0b111110] = complex_cast<int16_t>(symbol_scale*(-0.242436610692531 + 0.242436610692531i));
        _map[0b111111] = complex_cast<int16_t>(symbol_scale*(-0.242436610692531 - 0.242436610692531i));
    }

    std::vector<std::complex<int16_t>> map(const std::vector<uint8_t>& in) const override
    {
        std::vector<std::complex<int16_t>> out(in.size()*8/6);
        for (int i = 0, o = 0; i < in.size(); i+=3, o+=4) {
            out[o]   = _map[in[i] & 0b111111];
            out[o+1] = _map[in[i] >> 6 | (in[i+1] & 0b1111) << 2];
            out[o+2] = _map[in[i+1] >> 4 | (in[i+2] & 0b11) << 4];
            out[o+3] = _map[in[i+2] >> 2];
        }
        return out;
    }

    uint8_t demap(const std::complex<float>& in) const
    {
        uint8_t symb = 0;
        auto R2 = std::norm(in);
        symb |= (in.imag() < 0) << 0;
        symb |= (in.real() < 0) << 1;
        if (R2 <= 0.05897959183) {
            symb |= 0b0011 << 2;
        }
        else if (R2 <= 0.22903061223) {
            if      (1.73205080757 * fabs(in.imag()) <= fabs(in.real())) symb |= 0b0111 << 2;
            else if (fabs(in.imag()) <= 1.73205080757 * fabs(in.real())) symb |= 0b1111 << 2;
            else                                                         symb |= 0b1011 << 2;
        }
        else if (R2 <= 0.65147959183) {
            if      (3.07768353718 * fabs(in.imag()) <= fabs(in.real())) symb |= 0b0110 << 2;
            else if (1.37638192047 * fabs(in.imag()) <= fabs(in.real())) symb |= 0b1110 << 2;
            else if (fabs(in.imag()) <= 1.37638192047 * fabs(in.real())) symb |= 0b1100 << 2;
            else if (fabs(in.imag()) <= 3.07768353718 * fabs(in.real())) symb |= 0b1101 << 2;
            else                                                         symb |= 0b1001 << 2;
        }
        else {
            if      (4.38128626753 * fabs(in.imag()) <= fabs(in.real())) symb |= 0b0010 << 2;
            else if (2.07652139657 * fabs(in.imag()) <= fabs(in.real())) symb |= 0b1010 << 2;
            else if (1.25396033766 * fabs(in.imag()) <= fabs(in.real())) symb |= 0b1000 << 2;
            else if (fabs(in.imag()) <= 1.25396033766 * fabs(in.real())) symb |= 0b0000 << 2;
            else if (fabs(in.imag()) <= 2.07652139657 * fabs(in.real())) symb |= 0b0100 << 2;
            else if (fabs(in.imag()) <= 4.38128626753 * fabs(in.real())) symb |= 0b0101 << 2;
            else                                                         symb |= 0b0001 << 2;
        }
        return symb;
    }
    
    std::vector<uint8_t> demap(const std::vector<std::complex<float>>& in) const override
    {
        std::vector<uint8_t> out(in.size()*6/8);
        for (int i = 0, o = 0; i < in.size(); i+=4, o+=3) {
            uint8_t symb[4];
            for (int j = 0; j < sizeof(symb); j++) symb[j] = demap(in[i+j]);
            out[o]   = symb[0] | symb[1] << 6;
            out[o+1] = symb[1] >> 2 | symb[2] << 4;
            out[o+2] = symb[2] >> 4 | symb[3] << 2;
        }
        return out;
    }

    size_t mapped_size(size_t demapped_size) const override { return demapped_size*8/6; }
    size_t demapped_size(size_t mapped_size) const override { return mapped_size*6/8; }
};

}

#endif