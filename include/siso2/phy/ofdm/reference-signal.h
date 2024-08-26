#ifndef SISO2_REFERENCE_SIGNAL_H
#define SISO2_REFERENCE_SIGNAL_H

#include <stdlib.h>
#include <complex>
#include <vector>
#include <atomic>
#include <span>
#include "siso2/phy/phy.h"

namespace SISO2
{

class ReferenceSignal
{
public:
    phy_t phy;

private:
    int m_sequence_1[255];
    int m_sequence_2[255];

    std::vector<std::complex<float>> pilot_val;

public:
    ReferenceSignal(phy_t phy);
    std::vector<std::complex<float>> demodulate(const std::span<std::complex<int32_t>>& in) const;
};

}

#endif
