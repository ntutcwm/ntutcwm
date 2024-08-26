#ifndef SISO2_ADRV_DEVICE_H
#define SISO2_ADRV_DEVICE_H

#include "siso2/dev/iq-device.h"
#include <stdio.h>
#include <string.h>
#include <iio/iio.h>
#include <stdint.h>
#include <errno.h>
#include <poll.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <mutex>
#include "siso2/buffer/iio-tx-buffer.h"
#include "siso2/buffer/iio-rx-buffer.h"
#include "siso2/buffer/buffer-pool.h"
#include <memory>
#include <fstream>
#include <string>

namespace SISO2
{

typedef struct iio_context iio_context;
typedef struct iio_device iio_device;
typedef struct iio_channel iio_channel;
typedef struct iio_buffer iio_buffer;
typedef struct iio_channels_mask iio_channels_mask;

class AdrvDevice: public IqDevice
{
    const size_t tx_block_size;
    const size_t rx_block_size;

    int channel = 0;
    int select_device=0;// 1 is adrv9364 2 is PlutoSDR

    iio_context*        ctx;

    iio_device*         tx_device;
    iio_channel*        tx_i;
    iio_channel*        tx_q;
    iio_buffer*         tx_buffer;
    iio_channels_mask*  tx_mask;
    int                 tx_buffer_fd;
    std::vector<IIOTXBuffer<std::complex<int16_t>>> tx_blocks;
    std::unique_ptr<BufferPool<std::complex<int16_t>>> tx_buffer_pool;

    iio_device*         rx_device;
    iio_channel*        rx_i;
    iio_channel*        rx_q;
    iio_buffer*         rx_buffer;
    iio_channels_mask*  rx_mask;
    std::vector<IIORXBuffer<std::complex<int16_t>>> rx_blocks;
    std::unique_ptr<BufferPool<std::complex<int16_t>>> rx_buffer_pool;

    iio_channel*        lo_tx_chan;

    bool tx_enabled = false;

    uint32_t* trx_pinctrl = nullptr;
    uint32_t* frontend_pinctrl = nullptr;
    uint32_t* frontend_direction = nullptr;
    uint32_t* frontend_output = nullptr;

    AGCMode agc;

    enum class Dir {
        RX = 0,
        TX = 1,
    };

    enum class IQ {
        I_CHANNEL = 0,
        Q_CHANNEL = 1,
    };

    using enum Dir;
    using enum IQ;

public:
    AdrvDevice(size_t tx_block_size, size_t rx_block_size, size_t tx_interpolation = 1, size_t rx_decimation = 1) :
        tx_block_size(tx_block_size), rx_block_size(rx_block_size)
    {
        this->tx_interpolation = tx_interpolation;
        this->rx_decimation = rx_decimation;
        try {
            ctx = iio_create_context(nullptr, nullptr);
            if (!ctx) throw std::string("iio_create_default_context failed");
            if (iio_context_get_devices_count(ctx) <= 0)
                throw std::string("iio_context_get_devices_count failed");
        }
        catch (std::string what) {
            spdlog::error("{}: {}", what, errno);
            throw what;
        }

        lo_tx_chan = get_lo_chan(TX);

        iio_device_debug_attr_write_longlong(get_ad9361_phy(), "adi,ensm-enable-txnrx-control-enable", 1);
        iio_device_debug_attr_write_longlong(get_ad9361_phy(), "adi,frequency-division-duplex-independent-mode-enable", 1);
        iio_device_attr_write_string(get_ad9361_phy(), "ensm_mode", "pinctrl_fdd_indep");
        int devmem = open("/dev/mem", O_RDWR | O_SYNC);
        void* axi_gpreg_0 = mmap(nullptr, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x4C000000);
        trx_pinctrl = (uint32_t*)(((char*)axi_gpreg_0)+0x0404);
        *trx_pinctrl |= 0x1; // enable RX
        *trx_pinctrl |= 0x2; // enable TX
        if (get_model().find("ADRV9364") != std::string::npos) {
            void* axi_gpreg = mmap(nullptr, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0x41200000);
            frontend_pinctrl = (uint32_t*)(((char*)axi_gpreg)+0x0404);
            *frontend_pinctrl &= ~0x4;
            *frontend_pinctrl &= ~0x8;
            select_device=1;
        }
        else if(get_model().find("PlutoSDR") != std::string::npos){
            void* axi_gpreg = mmap(nullptr, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, devmem, 0xE000A000);
            frontend_pinctrl = (uint32_t*)(((char*)axi_gpreg)+0x0000004C);
            frontend_direction = (uint32_t*)(((char*)axi_gpreg)+0x000002C4);
            frontend_output = (uint32_t*)(((char*)axi_gpreg)+0x000002C8);
            *frontend_output |= 0x8;
            *frontend_direction |= 0x8;
            *frontend_pinctrl &= ~0x8;
            *frontend_output |= 0x100;
            *frontend_direction |= 0x100;
            *frontend_pinctrl |= 0x100;
            select_device=2;
        }
        set_tx_enabled(false);
    }

