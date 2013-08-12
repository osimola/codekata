CFLAGS=-O3 -std=c99

qsudoku: qsudoku.o

clean:
	rm *.o
	rm qsudoku
