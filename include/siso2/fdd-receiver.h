#ifndef SISO2_FDD_RECEIVER_H
#define SISO2_FDD_RECEIVER_H

#include "siso2/net/packet.h"
#include "siso2/dev/stdio-device.h"
#include <deque>
#include <mutex>
#include <atomic>
#include <memory>
#include <algorithm>
#include <iterator>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <chrono>
#include <functional>
#include "siso2/util/atomic_bool_with_wait.h"
#include "siso2/fdd-transmitter.h"
#include "siso2/codec/detector.h"
#include "siso2/codec/decoder.h"
#include "siso2/net/data-sink.h"
#include "siso2/util/thread.h"
#include <future>
#include "siso2/receiver.h"
#include <spdlog/spdlog.h>
#include "siso2/iq-recorder.h"

namespace SISO2
{

using namespace std::chrono_literals;

class FDDReceiver: public Receiver
{   
    std::shared_ptr<IqDevice> iq_device;
    std::future<void> receive_task, acq_task;
    std::vector<std::future<void>> decode_tasks;

public:
    std::shared_ptr<Detector> detector;
    std::vector<std::shared_ptr<Decoder>> decoders;
    std::shared_ptr<DataSink> data_sink;

    atomic_bool_with_wait _enabled {false};
    atomic_bool_with_wait receive_signal {false};

    FDDReceiver(std::shared_ptr<IqDevice> iq_device, std::shared_ptr<Detector> detector, std::vector<std::shared_ptr<Decoder>> decoders, std::shared_ptr<DataSink> data_sink)
        : iq_device(iq_device), detector(detector), decoders(decoders), data_sink(data_sink)
    {
        receive_task = std::async([this](){ 
            setup_thread(99, "RX/Receive");
            receive();
        });
        for (int i = 0; i < decoders.size(); i++) decode_tasks.emplace_back(std::async([this,i](){
            setup_thread(96, "RX/Decode");
            decode(this->decoders[i]);
        }));
    }

    void enable() override {
        this->iq_device->set_rx_enabled(true);
        _enabled.test_and_set();
        _enabled.notify_all();
    }

    void disable() override {
        this->iq_device->set_rx_enabled(false);
        _enabled.clear();
        _enabled.notify_all();
    }

    bool enabled() override {
        return _enabled.test();
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
                auto dec = decoder->decode(cfo_out);
                if (dec.size()) data_sink->unpack(dec);
            }
        }
    }
};

}

#endif