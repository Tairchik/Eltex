CC = gcc


all: view.o file_manager.o
	$(CC) view.o file_manager.o -o program -lncurses

view.o: view.c file_manager.h
	$(CC) -c view.c

file_manager.o: file_manager.c file_manager.h
	$(CC) -c file_manager.c

debug: clean
	$(CC) -g view.c file_manager.c -o program -lncurses

clean: 
	rm -f program* *.o