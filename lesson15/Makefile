CC = gcc

all: ex_1.c send_kill.c shared_m.h
	$(CC) ex_1.c -o server1 -lrt
	$(CC) send_kill.c -o kill -lrt

clean:
	rm -f server* kill