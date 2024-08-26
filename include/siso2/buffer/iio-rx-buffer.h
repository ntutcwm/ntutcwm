#ifndef SISO2_IIO_RX_BUFFER_H
#define SISO2_IIO_RX_BUFFER_H

#include "siso2/buffer/buffer.h"
#include <iio/iio.h>

namespace SISO2
{

template <typename T>
struct IIORXBuffer: public Buffer<T>
{
    iio_block* block;
    IIORXBuffer(size_t size, iio_buffer* buffer) {
        block = iio_buffer_create_block(buffer, size*sizeof(std::complex<int16_t>));
        this->size = size;
        this->data = (T*)iio_block_start(block);
        this->impl = block;
        int err = iio_block_enqueue(block, 0, /*cyclic*/false);
        if (err) ; //spdlog::error("RX iio_block_enqueue failed");
        else this->enqueued = true;
    }
    void dequeue() override {
        if (this->enqueued) {
            int err = iio_block_dequeue(block, /*nonblock*/false);
            if (err) ; //spdlog::error("RX iio_block_dequeue failed");
            else this->enqueued = false;
        }
    }
    void release() override {
        int err = iio_block_enqueue(block, 0, /*cyclic*/false);
        if (err) ; //spdlog::error("RX iio_block_enqueue failed");
        else this->enqueued = true;
        this->buffer_pool->release_buffer(this);
    }
};

}

#endif