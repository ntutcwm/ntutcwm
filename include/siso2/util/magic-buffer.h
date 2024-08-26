#ifndef SISO2_MAGIC_BUFFER_H
#define SISO2_MAGIC_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <spdlog/spdlog.h>

namespace SISO2
{

template<typename Type, size_t capacity>
class MagicBuffer
{
#ifdef __clang__
    Type* first;
    Type* second;
#else
    __restrict Type* first;
    __restrict Type* second;
#endif
public:
    MagicBuffer(void* orig_mem = nullptr);
    Type& operator[](size_t d) {
        return first[d];
    }
};

/*
    https://stackoverflow.com/questions/67781437/mmap-memory-backed-by-other-memory
    https://lo.calho.st/posts/black-magic-buffer/
*/
template<typename Type, size_t capacity>
MagicBuffer<Type, capacity>::MagicBuffer(void* orig_mem)
{
    size_t page_size = getpagesize();
    size_t buffer_size = capacity * sizeof(Type);
    assert(buffer_size % page_size == 0);

    // Get virtual address space of (2*capacity) for our buffer
    this->first = (Type*)mmap(NULL, 2*buffer_size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (!this->first) {
        spdlog::error("mmap failed");
        throw -1;
    }
    this->second = this->first+capacity;

    int memfd;
    uintptr_t physaddr = 0;
    if (!orig_mem)
    {
        orig_mem = mmap(NULL, buffer_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
        if (orig_mem == MAP_FAILED) {
            spdlog::error("mmap orig_mem failed: {}", strerror(errno));
            throw -1;
        }

        // Create a memfd
    #if __APPLE__
        const char* tmpfile = "/tmp/magic-buffer";
        memfd = open(tmpfile, O_RDWR|O_CREAT|O_EXCL, 0777);
        if (memfd != -1) unlink(tmpfile);
    #else
        memfd = memfd_create("buffer", 0);
    #endif
        if (memfd == -1) {
            spdlog::error("memfd_create failed");
            throw -1;
        }

        // Make an anonymous file and set its size
        if (ftruncate(memfd, buffer_size) == -1) {
            spdlog::error("ftruncate failed");
            throw -1;
        }
    }
    else
    {
        // Lock the page to prevent it from being swapped out
        if (mlock(orig_mem, buffer_size)) {
            spdlog::error("mlock orig_mem failed");
            throw -1;
        }

        // Get its information from /proc/self/pagemap
        FILE* fp = fopen("/proc/self/pagemap", "rb");
        if (!fp) {
            spdlog::error("Failed to open \"/proc/self/pagemap\"");
            throw -1;
        }

        if (fseek(fp, (uintptr_t)orig_mem/page_size*8, SEEK_SET)) {
            spdlog::error("fseek failed");
            throw -1;
        }
        uint64_t info;
        if (fread(&info, sizeof(info), 1, fp) != 1) {
            spdlog::error("fread failed");
            throw -1;
        }
        physaddr = (info & ((1ull<<55)-1)) * page_size;

        memfd = open("/dev/mem", O_RDWR|O_SYNC);
        if (memfd == -1) {
            spdlog::error("open(\"/dev/mem\") failed");
            throw -1;
        }
    }

    // Now map first half of our buffer to underlying buffer
    if (this->first != mmap(this->first, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, memfd, physaddr)) {
        spdlog::error("mmap first failed");
        throw -1;
    }

    // Similarly map second half of our buffer
    if (this->second != mmap(this->second, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, memfd, physaddr)) {
        spdlog::error("mmap second failed");
        throw -1;
    }

    for (int i = 0; i < capacity; i++) {
        first[i] = i;
        assert(first[i] == second[i]);
    }

    for (int i = 0; i < capacity; i++) first[i] = 0;
}

};

#endif