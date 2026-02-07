CC = gcc


all: main.o subscriberList.o
	$(CC) main.o subscriberList.o -o program 
	rm -f main.o subscriberList.o

main.o: main.c subscriberList.h
	$(CC) -c main.c

subscriberList.o: subscriberList.c subscriberList.h
	$(CC) -c subscriberList.c

clean: 
	rm -f program.exe subscriberList.o main.o