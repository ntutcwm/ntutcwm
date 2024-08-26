#ifndef SISO2_OFDM_SYMB_ACQUISITION_CPU_H
#define SISO2_OFDM_SYMB_ACQUISITION_CPU_H

#include <complex>
#include <vector>
#include <atomic>
#include <mutex>
#include <span>
#include "siso2/util/magic-buffer.h"
#include "siso2/util/atomic_bool_with_wait.h"
#include "siso2/phy/ofdm/ofdm-symb-acquisition.h"
#include <atomic_queue/atomic_queue.h>
#include "siso2/util/shift_reg.h"
#include "siso2/buffer/buffer-pool.h"
#include "siso2/buffer/vector-buffer.h"
#include "siso2/codec/decoder.h"

namespace SISO2
{

using namespace std::complex_literals;

class OfdmSymbAcquisitionCPU : public OfdmSymbAcquisition
{
    size_t FFT_LEN;
    size_t CP_LEN;
    size_t SUB_CP_NUM;
    size_t SUB_CP_LEN;

    static constexpr size_t buffer_size = 65536*32;

    MagicBuffer<std::complex<int16_t>, buffer_size> sample; // max 2^11
    ShiftRegB<uint32_t> metric_p; // max 128(SUB_CP_LEN) * 2^23(power) = 2^30
    MagicBuffer<std::complex<int32_t>, buffer_size> metric_cplx_1; // max 128(SUB_CP_LEN) * 2*(2^11)^2(sample^2) = 2^30
    MagicBuffer<std::complex<int32_t>, buffer_size> metric_cplx_2; // max 128(SUB_CP_LEN) * 2*(2^11)^2(sample^2) = 2^30
    MagicBuffer<std::complex<int32_t>, buffer_size> metric_cplx_3; // max 128(SUB_CP_LEN) * 2*(2^11)^2(sample^2) = 2^30
    std::atomic<size_t> metric_out = 0;

    BufferPool<std::complex<int16_t>> buffer_pool;

    atomic_bool_with_wait queue_signal {false};
    std::mutex pop_mutex;

    void push_buffer(size_t n) {
        size_t _metric_in = metric_in + n;
        if (_metric_in >= buffer_size) _metric_in -= buffer_size;
        metric_in = _metric_in;
        //spdlog::debug("push {} -> {} {} size: {}", n, metric_in, metric_out, size());
    }

    void pop_buffer(size_t n) {
        size_t _metric_out = metric_out + n;
        if (_metric_out >= buffer_size) _metric_out -= buffer_size;
        metric_out = _metric_out;
        //spdlog::debug("pop {} -> {} {} size: {}", n, metric_in, metric_out, size());
    }
public:
    MagicBuffer<float, buffer_size> metric;
    std::atomic<size_t> metric_in = 0;

    OfdmSymbAcquisitionCPU(size_t FFT_LEN, size_t CP_LEN) : 
        FFT_LEN(FFT_LEN), CP_LEN(CP_LEN), SUB_CP_LEN(CP_LEN/2),
        metric_p(FFT_LEN+SUB_CP_LEN+1),
        buffer_pool((VectorBuffer<std::complex<int16_t>>*)nullptr, 2048, FFT_LEN)
    {
        push_buffer(1);
        for (int i = 0; i < FFT_LEN+SUB_CP_LEN+1; i++) metric_p.shift(1);
    }

    size_t size() {
        int size = (int)metric_in - metric_out;
        if (size < 0) size += buffer_size;
        return size;
    }

    size_t space() {
        int space = (int)metric_out - metric_in - 1;
        if (space < 0) space += buffer_size;
        space -= (FFT_LEN+CP_LEN - 1); // account for sample_in - metric_in
        return space;
    }

    void push(Buffer<std::complex<int16_t>>* in) override;
    Buffer<std::complex<int16_t>>* pop() override;
};

}

#endif

