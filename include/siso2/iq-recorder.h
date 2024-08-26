#ifndef SISO2_IQ_RECORDER_H
#define SISO2_IQ_RECORDER_H

#include <iostream>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include "siso2/util/atomic_bool_with_wait.h"

namespace SISO2
{

class IQRecorder
{
    int iq_file_fd;
    std::mutex iq_recording_mutex;
    atomic_bool_with_wait iq_recording {false};

    void start_iq_recording(std::string filename = "") {
        std::unique_lock lk(iq_recording_mutex);
        if (filename.empty()) {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S.iq");
            filename = oss.str();
        }
        spdlog::info("Start iq recording: {}", filename);
        iq_file_fd = open(filename.c_str(), O_WRONLY | O_CREAT, 0666);
        iq_recording.test_and_set();
    }

    void stop_iq_recording() {
        std::unique_lock lk(iq_recording_mutex);
        spdlog::info("Stop iq recording");
        iq_recording.clear();
        close(iq_file_fd);
    }

    bool is_iq_recording() {
        return iq_recording.test();
    }

    void write() {
        // {
        //     std::unique_lock lk(iq_recording_mutex);
        //     if (iq_recording.test()) write(iq_file_fd, buffer.data(), n*sizeof(std::complex<int16_t>));
        // }
    }
};

}

#endif