CFLAGS=-g -Wall -std=c99 -O3

all: bc_cached bc_parallelsum

bc_cached: bc_cached.c mmaputil.c
bc_parallelsum: bc_parallelsum.c mmaputil.c

clean:
	rm bc_cached bc_parallelsum
