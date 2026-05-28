CC = gcc

all: server.c client.c listClient.c client.h server.h listClient.h
	$(CC) server.c listClient.c -o server
	$(CC) client.c -o client

client: client.c client.h
	$(CC) client.c -o client

server: server.c listClient.c server.h listClient.h
	$(CC) server.c listClient.c -o server

clean:
	rm -f server client