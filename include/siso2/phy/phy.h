#ifndef PHY_H
#define PHY_H

#include <stdint.h>

static constexpr uint16_t MAX_FFT_LEN = 2048;
static constexpr uint16_t MAX_CP_LEN = 256;

enum class Constellation {
    QPSK = 2,
    _8PSK = 3,
    _16APSK = 4,
    _64APSK = 6,
};

struct phy_t
{
    uint32_t FFT_LEN;
    uint32_t CP_LEN;
    uint32_t PILOT_TONE_NUM;
    uint32_t TONES_PER_PILOT;
    uint32_t PILOT_MAGNITUDE;
    uint32_t SYMBOL_SCALE;
    Constellation SYMBOL_MAP;

    constexpr uint32_t DATA_TONES_PER_PILOT() const {
        return TONES_PER_PILOT - 1;
    }

    constexpr uint32_t TOTAL_TONE_NUM() const {
        return PILOT_TONE_NUM*TONES_PER_PILOT - TONES_PER_PILOT + 1;
    }

    constexpr uint32_t CFO_MAX() const {
        return FFT_LEN - TOTAL_TONE_NUM() + 1;
    }

    constexpr uint32_t ZEROS_ON_RIGHT() const {
        return (FFT_LEN - TOTAL_TONE_NUM()) / 2;
    }

    constexpr uint32_t ZEROS_ON_LEFT() const {
        return FFT_LEN - TOTAL_TONE_NUM() - ZEROS_ON_RIGHT();
    }

    constexpr uint32_t DATA_TONE_NUM() const {
        return TOTAL_TONE_NUM() - PILOT_TONE_NUM;
    }

    constexpr uint32_t SYMBOL_SIZE() const {
        return DATA_TONE_NUM() * static_cast<int>(SYMBOL_MAP) / 8/*bits->bytes*/;
    }

    constexpr uint32_t RS_ENCODED_SIZE() const {
        return SYMBOL_SIZE() / 2;
    }

    constexpr uint32_t PACKET_SIZE() const {
        return RS_ENCODED_SIZE()-(1+(RS_ENCODED_SIZE()-1)/255)*16;
    }
};

static constexpr phy_t default_phy = {
	.FFT_LEN = 2048,
	.CP_LEN = 256,
	.PILOT_TONE_NUM = 273,
	.TONES_PER_PILOT = 7,
	.PILOT_MAGNITUDE = 9216,
    .SYMBOL_SCALE = 4876,
    .SYMBOL_MAP = Constellation::_64APSK,
};

static constexpr phy_t default_downlink_phy = {
	.FFT_LEN = 2048,
	.CP_LEN = 256,
	.PILOT_TONE_NUM = 273,
	.TONES_PER_PILOT = 7,
	.PILOT_MAGNITUDE = 9216,
    .SYMBOL_SCALE = 4876,
    .SYMBOL_MAP = Constellation::_64APSK,
};

static constexpr phy_t default_uplink_phy = {
	.FFT_LEN = 2048,
	.CP_LEN = 256,
	.PILOT_TONE_NUM = 273,
	.TONES_PER_PILOT = 7,
	.PILOT_MAGNITUDE = 9216,
    .SYMBOL_SCALE = 4876,
    .SYMBOL_MAP = Constellation::QPSK,
};

#endif