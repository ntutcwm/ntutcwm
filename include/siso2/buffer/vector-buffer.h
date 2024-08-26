#ifndef SISO2_VECTOR_BUFFER_H
#define SISO2_VECTOR_BUFFER_H

#include "siso2/buffer/buffer.h"
#include <vector>

namespace SISO2
{

template <typename T>
struct VectorBuffer: public Buffer<T>
{
    std::vector<T> buffer;
    VectorBuffer(size_t size) : buffer(size) {
        this->size = size;
        this->data = buffer.data();
    }
};

}

#endif