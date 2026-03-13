CC = gcc

all: myprog1.exe myprog2.exe

myprog1.exe: 1.c
	$(CC) -o myprog1 1.c

myprog2.exe: 2.c
	$(CC) -g -fno-stack-protector -no-pie 2.c -o myprog2*

clean:
	rm -f myprog*

