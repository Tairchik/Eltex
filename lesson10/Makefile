CC = gcc


all: 1.c 2.c 3.o shell.o shell.h
	gcc 1.c -o prm1
	gcc 2.c -o prm2
	gcc 3.o shell.o -o prm3

ex1: 1.c
	gcc 1.c -o prm1

ex2: 2.c 
	gcc 2.c -o prm2

ex3: 3.o shell.o shell.h
	gcc 3.o shell.o -o prm3

3.o: 3.c
	gcc -c 3.c

shell.o: shell.h shell.c
	gcc -c shell.c

clean:
	rm -f prm* *.o