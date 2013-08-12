#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint16_t elem_t;
typedef elem_t grid_t[9][9];
const elem_t EMPTY = 0x1FF; // 9 lowest bits set

enum gridstate { NONE, FEASIBLE, SOLUTION };

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
	    return 1;
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
    (*g)[y][x] = elem;
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

int main(void) {
    grid_t grid;
    readgrid(&grid);

    for (int y = 0; y < 9; y++) {
	for (int x = 0; x < 9; x++)
	    printf("0x%03x ", grid[y][x]);
	printf("\n");
    }
}
