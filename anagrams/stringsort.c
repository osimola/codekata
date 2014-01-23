#include "stringsort.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int comparechar(const void *a, const void *b) {
    // Should work for 7-bit ASCII
    char c1 = *(const char *)a;
    char c2 = *(const char *)b;
    return c1 - c2;
}

void sortinplace(char *buf) {
    size_t len = strlen(buf);

    for (size_t i = 0; i < len; i++)
        buf[i] = tolower(buf[i]);

    qsort(buf, len, 1, comparechar);
}

char *sortedcopy(const char *source) {
    size_t len = strlen(source);
    char *dst = malloc(len + 1);
    strcpy(dst, source);
    
    for (size_t i = 0; i < len; i++)
        dst[i] = tolower(dst[i]);

    qsort(dst, len, 1, comparechar);

    return dst;
}
