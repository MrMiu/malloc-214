CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined


memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) memtest.o mymalloc.o -o memtest

freetest: freetest.o mymalloc.c
	$(CC) $(CFLAGS) freetest.o -o freetest


mymalloc.o: mymalloc.c
	$(CC) $(CFLAGS) -c mymalloc.c

memtest.o: memtest.c
	$(CC) $(CFLAGS) -c memtest.c

freetest.o: freetest.c
	$(CC) $(CFLAGS) -c freetest.c

mymalloc.o memtest.o freetest.o: mymalloc.h