    ~AdrvDevice() {
        if (tx_buffer) iio_buffer_destroy(tx_buffer);
        if (tx_i) iio_channel_disable(tx_i, tx_mask);
        if (tx_q) iio_channel_disable(tx_q, tx_mask);

        if (rx_buffer) iio_buffer_destroy(rx_buffer);
        if (rx_i) iio_channel_disable(rx_i, rx_mask);
        if (rx_q) iio_channel_disable(rx_q, rx_mask);

        if (ctx) iio_context_destroy(ctx);
    }

    std::string get_name() override { return "ADRV"; }
    std::string get_model() override {
        FILE* fp = fopen("/sys/firmware/devicetree/base/model", "r");
        if (fp == NULL) return "";
        char model[256];
        if (fgets(model, sizeof(model), fp) == NULL) {
            fclose(fp);
            return "";
        }
        fclose(fp);
        return std::string(model);
    }

    void set_xo_correction(long long xo_correction) override {
        iio_device_attr_write_longlong(get_ad9361_phy(), "xo_correction", xo_correction);
    }

    long long get_xo_correction() override{
        long long xo_correction = 0;
        iio_device_attr_read_longlong(get_ad9361_phy(), "xo_correction", &xo_correction);
        return xo_correction;
    }

    /* TX */
    void set_tx_enabled(bool enabled) override {
        if (tx_enabled != enabled) {
            wr_ch_lli(lo_tx_chan, "powerdown", !enabled);
            tx_enabled = enabled;
        }
        if (frontend_pinctrl && select_device==1) {
            if (!enabled) *frontend_pinctrl &= ~0x4;
            else *frontend_pinctrl |= 0x4;
        }
        else if(frontend_pinctrl && select_device==2){
            if(!enabled) *frontend_pinctrl &= ~0x8;
            else *frontend_pinctrl |= 0x8;
        }
        // if (!enabled) *trx_pinctrl &= ~0x2;
        // else *trx_pinctrl |= 0x2;
    }
    void set_tx_rate(double rate) override {
        wr_ch_lli(get_phy_chan(TX, channel), "sampling_frequency", rate);
    }
    void set_tx_bw(double bw) override {
        wr_ch_lli(get_phy_chan(TX, channel), "rf_bandwidth", bw);
    }
    void set_tx_freq(double freq) override {
        wr_ch_lli(get_lo_chan(TX), "frequency", freq);
    }
    void set_tx_gain(double gain) override {
        wr_ch_lli(get_phy_chan(TX, channel), "hardwaregain", gain);
    }
    void set_tx_port(std::string port) override {
        wr_ch_str(get_phy_chan(TX, channel), "rf_port_select", port.c_str());
        iio_device_attr_write_string(get_ad9361_phy(), "calib_mode", "tx_quad");
    }
    double get_tx_rate() override {
        long long rate = 0;
        iio_channel_attr_read_longlong(get_phy_chan(TX, channel), "sampling_frequency", &rate);
        return rate;
    }
    double get_tx_bw() override {
        long long bw = 0;
        iio_channel_attr_read_longlong(get_phy_chan(TX, channel), "rf_bandwidth", &bw);
        return bw;
    }
    double get_tx_freq() override {
        long long freq = 0;
        iio_channel_attr_read_longlong(get_lo_chan(TX), "frequency", &freq);
        return freq;
    }
    double get_tx_gain() override {
        long long gain = 0;
        iio_channel_attr_read_longlong(get_phy_chan(TX, channel), "hardwaregain", &gain);
        return gain;
    }
    std::string get_tx_port() override {
        char text[1024];
        iio_channel_attr_read_raw(get_phy_chan(TX, channel), "rf_port_select", text, sizeof(text));
        return std::string(text);
    }
    std::vector<std::string> get_tx_ports_available() override {
        return {"A", "B"};
    }
    /* RX */
    void set_rx_enabled(bool enabled) override {
        if (frontend_pinctrl && select_device==1) {
            if (!enabled) {
                usleep(500);
                *frontend_pinctrl &= ~0x8;
            }
            else {
                *frontend_pinctrl |= 0x8;
                usleep(500);
            }
        }
        else if (frontend_pinctrl && select_device==2){
            if(!enabled) {
                usleep(500);
                *frontend_pinctrl |= 0x100;
            }
            else {
                *frontend_pinctrl &= ~0x100;
                usleep(500);
            }

        }
    }
    void set_rx_rate(double rate) override {
        wr_ch_lli(get_phy_chan(RX, channel), "sampling_frequency", rate);
    }
    void set_rx_bw(double bw) override {
        wr_ch_lli(get_phy_chan(RX, channel), "rf_bandwidth", bw);
    }
    void set_rx_freq(double freq) override {
        wr_ch_lli(get_lo_chan(RX), "frequency", freq);
    }
    void set_rx_gain(double gain) override {
        wr_ch_lli(get_phy_chan(RX, channel), "hardwaregain", gain);
    }
    void set_rx_agc(AGCMode mode) override {
        agc = mode;
        if (mode == AGCMode::custom) wr_ch_str(get_phy_chan(RX, channel), "gain_control_mode", "manual");
        else wr_ch_str(get_phy_chan(RX, channel), "gain_control_mode", agc_str(mode).c_str());
    }
    void set_rx_port(std::string port) override {
        wr_ch_str(get_phy_chan(RX, channel), "rf_port_select", port.c_str());
        iio_device_attr_write_string(get_ad9361_phy(), "calib_mode", "rf_dc_offs");
    }
    double get_rx_rate() override {
        long long rate = 0;
        iio_channel_attr_read_longlong(get_phy_chan(RX, channel), "sampling_frequency", &rate);
        return rate;
    }
    double get_rx_bw() override {
        long long bw = 0;
        iio_channel_attr_read_longlong(get_phy_chan(RX, channel), "rf_bandwidth", &bw);
        return bw;
    }
    double get_rx_freq() override {
        long long freq = 0;
        iio_channel_attr_read_longlong(get_lo_chan(RX), "frequency", &freq);
        return freq;
    }
    double get_rx_gain() override {
        long long gain = 0;
        iio_channel_attr_read_longlong(get_phy_chan(RX, channel), "hardwaregain", &gain);
        return gain;
    }
    AGCMode get_rx_agc() override {
        if (agc == AGCMode::custom) return agc;
        char text[1024];
        iio_channel_attr_read_raw(get_phy_chan(RX, channel), "gain_control_mode", text, sizeof(text));
        return str_agc(std::string(text));
    }
    std::string get_rx_port() override {
        char text[1024];
        iio_channel_attr_read_raw(get_phy_chan(RX, channel), "rf_port_select", text, sizeof(text));
        return std::string(text);
    }
    std::vector<std::string> get_rx_ports_available() override {
        return {"A_BALANCED", "B_BALANCED"};
    }
    double get_temperature() {
        double tempValue=0;
        char* path= "/sys/bus/i2c/devices/0-0048/hwmon/hwmon0/temp1_input";
        if(access(path,F_OK)!= -1){
            std::ifstream file(path);
            std::string temperature;
            if(file.is_open()){
                std::getline(file,temperature);
                file.close();
            }
            tempValue = std::stod(temperature) / 1000;
        }
        return tempValue;
    }
    void tx_init() override
    {
        tx_device = get_ad9361_stream_dev(TX);
        if (!tx_device) throw std::string("TX get_ad9361_stream_dev failed");

        tx_i = get_ad9361_stream_ch(TX, tx_device, I_CHANNEL);
        if (!tx_i) throw std::string("TX get_ad9361_stream_ch failed");

        tx_q = get_ad9361_stream_ch(TX, tx_device, Q_CHANNEL);
        if (!tx_q) throw std::string("TX get_ad9361_stream_ch failed");

        tx_mask = iio_create_channels_mask(iio_device_get_channels_count(tx_device));
        if (!tx_mask) throw std::string("TX iio_create_channels_mask failed");
    }

