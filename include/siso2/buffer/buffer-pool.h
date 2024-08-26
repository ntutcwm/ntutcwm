#ifndef SISO2_BUFFER_POOL_H
#define SISO2_BUFFER_POOL_H

#include <atomic_queue/atomic_queue.h>
#include <memory>
#include <vector>

namespace SISO2
{

template <typename T>
struct Buffer;

template <typename T>
struct BufferPool
{
    std::vector<std::unique_ptr<Buffer<T>>> buffer_impl;
    atomic_queue::AtomicQueueB<Buffer<T>*> buffer_queue;

    template<template<class> typename BufferType, typename... Types>
    BufferPool(BufferType<T>* typeptr, size_t N) : buffer_queue(N) {
    }

    template<template<class> typename BufferType, typename... Types>
    BufferPool(BufferType<T>* typeptr, size_t N, Types... args) : buffer_impl(N), buffer_queue(N) {
        for (int i = 0; i < N; i++) {
            buffer_impl[i] = std::make_unique<BufferType<T>>(args...);
            buffer_impl[i]->buffer_pool = this;
            buffer_queue.push(buffer_impl[i].get());
        }
    }

    Buffer<T>* create_buffer() {
        buffer_impl.emplace_back(std::move(std::make_unique<Buffer<T>>()));
        auto buffer = buffer_impl.back().get();
        buffer->buffer_pool = this;
        buffer_queue.push(buffer);
        return buffer;
    }

    Buffer<T>* get_buffer() {
        auto buffer = buffer_queue.pop();
        buffer->dequeue();
        return buffer;
    }

    void release_buffer(Buffer<T>* buffer) {
        buffer_queue.push(buffer);
    }

    bool empty() {
        return buffer_queue.was_empty();
    }

    bool full() {
        return buffer_queue.was_full();
    }

    size_t size() {
        return buffer_queue.was_size();
    }
};

}

#endif