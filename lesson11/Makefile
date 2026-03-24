CC = gcc
CFLAGS = -Wall
TARGET = run

OBJS = program.o randomGenerator.o

all: $(TARGET)

debug: CFLAGS += -g
debug: clean all

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

program.o: program.c
	$(CC) $(CFLAGS) -c program.c

randomGenerator.o: randomGenerator.c randomGenerator.h
	$(CC) $(CFLAGS) -c randomGenerator.c

clean:
	rm -f run *.o