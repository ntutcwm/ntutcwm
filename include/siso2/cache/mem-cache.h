#ifndef SISO2_MEM_CACHE_H
#define SISO2_MEM_CACHE_H

#include "siso2/cache/cache.h"
#include <atomic_queue/atomic_queue.h>

namespace SISO2
{

class MemCache: public Cache
{
    BufferPool<std::complex<int16_t>> buffer_pool;
    atomic_queue::AtomicQueueB<Buffer<std::complex<int16_t>>*> cache;
public:
    MemCache(size_t block_size, size_t block_num)
        : buffer_pool((VectorBuffer<std::complex<int16_t>>*)nullptr, block_num, block_size)
        , cache(block_num) {}
    
    Buffer<std::complex<int16_t>>* get_buffer() override {
        return buffer_pool.get_buffer();
    }
    
    void push(Buffer<std::complex<int16_t>>* buffer) override {
        cache.push(buffer);
    }
    
    Buffer<std::complex<int16_t>>* pop() override {
        return cache.pop();
    }
    
    bool empty() override {
        return cache.was_empty();
    }
   
    bool full() override {
        return cache.was_full();
    }
    
    size_t size() override {
        return cache.was_size();
    }
};
    
}

#endif