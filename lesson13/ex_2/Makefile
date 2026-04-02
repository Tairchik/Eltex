CC = gcc


all: client.c server.c client.h queue.h
	$(CC) client.c -o client_prm -lncurses
	$(CC) server.c -o server_prm


clean: 
	rm -f client_prm server_prm *.o