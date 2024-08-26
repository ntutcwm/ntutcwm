#ifndef SISO2_FDD_TRANSCEIVER_H
#define SISO2_FDD_TRANSCEIVER_H

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
#include <set>
#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std::chrono_literals;

namespace SISO2
{

class FDDTransceiver: public Transmitter, public Receiver
{
    std::shared_ptr<IqDevice> iq_device;
    phy_t phy;

    /* TX */
    atomic_bool_with_wait encode_request {false};

    std::atomic<double> duty_cycle = 1;

    std::vector<std::future<void>> encoder_tasks;
    std::future<void> transfer_task;

    size_t burst_size;

    std::atomic<size_t> n_cached_ = 0;

    atomic_bool_with_wait tx_signal;

    /* IP */
    std::vector<std::future<void>> ip_tasks;
    /* RX */
    std::future<void> receive_task, acq_task;
    std::vector<std::future<void>> decode_tasks;

public:
    /* TX */
    size_t n_cached() override { return n_cached_; }

    atomic_bool_with_wait _enabled;

    std::shared_ptr<DataSource> data_source;
    std::vector<std::shared_ptr<Encoder>> encoders;

    std::shared_ptr<Cache> date_cache;

    /* RX */
    std::shared_ptr<Detector> detector;
    std::vector<std::shared_ptr<Decoder>> decoders;
    std::shared_ptr<DataSink> data_sink;

    atomic_bool_with_wait receive_signal {false};

    enum class Mode {
        Base,
        Term,
    };


#ifdef SISO2_ADRV
    Mode mode = Mode::Base;
#else
    Mode mode = Mode::Term;
#endif

    /* for terminal */
    uint8_t user_id;
    atomic_bool_with_wait uplink_flag {false};
    /* for base */
    std::mutex user_mutex;
    std::vector<uint8_t> users = { 0xFF };
    int ma_idx = 0;

    bool associated;

    /* Transceiver */
    FDDTransceiver(Mode mode, size_t burst_size, std::shared_ptr<IqDevice> iq_device, std::vector<std::shared_ptr<Encoder>> encoders, std::shared_ptr<DataSource> data_source
        , std::shared_ptr<Detector> detector, std::vector<std::shared_ptr<Decoder>> decoders, std::shared_ptr<DataSink> data_sink, uint8_t user_id)
        : mode(mode), burst_size(burst_size) ,iq_device(iq_device), phy(encoders[0]->phy)
        , encoders(encoders), data_source(data_source)
        , detector(detector), decoders(decoders), data_sink(data_sink)
        , user_id(user_id) {

        date_cache = std::make_shared<SDCache>("date_cache.iq", phy.FFT_LEN+phy.CP_LEN, 1024);
        //date_cache = std::make_shared<MemCache>(phy.FFT_LEN+phy.CP_LEN, 4096);

        IQBuffer* filler = this->iq_device->get_tx_buffer_pool()->get_buffer();
        for (int i = 0; i < filler->size; i++) filler->data[i] = 0;

        transfer_task = std::async([this,filler](){
            setup_thread(98, "TX/Transfer");
            while (true) {
                if (!_enabled.test()) _enabled.wait(false);
                // spdlog::debug("transfer");
                if (this->mode == Mode::Term) {
                    if (!uplink_flag.test()) this->iq_device->set_tx_enabled(false);
                    uplink_flag.wait(false);
                    uplink_flag.clear();
                    this->iq_device->set_tx_enabled(true);
                    spdlog::debug("[Term] uplink");
                    if (!associated) send_data_burst(10);
                    else send_data_burst(this->burst_size);
                }
                else if (this->mode == Mode::Base) {
                    send_data_burst(this->burst_size);
                }
            }
        });

        for (int n = 0; n < encoders.size(); n++)
            encoder_tasks.emplace_back(std::async([this,n]{
                setup_thread(9, "TX/Encode");
                while (true) {
                    encode_data_packet(n);
                }
            }));

        receive_task = std::async([this](){ 
            setup_thread(99, "RX/Receive");
            receive();
        });
        for (int i = 0; i < decoders.size(); i++) decode_tasks.emplace_back(std::async([this,i](){
            setup_thread(96, "RX/Decode");
            decode(this->decoders[i]);
        }));
    }

