#ifndef SISO2_IIO_TX_BUFFER_H
#define SISO2_IIO_TX_BUFFER_H

#include "siso2/buffer/buffer.h"
// #include "siso2/util/physaddr.h"
// #include "siso2/dev/xilinx-axidma.h"
#include <iio/iio.h>

namespace SISO2
{

template <typename T>
struct IIOTXBuffer: public Buffer<T>
{
    iio_block* block;
    // static inline int devmem = -1;
    IIOTXBuffer(size_t size, iio_buffer* buffer) {
        // if (!XilinxAXIDMA::device) XilinxAXIDMA::device = axidma_init();
        // if (devmem == -1) devmem = open("/dev/mem", O_RDWR | O_SYNC);
        block = iio_buffer_create_block(buffer, size*sizeof(std::complex<int16_t>));
        this->size = size;
        auto data = iio_block_start(block);
        // auto data_phys = physaddr((uintptr_t)data);
        // auto alias = mmap(nullptr, size*sizeof(std::complex<int16_t>), PROT_READ | PROT_WRITE, MAP_SHARED, devmem, (off_t)data);
        // fprintf(stderr, "IIOTXBuffer mmap 0x%08x -> 0x%08x\n", (off_t)data, (off_t)alias);
        this->data = (T*)data; //alias;
        this->impl = block;
        // int result = axidma_register_buffer(XilinxAXIDMA::device, devmem, this->data, this->size*sizeof(std::complex<int16_t>));
        // fprintf(stderr, "axidma_register_buffer: %d\n", result);
    }
    void dequeue() override {
        if (this->enqueued) {
            int err = iio_block_dequeue(block, /*nonblock*/false);
            if (err) ; //spdlog::error("TX iio_block_dequeue failed");
            else this->enqueued = false;
        }
    }
};

}

#endif