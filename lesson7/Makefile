CC = gcc


all: library
	$(CC) main.c -o program -L. -lcalc

library: calc.h div.c add.c mul.c sub.c
	$(CC) -c add.c div.c mul.c sub.c
	ar rc libcalc.a add.o div.o mul.o sub.o
	rm -f add.o div.o mul.o sub.o

clean:
	rm -f program libcalc.a