    void send_data_burst(int burst_size) {
        static int duty_count = 0;
        auto burst = this->iq_device->get_tx_buffer_pool()->get_buffer();
        burst->level = 0;
        for (int i = 0; i < burst_size; i++) {
            if (!date_cache->empty() && duty_count < duty_cycle*100) {
                auto encoded_packet = date_cache->pop();
                tx_signal.test_and_set();
                tx_signal.notify_one();
                memcpy(burst->data+i*(phy.FFT_LEN+phy.CP_LEN), encoded_packet->data, (phy.FFT_LEN+phy.CP_LEN)*sizeof(std::complex<int16_t>));
                burst->level += (phy.FFT_LEN+phy.CP_LEN)*sizeof(std::complex<int16_t>);
                Transmitter::stats.n_packets += 1;
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
        Transmitter::stats.n_bursts++;
        burst->release();
    }

    void encode_data_packet(int n) {
        static int packet_count = 0;
        auto encoded_packet = date_cache->get_buffer();
        auto data_packet = this->data_source->get_data();
        auto section_header = (SectionHeader*)(data_packet->data + sizeof(PacketHeader));
        if (mode == Mode::Base) {
            std::unique_lock lk{user_mutex};
            section_header->channel = users[ma_idx];
            packet_count++;
            if (packet_count == 100) {
                packet_count = 0;
                ma_idx++;
                if (ma_idx >= users.size()) ma_idx = 0;
            }
        }
        else if (mode == Mode::Term) {
            section_header->channel = user_id;
        }
        // spdlog::debug("encode");
        this->encoders[n]->encode(data_packet, encoded_packet);
        data_packet->release();
        this->data_source->stats.n_packets++;
        if (date_cache->full()) tx_signal.wait(false);
        tx_signal.clear();
        date_cache->push(encoded_packet);
        // n_cached_++;
    }

    void enable() override {
        this->iq_device->set_tx_enabled(true);
        this->iq_device->set_rx_enabled(true);
        _enabled.test_and_set();
        _enabled.notify_all();
    }

    bool enabled() override {
        return _enabled.test();
    }

    void disable() override {
        _enabled.clear();
        _enabled.notify_all();
    }

    size_t tx_queue_size() override {
        return date_cache->size();
    }

    void set_duty_cycle(float duty_cycle) override {
        this->duty_cycle = duty_cycle;
        spdlog::info("Duty cycle: {}", duty_cycle);
    }

    float get_duty_cycle() override {
        return this->duty_cycle;
    }

    size_t recv_queue_size() override {
        return 0; //buffer_queue.was_size();
    }

    size_t decode_queue_size() override {
        return decoders[0]->buffer_queue.was_size();
    }

    size_t sample_queue_size() override {
        return 0;
    }

    void ip_packet()
    {

    }

    void receive()
    {
        while (true) {
            if (!_enabled.test()) _enabled.wait(false);
            auto buffer = iq_device->read();
            detector->push(buffer);
            receive_signal.test_and_set();
            receive_signal.notify_one();
        }
    }

    void decode(std::shared_ptr<Decoder> decoder)
    {
        while (true) {
            if (!_enabled.test()) _enabled.wait(false);
            // while (decoder->buffer_queue.was_empty()) decoder->acq_signal.wait(false);
            // decoder->acq_signal.clear();
            auto cfo_out = detector->pop();
            if (cfo_out.size()) {
                auto packet = decoder->decode(cfo_out);
                if (packet.size()) {
                    auto packet_header = (PacketHeader*)packet.data();
                    auto section_header = (SectionHeader*)(packet.data() + sizeof(PacketHeader));
                    uint8_t channel = section_header->channel;
                    data_sink->unpack(packet);
                    if (mode == Mode::Term)
                    {
                        spdlog::debug("[Term] channel {}", channel);
                        if (channel == 0xFF) {
                            // Terminal: prepare for association
                            if (!associated) {
                                uplink_flag.test_and_set();
                                uplink_flag.notify_all();
                            } 
                        }
                        else if (channel == user_id) {
                            // Terminal: prepare for uplink
                            if (!associated) {
                                spdlog::info("[Term {}] associated", user_id);
                                associated = true;
                            }
                            else spdlog::debug("[Term {}] uplink", user_id);
                            uplink_flag.test_and_set();
                            uplink_flag.notify_all();
                        }  
                    }
                    else if (mode == Mode::Base)
                    {
                        std::unique_lock lk{user_mutex};
                        // Handle association
                        spdlog::debug("[Base] channel {}", channel);
                        //for (auto& user: users) spdlog::info("[Base] user: {}", user);
                        if (std::find(users.begin(), users.end(), channel) == users.end()) {
                            spdlog::info("[Base] user {} associated", channel);
                            users.push_back(channel);
                        }
                    }
                }
            }
        }
    }
};

}

#endif