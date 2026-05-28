CC = gcc

all: server.c client.c listClient.c client.h server.h listClient.h
	$(CC) server.c listClient.c -o server
	$(CC) client.c -o client

client: client.c client.h
	$(CC) client.c -o client

server: server.c listClient.c server.h listClient.h
	$(CC) server.c listClient.c -o server

# Не рекомендую использовать, если в системе запущены другие программы с названием ./server
# Лучше найти PID через команду ps -ef и выполнить kill -SIGUSR1 "PID"
kill:
	sudo pkill -SIGUSR1 -f ./server

clean:
	rm -f server client