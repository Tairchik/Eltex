CC = gcc


all: client.c server.c client.h shared_m.h
	$(CC) -o client_prm -lrt client.c -lncurses 
	$(CC) -o server_prm -lrt server.c
clean: 
	rm -f client_prm server_prm *.o