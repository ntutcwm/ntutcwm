#ifndef SISO2_BUFFER_H
#define SISO2_BUFFER_H

#include <stdlib.h>
#include "siso2/buffer/buffer-pool.h"

namespace SISO2
{

template <typename T>
struct Buffer
{
    size_t size;
    size_t level;
    T* data;
    void* impl;
    bool enqueued;
    BufferPool<T>* buffer_pool;
    virtual void release() { buffer_pool->release_buffer(this); }
    virtual void dequeue() {}
};

}

#endif