CFLAGS=-std=c99 -Wall -O3

qsudoku: qsudoku.o

clean:
	rm *.o
	rm qsudoku
