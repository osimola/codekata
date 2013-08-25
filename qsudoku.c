#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

typedef uint16_t elem_t;
typedef elem_t grid_t[9][9];
const elem_t EMPTY = 0x1FF; // 9 lowest bits set

enum gridstate { NONE, FEASIBLE, SOLUTION };

void search(grid_t* g, grid_t* wip);

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

void try_uniqrow(grid_t* g, grid_t *wip, int y) {
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
#                    printf("Found row-uniq: (%d %d), %d\n", x, y, num);
                    search(&g2, wip);
                }
            }
}

void try_uniques(grid_t* g, grid_t* wip) {
    for (int y = 0; y < 9; y++)
        try_uniqrow(g, wip, y);
    /* for (int x = 0; x < 9; x++) */
    /*     try_uniqcol(g, wip, x); */

    /* for (int y = 0; y < 3; y++) */
    /*     for (int x = 0; x < 3; x++) */
    /*         try_uniqsq(g, wip, x , y); */
}

void search(grid_t* g, grid_t* wip) {
    enum gridstate s = is_solution(g);
    if (s == NONE)
        return;
    else if (s == SOLUTION) {
        print_solution(g);
        exit(0);
    }

    try_uniques(g, wip);

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
            search(&g2, &wip2);
        }
}

int main(void) {
    grid_t grid;
    grid_t wip;

    readgrid(&grid);
    memset(&wip, 0, sizeof(grid_t));

    search(&grid, &wip);
}
