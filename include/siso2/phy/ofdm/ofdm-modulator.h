#ifndef SISO2_OFDM_MODULATOR_H
#define SISO2_OFDM_MODULATOR_H

#include <stdlib.h>
#include <complex>
#include <vector>
#include <stdint.h>
#include "siso2/phy/phy.h"

namespace SISO2
{

class OfdmModulator
{
    phy_t phy;
    int m_sequence_1[255];
    int m_sequence_2[255];
    std::vector<std::complex<float>> pilot_val;

public:
    OfdmModulator(phy_t phy);
    std::vector<std::complex<int16_t>> modulate(const std::vector<std::complex<int16_t>>& in) const;
};

}

#endif
