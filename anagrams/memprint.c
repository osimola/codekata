#include "memprint.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

void memprint() {
    struct rusage usage;
    (void) getrusage(RUSAGE_SELF, &usage);
    printf("Total mem: %ld\n", usage.ru_maxrss);
}