    void tx_start() override
    {
        iio_channel_enable(tx_i, tx_mask);
        iio_channel_enable(tx_q, tx_mask);

        tx_buffer = iio_device_create_buffer(tx_device, 0, tx_mask);
        if (!tx_buffer) throw std::string("TX iio_device_create_buffer failed");

        tx_buffer_pool = std::make_unique<BufferPool<std::complex<int16_t>>>((IIOTXBuffer<std::complex<int16_t>>*)nullptr, 32, tx_block_size, tx_buffer);
        
        iio_buffer_enable(tx_buffer);

        // if (iio_buffer_set_blocking_mode(tx_buffer, false)) throw std::string("TX iio_buffer_set_blocking_mode failed");
        // if ((tx_buffer_fd = iio_buffer_get_poll_fd(tx_buffer)) < 0) throw std::string("TX iio_buffer_get_poll_fd failed");
    }
    
    void rx_init() override
    {
        rx_device = get_ad9361_stream_dev(RX);
        if (!rx_device) throw std::string("RX get_ad9361_stream_dev failed");

        rx_i = get_ad9361_stream_ch(RX, rx_device, I_CHANNEL);
        if (!rx_i) throw std::string("RX get_ad9361_stream_ch failed");

        rx_q = get_ad9361_stream_ch(RX, rx_device, Q_CHANNEL);
        if (!rx_q) throw std::string("RX get_ad9361_stream_ch failed");

        rx_mask = iio_create_channels_mask(iio_device_get_channels_count(rx_device));
        if (!rx_mask) throw std::string("RX iio_create_channels_mask failed");
    }

