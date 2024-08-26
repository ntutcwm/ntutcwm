#ifndef SISO2_ENERGY_DISPERSAL_H
#define SISO2_ENERGY_DISPERSAL_H

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#if __cplusplus >= 202002L
#include <span>
#endif
#ifndef __HLS__
#include "siso2/buffer/buffer.h"
#endif

namespace SISO2
{

class EnergyDispersal
{
public:
    uint8_t prbs[256];

    constexpr EnergyDispersal() : prbs()
	{
		uint32_t d_reg = 0xa9; // Register for PRBS
		for (int i = 0; i < sizeof(prbs); i++) {
			uint32_t res = 0;
			uint32_t feedback = 0;
			for (int j = 0; j < 8; j++) {
				feedback = ((d_reg >> (14 - 1)) ^ (d_reg >> (15 - 1))) & 0x1;
				d_reg = ((d_reg << 1) | feedback) & 0x7fff;
				res = (res << 1) | feedback;
			}
			prbs[i] = (uint8_t)res;
		}
	}

#if __cplusplus >= 202002L
    auto operator()(const std::span<uint8_t>& in) const {
        std::vector<uint8_t> out(in.size());
        for (int i = 0; i < in.size(); i++) out[i] = in[i] ^ prbs[i%256];
        return out;
    }
#endif
#ifndef __HLS__
	auto operator()(Buffer<uint8_t>* in) const {
        std::vector<uint8_t> out(in->size);
        for (int i = 0; i < in->size; i++) out[i] = in->data[i] ^ prbs[i%256];
        return out;
    }
#endif
};

}

#endif
