#ifndef SISO2_AXIDMA_BUFFER_H
#define SISO2_AXIDMA_BUFFER_H

#include "siso2/buffer/buffer.h"
#include "siso2/dev/xilinx-axidma.h"

namespace SISO2
{

template <typename T>
struct AXIDMABuffer: public Buffer<T>
{
    AXIDMABuffer(size_t size) {
        if (!XilinxAXIDMA::device) XilinxAXIDMA::device = axidma_init();
        this->size = size;
        this->data = (T*)axidma_malloc(XilinxAXIDMA::device, size*sizeof(T));
    }
    ~AXIDMABuffer() {
        axidma_free(XilinxAXIDMA::device, this->data, this->size*sizeof(T));
    }
};

}

#endif