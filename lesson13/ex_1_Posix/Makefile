CC = gcc

all: client.c server.c
	$(CC) -o client_prm -lrt client.c 
	$(CC) -o server_prm -lrt server.c

clean:
	rm -f client_prm server_prm *.o