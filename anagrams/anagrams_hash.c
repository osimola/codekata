#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "stringsort.h"
#include "memprint.h"

GHashTable *readdata() {
    const size_t buflen = 100;
    char buf[buflen];
    GHashTable *table = g_hash_table_new(g_str_hash, g_str_equal);

    while (1) {
        fgets(buf, buflen, stdin);
        if (feof(stdin))
            break;
        int len = strlen(buf);
        if (strlen(buf) == buflen - 1) {
            fprintf(stderr, "Unexpectedly long line. Increase buflen.");
            exit(1);
        }
        buf[len - 1] = '\0'; // C really needs a strip() function
        
        char *original = malloc(len + 1);
        strcpy(original, buf);
        sortinplace(buf);

        GSList *list = g_hash_table_lookup(table, buf);
        char *key = buf;
        if (list == NULL) {
            key = calloc(1, len + 1);
            memcpy(key, buf, len);
        }
        g_hash_table_insert(table, key, g_slist_prepend(list, original));
    }

    return table;
}

void print_anagrams(GHashTable *table) {
    GHashTableIter iter;
    gpointer key;
    GSList *values;

    g_hash_table_iter_init(&iter, table);
    while (g_hash_table_iter_next(&iter, &key, (gpointer *) &values)) {
        if (g_slist_next(values) != NULL) {
            while (values != NULL) {
                printf("%s ", (const char *)values->data);
                values = g_slist_next(values);
            }
            fputc('\n', stdout);
        }
    }
}

int main(void) {
    GHashTable *table = readdata();
    print_anagrams(table);
    memprint();
    return 0;
}
