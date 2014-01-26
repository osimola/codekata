#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <glib.h>

typedef uint16_t elem_t;
typedef elem_t grid_t[9][9];
const elem_t EMPTY = 0x1FF; // 9 lowest bits set

const int ROWLEN = 80;

enum gridstate { NONE, FEASIBLE, SOLUTION };

struct step {
    char *desc;
    struct step *next;
};

struct step* search(grid_t* g, grid_t* wip, GHashTable* hash);

struct step* mkstep(char* type, int n, int x, int y, struct step* next) {
    struct step* step = malloc(sizeof(struct step));
    char *desc = malloc(ROWLEN + 1);
    snprintf(desc, ROWLEN, "%s: %d at (%d, %d)", type, n + 1, x + 1, y + 1);
    step->desc = desc;
    step->next = next;
    return step;
}

int bitcount(uint16_t word) {
    int r = 0;
    for (int i = 0; i < 16; i++) {
        if ((word & 0x1) != 0)
            r++;
        word >>= 1;
    }
    return r;
}

int lowbit(uint16_t word) {
    for (int i = 0; i < 16; i++) {
        if ((word & 0x1) != 0)
            return i;
        word >>= 1;
    }

    // Return 16 on all-zeroes
    return 16;
}

enum gridstate is_solution(grid_t* g) {
    enum gridstate result = SOLUTION;
    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++) {
            int bc = bitcount((*g)[y][x]);
            if (bc > 1)
                result = FEASIBLE;
            if (bc == 0)
                result = NONE;
        }
    return result;
}

void mark(grid_t* g, int x, int y, int value) {
    elem_t elem = (elem_t)1 << value;
    elem_t mask = ~elem;

    for (int i = 0; i < 9; i++) {
        (*g)[y][i] &= mask;
        (*g)[i][x] &= mask;
    }

    int x0 = 3 * (x / 3);
    int y0 = 3 * (y / 3);

    for (int dy = 0; dy < 3; dy++)
        for (int dx = 0; dx < 3; dx++)
            (*g)[y0 + dy][x0 + dx] &= mask;
    (*g)[y][x] = elem;
}

void print_solution(grid_t* g) {
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++)
            printf("%d ", lowbit((*g)[y][x]) + 1);
        printf("\n");
    }
}

void readgrid(grid_t* g) {
    char c;
    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++)
            (*g)[y][x] = EMPTY;

    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++) {
            int r = scanf(" %c", &c);
            if (r != 1)
                exit(-1);
            if (c >= '1' && c <= '9')
                mark(g, x, y, c - '1');
        }
}

guint grid_hash(const void *arg) {
    const grid_t* g = (const grid_t*) arg;
    guint rv = 0;
    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++)
            rv = rv * 31 + (*g)[y][x];
    return rv;
}

gboolean grid_equals(const void* g1, const void* g2) {
    return (memcmp(g1, g2, sizeof(grid_t)) == 0);
}

bool is_visited(GHashTable* hash, const grid_t* g) {
    if (g_hash_table_lookup(hash, g))
        return true;
    else {
        grid_t* g2 = malloc(sizeof(grid_t));
        memcpy(g2, g, sizeof(grid_t));
        g_hash_table_insert(hash, g2, g2);
        return false;
    }
}

void find_candidate(grid_t* g, grid_t *wip, int* px, int* py) {
    int count = INT_MAX;
    *px = 0;
    *py = 0;
    for (int y = 0; y < 9; y++)
        for (int x = 0; x < 9; x++) {
            if ((*wip)[y][x] == 0) {
                int bc = bitcount((*g)[y][x]);
                if (bc > 1 && bc < count) {
                    count = bc;
                    *px = x;
                    *py = y;
                }
            }
        }
}

struct step* try_uniqrow(grid_t* g, grid_t *wip, int y, GHashTable* hash) {
    int counts[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int x = 0; x < 9; x++) {
        elem_t e = (*g)[y][x];
        for (int bit = 0; bit < 9; bit++)
            if (((e >> bit) & 0x1) != 0)
                counts[bit] += 1;
    }

    for (int num = 0; num < 9; num++)
        if (counts[num] == 1)
            for (int x = 0; x < 9; x++) {
                elem_t e = (*g)[y][x];
                if ((e >> num) & 0x1 && bitcount(e) > 1) {
                    grid_t g2;
                    
                    memcpy(&g2, g, sizeof(grid_t));
                    mark(&g2, x, y, num);
                    struct step *s = search(&g2, wip, hash);
                    if (s != NULL)
                        return mkstep("Row unique", num, x, y, s);
                }
            }
    return NULL;
}

