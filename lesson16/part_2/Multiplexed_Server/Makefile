CC = gcc

all: server.c client_tcp.c client_udp.c server.h
	$(CC) server.c -o server
	$(CC) client_tcp.c -o client_tcp
	$(CC) client_udp.c -o client_udp

clean:
	rm -f server client_udp client_tcp