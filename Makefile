CFLAGS=-O3 -std=c99 -Wall

qsudoku: qsudoku.o

clean:
	rm *.o
	rm qsudoku