struct step* try_uniqcol(grid_t* g, grid_t *wip, int x, GHashTable* hash) {
    int counts[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int y = 0; y < 9; y++) {
        elem_t e = (*g)[y][x];
        for (int bit = 0; bit < 9; bit++)
            if (((e >> bit) & 0x1) != 0)
                counts[bit] += 1;
    }

    for (int num = 0; num < 9; num++)
        if (counts[num] == 1)
            for (int y = 0; y < 9; y++) {
                elem_t e = (*g)[y][x];
                if ((e >> num) & 0x1 && bitcount(e) > 1) {
                    grid_t g2;
                    
                    memcpy(&g2, g, sizeof(grid_t));
                    mark(&g2, x, y, num);
                    struct step* s = search(&g2, wip, hash);
                    if (s != NULL)
                        return mkstep("Column unique", num, x, y, s);
                }
            }
    return NULL;
}

struct step* try_uniqsq(grid_t *g, grid_t *wip, int sq_x, int sq_y, GHashTable* hash) {
    const int x0 = sq_x * 3;
    const int y0 = sq_y * 3;
    int counts[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int y = y0; y < y0 + 3; y++) 
        for (int x = x0; x < x0 + 3; x++) {
            elem_t e = (*g)[y][x];
            for (int bit = 0; bit < 9; bit++)
                if (((e >> bit) & 0x1) != 0)
                    counts[bit] += 1;
        }
    
    for (int num = 0; num < 9; num++)
        if (counts[num] == 1)
            for (int y = y0; y < y0 + 3; y++) 
                for (int x = x0; x < x0 + 3; x++) {
                    elem_t e = (*g)[y][x];
                    if ((e >> num) & 0x1 && bitcount(e) > 1) {
                        grid_t g2;
                        
                        memcpy(&g2, g, sizeof(grid_t));
                        mark(&g2, x, y, num);
                        struct step* s = search(&g2, wip, hash);
                        if (s != NULL)
                            return mkstep("Square unique", num, x, y, s);
                    }
                }
    return NULL;
}

struct step* try_uniques(grid_t* g, grid_t* wip, GHashTable* hash) {
    struct step* s = NULL;
    for (int y = 0; y < 9; y++) {
        s = try_uniqrow(g, wip, y, hash);
        if (s != NULL)            
            return s;
    }

    for (int x = 0; x < 9; x++) {
        s = try_uniqcol(g, wip, x, hash);
        if (s != NULL)
            return s;
    }
    

    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 3; x++) {
            s = try_uniqsq(g, wip, x , y, hash);
            if (s != NULL)
                return s;
        }

    return NULL;
}

struct step* search(grid_t* g, grid_t* wip, GHashTable* hash) {
    if (is_visited(hash, (const grid_t*)g))
        return NULL;

    enum gridstate state = is_solution(g);
    if (state == NONE)
        return NULL;
    else if (state == SOLUTION) {
        print_solution(g);
        struct step *s = malloc(sizeof(struct step));
        char* msg = malloc(8);
        strcpy(msg, "Ready!");
        s->desc = msg;
        s->next = NULL;
        return s;
    }

    struct step *s = try_uniques(g, wip, hash);
    if (s != NULL)
        return s;

    int x; int y;
    find_candidate(g, wip, &x, &y);
    elem_t bits = (*g)[y][x];

    grid_t wip2;
    memcpy(&wip2, wip, sizeof(grid_t));
    wip2[y][x] = 1;

    for (int i = 0; i < 9; i++)
        if (((bits >> i) & 0x1) != 0) {
            grid_t g2;

            memcpy(&g2, g, sizeof(grid_t));
            mark(&g2, x, y, i);
            struct step* s = search(&g2, &wip2, hash);
            if (s != NULL)
                return mkstep("Guess", i, x, y, s);
        }
    
    return NULL;
}

int main(void) {
    grid_t grid;
    grid_t wip;

    GHashTable* hash = g_hash_table_new_full(grid_hash, grid_equals, NULL, free);

    readgrid(&grid);
    memset(&wip, 0, sizeof(grid_t));

    struct step *s = search(&grid, &wip, hash);
    while (s != NULL) {
        printf("%s\n", s->desc);
        struct step *n = s->next;
        free(s->desc);
        free(s);
        s = n;
    }

    g_hash_table_destroy(hash);
}
