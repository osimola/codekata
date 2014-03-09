#include "mmaputil.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static inline int bitcount64(uint64_t w) {
    w = (w & 0x5555555555555555) + ((w & 0xAAAAAAAAAAAAAAAA) >> 1);
    w = (w & 0x3333333333333333) + ((w & 0xCCCCCCCCCCCCCCCC) >> 2);
    w = (w & 0x0F0F0F0F0F0F0F0F) + ((w & 0xF0F0F0F0F0F0F0F0) >> 4);
    w = (w & 0x00FF00FF00FF00FF) + ((w & 0xFF00FF00FF00FF00) >> 8);
    w = (w & 0x0000FFFF0000FFFF) + ((w & 0xFFFF0000FFFF0000) >> 16);
    w = (w & 0x00000000FFFFFFFF) + ((w & 0xFFFFFFFF00000000) >> 32);
    return w;
}

static inline inline int bitcount8(uint8_t w) {
    w = (w & 0x55) + ((w & 0xAA) >> 1);
    w = (w & 0x33) + ((w & 0xCC) >> 2);
    w = (w & 0x0F) + ((w & 0xF0) >> 4);
    return w;
}

int bitcount(const struct mmapi* mmapi) {
    size_t rounded = mmapi->size / sizeof(uint64_t);
    int count = 0;

    for (size_t i = 0; i < rounded; i++) {
        count += bitcount64(((uint64_t*) mmapi->map)[i]);
    }

    for (size_t i = rounded * sizeof(uint64_t); i < mmapi->size; i++) {
        count += bitcount8(((uint8_t*) mmapi->map)[i]);
    }

    return count;
}

int main(int argc, char* argv[]) {
    if (argc < 2)
        return -1;

    struct mmapi* mmapi = open_and_mmap(argv[1]);
    if (mmapi == NULL)
        return -1;

    printf("%d\n", bitcount(mmapi));

    munmap_and_close(mmapi);
    
    return 0;
}
