CC = gcc

all: server.c client.c listClient.c
	$(CC) server.c listClient.c -o server
	$(CC) client.c -o client

clean:
	rm -f server client