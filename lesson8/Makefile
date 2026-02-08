CC = gcc


all: library
	$(CC) main.c -o program -L. -lcalc
	LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./program

library: calc.h div.c add.c mul.c sub.c
	$(CC) add.c div.c mul.c sub.c -fPIC -c
	$(CC) -shared add.o div.o mul.o sub.o -o libcalc.so
	rm -f add.o div.o mul.o sub.o

clean:
	rm -f program libcalc.so