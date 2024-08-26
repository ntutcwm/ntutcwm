#ifndef SISO2_CFO_CORRECTION_H
#define SISO2_CFO_CORRECTION_H

#include <stdlib.h>
#include <complex>
#include <vector>
#include <atomic>
#include "siso2/phy/phy.h"

namespace SISO2
{

class CfoCorrection
{
public:
    phy_t phy;

    std::atomic<int> CFO;
    std::atomic<float> RSSI;
    std::atomic<float> SNR;

    CfoCorrection(phy_t phy): phy(phy) {}
    std::vector<std::complex<int32_t>> cfo_correction(const std::vector<std::complex<int32_t>>& in);
};

}

#endif
