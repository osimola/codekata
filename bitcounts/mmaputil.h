#ifndef MMAPUTIL_H
#define MMAPUTIL_H

#include <stdlib.h>

struct mmapi { 
    void *map;
    size_t size;
    int fd;
};

struct mmapi* open_and_mmap(const char* name);
void munmap_and_close(struct mmapi* mmapi);

#endif
