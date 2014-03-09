#include "mmaputil.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

struct mmapi* open_and_mmap(const char* name) {
    int fd = open(name, O_RDONLY);
    if (fd < 0)
        goto fail;

    struct stat stat;
    if (fstat(fd, &stat) < 0)
        goto fail;

    void *map = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);

    struct mmapi* mmapi = malloc(sizeof(struct mmapi));
    mmapi->fd = fd;
    mmapi->size = stat.st_size;
    mmapi->map = map;
    return mmapi;
    
 fail:
    if (fd > 0)
        close(fd);
    return NULL;
}

void munmap_and_close(struct mmapi* mmapi) {
    munmap(mmapi->map, mmapi->size);
    close(mmapi->fd);
    free(mmapi);
}
