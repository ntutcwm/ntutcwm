#ifndef SISO2_PHYSADDR_H
#define SISO2_PHYSADDR_H

#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <inttypes.h>

uintptr_t physaddr(uintptr_t virtaddr)
{
    size_t page_size = getpagesize();

    // Get its information from /proc/self/pagemap
    FILE* fp = fopen("/proc/self/pagemap", "rb");
    if (!fp) {
        // spdlog::error("Failed to open \"/proc/self/pagemap\"");
        throw -1;
    }

    if (fseek(fp, (uintptr_t)virtaddr/page_size*8, SEEK_SET)) {
        // spdlog::error("fseek failed");
        throw -1;
    }
    uint64_t info;
    if (fread(&info, sizeof(info), 1, fp) != 1) {
        // spdlog::error("fread failed");
        throw -1;
    }
    return (info & ((1ull<<55)-1)) * page_size;
}

#endif