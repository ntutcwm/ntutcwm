#ifndef SISO2_SD_CACHE_H
#define SISO2_SD_CACHE_H

#include "siso2/cache/cache.h"

namespace SISO2
{

class SDCache: public Cache
{
    BufferPool<std::complex<int16_t>> buffer_pool;
    atomic_queue::AtomicQueueB<Buffer<std::complex<int16_t>>*> cache;

public:
    SDCache(std::string path, size_t block_size, size_t block_num);
    Buffer<std::complex<int16_t>>* get_buffer() override;
    void push(Buffer<std::complex<int16_t>>* buffer) override;
    Buffer<std::complex<int16_t>>* pop() override;
    bool empty() override;
    bool full() override;
    size_t size() override;
};

}

#endif