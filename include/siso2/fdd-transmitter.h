#ifndef SISO2_FDD_TRANSMITTER_H
#define SISO2_FDD_TRANSMITTER_H

#include "siso2/net/packet.h"
#include "siso2/net/data-source.h"
#include "siso2/dev/stdio-device.h"
#include "siso2/codec/encoder.h"
#include <deque>
#include <mutex>
#include <atomic>
#include <memory>
#include <pthread.h>
#include <chrono>
#include "siso2/util/atomic_bool_with_wait.h"
#include "siso2/util/thread.h"
#include <future>
#include "siso2/transmitter.h"
#include <atomic_queue/atomic_queue.h>
#include "siso2/buffer/buffer-pool.h"
#include <unistd.h>
#include <fcntl.h>
#ifdef SISO2_ADRV
#include "siso2/buffer/axidma-buffer.h"
#else
#include "siso2/buffer/vector-buffer.h"
#endif
#include "siso2/cache/cache.h"
#include "siso2/cache/sd-cache.h"
#include "siso2/cache/mem-cache.h"

using namespace std::chrono_literals;

namespace SISO2
{

class FDDTransmitter: public Transmitter
{
    std::shared_ptr<IqDevice> iq_device;
    atomic_bool_with_wait encode_request {false};

    std::atomic<double> duty_cycle = 1;

    std::vector<std::future<void>> encoder_tasks;
    std::future<void> transfer_task;

    phy_t phy;

    size_t burst_size;

    std::atomic<size_t> n_cached_ = 0;

    atomic_bool_with_wait tx_signal;

public:
    size_t n_cached() override { return n_cached_; }

    atomic_bool_with_wait _enabled;

    std::shared_ptr<DataSource> data_source;
    std::vector<std::shared_ptr<Encoder>> encoders;

    std::shared_ptr<Cache> cache;

    FDDTransmitter(size_t burst_size, std::shared_ptr<IqDevice> iq_device, std::vector<std::shared_ptr<Encoder>> encoders, std::shared_ptr<DataSource> data_source)
        : burst_size(burst_size), iq_device(iq_device), encoders(encoders), data_source(data_source), phy(encoders[0]->phy) {

        cache = std::make_shared<SDCache>("cache.iq", phy.FFT_LEN+phy.CP_LEN, 65536);
        //cache = std::make_shared<MemCache>(phy.FFT_LEN+phy.CP_LEN, 4096);

        IQBuffer* filler = this->iq_device->get_tx_buffer_pool()->get_buffer();
        for (int i = 0; i < filler->size; i++) filler->data[i] = 0;

        transfer_task = std::async([this,filler](){
            setup_thread(98, "TX/Transfer");
            int duty_count = 0;
            while (true) {
                if (!_enabled.test()) _enabled.wait(false);
                // spdlog::debug("transfer");

                auto burst = this->iq_device->get_tx_buffer_pool()->get_buffer();
                burst->level = 0;
                for (int i = 0; i < this->burst_size; i++) {
                    if (!cache->empty() && duty_count < duty_cycle*100) {
                        auto encoded_packet = cache->pop();
                        tx_signal.test_and_set();
                        tx_signal.notify_one();
                        memcpy(burst->data+i*(phy.FFT_LEN+phy.CP_LEN), encoded_packet->data, (phy.FFT_LEN+phy.CP_LEN)*sizeof(std::complex<int16_t>));
                        burst->level += (phy.FFT_LEN+phy.CP_LEN)*sizeof(std::complex<int16_t>);
                        stats.n_packets += this->burst_size;
                        encoded_packet->release();
                    }
                    else {
                        memset(burst->data+i*(phy.FFT_LEN+phy.CP_LEN), 0, (phy.FFT_LEN+phy.CP_LEN)*sizeof(std::complex<int16_t>));
                        burst->level += (phy.FFT_LEN+phy.CP_LEN)*sizeof(std::complex<int16_t>);
                    }
                    duty_count++;
                    if (duty_count == 100) duty_count = 0;
                }
                this->iq_device->write(burst);
                stats.n_bursts++;
                burst->release();
            }
        });

        for (int n = 0; n < encoders.size(); n++)
            encoder_tasks.emplace_back(std::async([this,n]{
                setup_thread(9, "TX/Encode");
                while (true) {
                    auto encoded_packet = cache->get_buffer();
                    auto data_packet = this->data_source->get_data();
                    // spdlog::debug("encode");
                    this->encoders[n]->encode(data_packet, encoded_packet);
                    data_packet->release();
                    this->data_source->stats.n_packets++;
                    if (cache->full()) tx_signal.wait(false);
                    tx_signal.clear();
                    cache->push(encoded_packet);
                    // n_cached_++;
                }
            }));
    }

    bool enabled() override {
        return _enabled.test();
    }

    void disable() override {
        this->iq_device->set_rx_enabled(false);
        _enabled.clear();
        _enabled.notify_all();
    }

    size_t tx_queue_size() override {
        return cache->size();
    }

    void set_duty_cycle(float duty_cycle) override {
        this->duty_cycle = duty_cycle;
        spdlog::info("Duty cycle: {}", duty_cycle);
    }

    float get_duty_cycle() override {
        return this->duty_cycle;
    }

    void enable() override {
        this->iq_device->set_tx_enabled(true);
        _enabled.test_and_set();
        _enabled.notify_all();
    }
};

}

#endif