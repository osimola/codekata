# CFLAGS=-std=c99 -Wall -O3 $(shell pkg-config --cflags glib-2.0)
CFLAGS:=-std=c99 -Wall -g $(shell pkg-config --cflags glib-2.0)
LDLIBS:=$(shell pkg-config --libs glib-2.0)

qsudoku: qsudoku.o

clean:
	rm *.o
	rm qsudoku
