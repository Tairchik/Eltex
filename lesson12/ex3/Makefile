CC = gcc
CFLAGS = -Wall
TARGET = run

OBJS = main.o shell.o

all: $(TARGET)

debug: CFLAGS += -g
debug: clean all

$(TARGET): $(OBJS) shell.h
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f run *.o