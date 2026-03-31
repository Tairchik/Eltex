CC = gcc
CFLAGS = -Wall
TARGET = run

OBJS = ex1.o

all: $(TARGET)

debug: CFLAGS += -g
debug: clean all

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

ex1.o: ex1.c
	$(CC) $(CFLAGS) -c ex1.c

clean:
	rm -f run *.o