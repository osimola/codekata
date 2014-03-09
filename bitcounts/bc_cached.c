#include "mmaputil.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static int counts[65535];

void init_table() {
    memset(counts, 0, sizeof(counts));

    for (int bit = 0; bit < 16; bit++) {
        for (int i = 1 << bit; i < 2 << bit; i++)
            counts[i] = counts[i - (1 << bit)] + 1;
    }
}

int bitcount(const struct mmapi* mmapi) {
    size_t rounded = mmapi->size / sizeof(uint16_t);
    int count = 0;

    for (size_t i = 0; i < rounded; i++) {
        count += counts[((uint16_t*) mmapi->map)[i]];
    }

    if (rounded % 2 != 0)
        count += counts[((uint8_t*)mmapi->map)[mmapi->size - 1]];

    return count;
}

int main(int argc, char* argv[]) {
    if (argc < 2)
        return -1;
    init_table();

    struct mmapi* mmapi = open_and_mmap(argv[1]);
    if (mmapi == NULL)
        return -1;

    printf("%d\n", bitcount(mmapi));

    munmap_and_close(mmapi);
    
    return 0;
}
