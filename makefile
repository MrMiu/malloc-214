CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined

all: malloctest freetest leaktest memgrind

malloctest: malloctest.c mymalloc.c
	$(CC) $(CFLAGS) malloctest.c -o malloctest

freetest: freetest.c mymalloc.c
	$(CC) $(CFLAGS) freetest.c -o freetest

leaktest: leaktest.c
	$(CC) $(CFLAGS) leaktest.o -o leaktest

memgrind: memgrind.c mymalloc.c
	$(CC) $(CFLAGS) memgrind.c -o memgrind


mymalloc.o: mymalloc.c
	$(CC) $(CFLAGS) -c mymalloc.c

leaktest.o: leaktest.c
	$(CC) $(CFLAGS) -c leaktest.c

mymalloc.o leaktest.o malloctest freetest memgrind: mymalloc.h