    void rx_start() override
    {
        iio_channel_enable(rx_i, rx_mask);
        iio_channel_enable(rx_q, rx_mask);

        rx_buffer = iio_device_create_buffer(rx_device, 0, rx_mask);
        if (!rx_buffer) throw std::string("RX iio_device_create_buffer failed");

        rx_buffer_pool = std::make_unique<BufferPool<std::complex<int16_t>>>((IIORXBuffer<std::complex<int16_t>>*)nullptr, 32, rx_block_size, rx_buffer);

        iio_buffer_enable(rx_buffer);
    }

    BufferPool<std::complex<int16_t>>* get_tx_buffer_pool() override
    {
        return tx_buffer_pool.get();
    }

    int write(IQBuffer* buffer) override {
        auto size = buffer->level;
        int err = iio_block_enqueue((iio_block*)buffer->impl, size*sizeof(std::complex<int16_t>), /*cyclic*/false);
        // if (err) spdlog::error("TX iio_block_enqueue failed");
        buffer->enqueued = true;
        this->tx_samples += size;
        return size;
    }

    IQBuffer* read() override {
        auto buffer = rx_buffer_pool->get_buffer();
        for (int i = 0; i < buffer->size/this->rx_decimation; i++) buffer->data[i] = buffer->data[i*this->rx_decimation];
        buffer->level = buffer->size/this->rx_decimation;
        this->rx_samples += buffer->size;
        return buffer;
    }

    /* returns ad9361 phy device */
    iio_device* get_ad9361_phy() {
        return iio_context_find_device(ctx, "ad9361-phy");
    }

    /* finds AD9361 streaming IIO devices */
    iio_device* get_ad9361_stream_dev(Dir dir) {
        return iio_context_find_device(ctx, (bool)dir ? "cf-ad9361-dds-core-lpc" : "cf-ad9361-lpc");
    }

    /* finds AD9361 streaming IIO channels */
    iio_channel* get_ad9361_stream_ch(Dir dir, iio_device* dev, IQ channel) {
        auto chn = iio_device_find_channel(dev, get_ch_name("voltage", (int)channel).c_str(), (int)dir);
        if (!chn) chn = iio_device_find_channel(dev, get_ch_name("altvoltage", (int)channel).c_str(), (int)dir);
        return chn;
    }

    /* finds AD9361 phy IIO configuration channel with id chid */
    iio_channel* get_phy_chan(Dir dir, int channel) {
        auto phy_chn = iio_device_find_channel(get_ad9361_phy(), get_ch_name("voltage", channel).c_str(), (int)dir);
        if (!phy_chn) throw std::string("get_phy_chan failed");
        return phy_chn;
    }

    /* finds AD9361 local oscillator IIO configuration channels */
    iio_channel* get_lo_chan(Dir dir) {
        auto lo_chn = iio_device_find_channel(get_ad9361_phy(), get_ch_name("altvoltage", (int)dir).c_str(), true);
        if (!lo_chn) throw std::string("get_lo_chan failed");
        return lo_chn;
    }

    /* write attribute: string */
    void wr_ch_str(iio_channel* chn, const char* what, const char* str) {
        if (iio_channel_attr_write_string(chn, what, str) < 0)
            ; //throw std::string("wr_ch_str failed");
    }

    /* write attribute: long long int */
    void wr_ch_lli(iio_channel *chn, const char* what, long long val) {
        if (iio_channel_attr_write_longlong(chn, what, val) < 0) 
            ; //throw std::string("wr_ch_lli failed");
    }

    /* helper function generating channel names */
    std::string get_ch_name(const char* type, int id) {
        return fmt::format("{}{}", type, id);
    }
};

}

#endif