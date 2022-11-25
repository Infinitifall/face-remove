CC=gcc
CFLAGS = -Wall -O3 -ffast-math

all: face-remove

face-remove: face-remove.o
	$(CC) $(CFLAGS) -o face-remove face-remove.o -lm

face-remove.o: face-remove.c
	$(CC) $(CFLAGS) -c face-remove.c

clean:
	$(RM) face-remove
	$(RM) face-remove.o
