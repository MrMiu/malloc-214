malloc:mymalloc.c
	gcc -Wall -g -fsanitize=address -fsanitize=undefined mymalloc.c -o mymalloc