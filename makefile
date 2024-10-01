malloc:mymalloc.c
	gcc -Wall -g -std=c99 -Wvla -fsanitize=address, undefined mymalloc.c -o mymalloc
