CC=gcc
CFLAGS = -Wall

.PHONY: all debug clean

all: CFLAGS += -O3 -ffast-math -march=native -flto
all: face-remove

debug: CFLAGS += -g
debug: face-remove

debug_fast: CFLAGS += -g -O3 -ffast-math -march=native -flto
debug_fast: face-remove

face-remove: face-remove.o
	$(CC) $(CFLAGS) -o face-remove face-remove.o -lm

face-remove.o: face-remove.c
	$(CC) $(CFLAGS) -c face-remove.c

clean:
	$(RM) face-remove
	$(RM) face-remove.o
