CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined


memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) memtest.o mymalloc.o -o memtest

leakmemtest: leakmemtest.o mymalloc.o
	$(CC) $(CFLAGS) leakmemtest.o mymalloc.o -o leakmemtest

leaktest: leaktest.o mymalloc.o
	$(CC) $(CFLAGS) leaktest.o mymalloc.o -o leaktest

realmalloc: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC memtest.c -o realmalloc

leakmalloc: memtest.c
	$(CC) $(CFLAGS) -DREALMALLOC -DLEAK memtest.c -o leakrealmalloc  

freetest: freetest.o mymalloc.c
	$(CC) $(CFLAGS) freetest.o -o freetest


mymalloc.o: mymalloc.c
	$(CC) $(CFLAGS) -c mymalloc.c

memtest.o: memtest.c
	$(CC) $(CFLAGS) -c memtest.c

leakmemtest.o: memtest.c
	$(CC) $(CFLAGS) -c -DLEAK memtest.c -o leakmemtest.o

leaktest.o: leaktest.c
	$(CC) $(CFLAGS) -c leaktest.c

freetest.o: freetest.c
	$(CC) $(CFLAGS) -c freetest.c

malloctest.o: malloctest.c
	$(CC) $(CFLAGS) -c malloctest.c

mymalloc.o memtest.o leaktest.o freetest.o malloctest.o leakmemtest.o: mymalloc.h
