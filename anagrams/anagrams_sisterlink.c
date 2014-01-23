#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stringsort.h"
#include "sisterlinkedtree.h"
#include "memprint.h"

struct slt_tree *readdata() {
    const size_t buflen = 100;
    char buf[buflen];
    char sorted[buflen];
    struct slt_tree *tree = slt_new();

    while (1) {
        fgets(buf, buflen, stdin);
        if (feof(stdin))
            break;
        int len = strlen(buf);
        if (strlen(buf) == buflen - 1) {
            fprintf(stderr, "Unexpectedly long line. Increase buflen.");
            exit(1);
        }
        buf[len - 1] = '\0'; // Erase newline

        strcpy(sorted, buf);
        sortinplace(sorted);

        slt_add(tree, sorted, buf);
    }

    return tree;
}

int main(void) {
    struct slt_tree *tree = readdata();
    slt_print_anagrams(tree);
    memprint();
    // slt_population(tree);
    return 0;
}
