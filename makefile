CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined


memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) memtest.o mymalloc.o -o memtest

lmemtest: lmemtest.o mymalloc.o
	$(CC) $(CFLAGS) leakmemtest.o mymalloc.o -o leakmemtest

rmemtest: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC memtest.c -o rmemtest

lrmemtest: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC -DLEAK memtest.c -o lrmemtest


malloctest: malloctest.c mymalloc.c
	$(CC) $(CFLAGS) malloctest.c -o malloctest

freetest: freetest.c mymalloc.c
	$(CC) $(CFLAGS) freetest.c -o freetest

leaktest: leaktest.o mymalloc.o
	$(CC) $(CFLAGS) leaktest.o mymalloc.o -o leaktest

memgrind: memgrind.c mymalloc.c
	$(CC) $(CFLAGS) memgrind.c -o memgrind


mymalloc.o: mymalloc.c
	$(CC) $(CFLAGS) -c mymalloc.c

memtest.o: memtest.c
	$(CC) $(CFLAGS) -c memtest.c

lmemtest.o: memtest.c
	$(CC) $(CFLAGS) -c -DLEAK memtest.c -o lmemtest.o

leaktest.o: leaktest.c
	$(CC) $(CFLAGS) -c leaktest.c

mymalloc.o memtest.o lmemtest.o leaktest.o malloctest freetest memgrind: mymalloc.h
