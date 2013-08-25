# CFLAGS=-std=c99 -Wall -O3
CFLAGS=-std=c99 -Wall -g


qsudoku: qsudoku.o

clean:
	rm *.o
	rm qsudoku
