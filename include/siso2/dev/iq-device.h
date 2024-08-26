#ifndef SISO2_IQ_DEVICE_H
#define SISO2_IQ_DEVICE_H

#include <stdlib.h>
#include <complex>
#include <vector>
#include <string>
#include <span>
#include "siso2/buffer/buffer.h"
#include <queue>
#include <mutex>

namespace SISO2
{

typedef Buffer<std::complex<int16_t>> IQBuffer;

class IqDevice
{
protected:
    size_t tx_interpolation = 1;
    size_t rx_decimation = 1;
public:

    enum class Device
    {
        loopback,
        STDIO,
        UDP,
        USRP,
        ADRV,
    };

    enum class AGCMode
    {
        manual = 0,
        fast_attack = 1,
        slow_attack = 2,
        custom = 3,
    };

    static std::string agc_str(AGCMode mode)
    {
        switch (mode)
        {
            case AGCMode::manual: return "manual";
            case AGCMode::fast_attack: return "fast_attack";
            case AGCMode::slow_attack: return "slow_attack";
            case AGCMode::custom: return "custom";
        }
        return "";
    }

    static AGCMode str_agc(std::string str)
    {
        if (str == "manual") return AGCMode::manual;
        if (str == "fast_attack") return AGCMode::fast_attack;
        if (str == "slow_attack") return AGCMode::slow_attack;
        if (str == "custom") return AGCMode::custom;
        return AGCMode::manual;
    }

    size_t tx_samples = 0;
    size_t rx_samples = 0;
    virtual int write(IQBuffer* in) = 0;
    virtual IQBuffer* read() = 0;
    virtual std::string get_name() = 0;
    virtual std::string get_model() { return ""; }
    virtual void set_xo_correction(long long xo_correction) {}
    virtual long long get_xo_correction() { return 0; }
    virtual double get_temperature() { return 0; }
    /* TX */
    virtual void tx_init() {}
    virtual void tx_start() {}
    virtual void set_tx_enabled(bool enabled) {}
    virtual void set_tx_rate(double rate) {}
    virtual void set_tx_bw(double bw) {}
    virtual void set_tx_freq(double freq) {}
    virtual void set_tx_gain(double gain) {}
    virtual void set_tx_port(std::string port) {}
    virtual void set_tx_interpolation(size_t tx_interpolation) { this->tx_interpolation = tx_interpolation; }
    virtual double get_tx_rate() { return 0; }
    virtual double get_tx_bw() { return 0; }
    virtual double get_tx_freq() { return 0; }
    virtual double get_tx_gain() { return 0; }
    virtual std::string get_tx_port() { return 0; }
    virtual size_t get_tx_interpolation() { return tx_interpolation; }
    virtual std::vector<std::string> get_tx_ports_available() { return {}; }
    virtual BufferPool<std::complex<int16_t>>* get_tx_buffer_pool() { return nullptr; }
    /* RX */
    virtual void rx_init() {}
    virtual void rx_start() {}
    virtual void set_rx_enabled(bool enabled) {}
    virtual void set_rx_rate(double rate) {}
    virtual void set_rx_bw(double bw) {}
    virtual void set_rx_freq(double freq) {}
    virtual void set_rx_gain(double gain) {}
    virtual void set_rx_agc(AGCMode mode) {}
    virtual void set_rx_port(std::string port) {}
    virtual void set_rx_decimation(size_t rx_decimation) { this->rx_decimation = rx_decimation; }
    virtual double get_rx_rate() { return 0; }
    virtual double get_rx_bw() { return 0; }
    virtual double get_rx_freq() { return 0; }
    virtual double get_rx_gain() { return 0; }
    virtual AGCMode get_rx_agc() { return AGCMode::manual; }
    virtual std::string get_rx_port() = 0;
    virtual size_t get_rx_decimation() { return rx_decimation; }
    virtual std::vector<std::string> get_rx_ports_available() { return {}; }
};

}

#endif