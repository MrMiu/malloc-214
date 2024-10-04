#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.c"

void check(int chunkNumber, void *chunk, int expectedUsed, size_t expectedSize);
void expectInvalid(); 

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "No test case given.\n");
		exit(EXIT_FAILURE);
	}
	int testCase = atoi(argv[1]);

	//Partitions heap.bytes into 5 chunks.

	size_t size1 = 16;
	size_t size2 = 32;
	size_t size3 = 64;
	size_t size4 = 128;
	size_t size5 = MEMLENGTH - size1 - size2 - size3 - size4;

	void *ptr1 = malloc(size1 - HEADER_SIZE);
	void *ptr2 = malloc(size2 - HEADER_SIZE);
	void *ptr3 = malloc(size3 - HEADER_SIZE);
	void *ptr4 = malloc(size4 - HEADER_SIZE);
	void *ptr5 = malloc(size5 - HEADER_SIZE);

	char *start = heap.bytes;
	char *end = start + MEMLENGTH;

	switch(testCase)
	{
		//freeing pointer before heap.bytes
		case 1:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(start - 1);
			printf("Test failed.\n"); //makes sure free doesn't work
			break;
		//freeing heap.bytes
		case 2:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(start);
			printf("Test failed.\n"); //makes sure free doesn't work
			break;
		//freeing end of heap.bytes:
		case 3:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(end);
			printf("Test failed.\n"); //makes sure free doesn't work
			break;
		//freeing pointer after end of heap.bytes
		case 4:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(end + 1);
			printf("Test failed.\n"); //makes sure free doesn't work
			break;
		//freeing chunk that is already free
		case 5:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(ptr1);
			printf("Test failed.\n"); //makes sure free doesn't work
			break;
		//freeing pointer to middle of chunk
		case 6:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(ptr1 + 1);
			printf("Test failed.\n"); //makes sure free doesn't work
			break;
		//freeing pointer to beginning of header
		case 7:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(getHeader(ptr1));
			printf("Test failed.\n"); //makes sure free doesn't work
		//freeing pointer to middle of header
		case 8:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(getHeader(ptr1) + HEADER_SIZE - 1);
			printf("Test failed.\n"); //makes sure free doesn't work
		//freeing chunk that occupies all of heap.bytes
		case 9:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			ptr1 = malloc(size1 + size2 + size3 + size4 + size5 - HEADER_SIZE);
			free(ptr1);
			check(1, ptr1, 0, size1 + size2 + size3 + size4 + size5);
			break;
		//freeing chunk at beginning with used chunk after
		case 10:
			free(ptr1);
			check(1, ptr1, 0, size1);
			check(2, ptr2, 1, size2);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing chunk at beginning with free chunk after
		case 11:
			free(ptr2);
			free(ptr1);
			check(1, ptr1, 0, size1 + size2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing chunk at end with free chunk before
		case 12:
			free(ptr4);
			free(ptr5);
			check(4, ptr4, 0, size4 + size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			break;
		//freeing chunk at end with used chunk before
		case 13:
			free(ptr5);
			check(4, ptr4, 1, size4);
			check(5, ptr5, 0, size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			break;
		//freeing chunk in middle with used chunk before & after
		case 14:
			free(ptr3);
			check(2, ptr2, 1, size2);
			check(3, ptr3, 0, size3);
			check(4, ptr4, 1, size4);
			free(ptr1);
			free(ptr2);
			free(ptr4);
			free(ptr5);
			break;
		//freeing chunk in middle with used chunk before & free chunk after
		case 15:
			free(ptr4);
			free(ptr3);
			check(2, ptr2, 1, size2);
			check(3, ptr3, 0, size3 + size4);
			free(ptr1);
			free(ptr2);
			free(ptr5);
			break;
		//freeing chunk in middle with free chunk before & used chunk after
		case 16:
			free(ptr2);
			free(ptr3);
			check(2, ptr2, 0, size2 + size3);
			check(4, ptr4, 1, size4);
			free(ptr1);
			free(ptr4);
			free(ptr5);
			break;
		//freeing chunk in middle with free chunk before & after
		case 17:
			free(ptr2);
			free(ptr4);
			free(ptr3);
			check(2, ptr2, 0, size2 + size3 + size4);
			free(ptr1);
			free(ptr5);
			break;
		//freeing 2nd chunk when 1st chunk is free & 3rd chunk is used
		case 18:
			free(ptr1);
			free(ptr2);
			check(1, ptr1, 0, size1 + size2);
			check(3, ptr3, 1, size3);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing 2nd chunk when 1st chunk is used & 3rd chunk is free
		case 19:
			free(ptr3);
			free(ptr2);
			check(1, ptr1, 1, size1);
			check(2, ptr2, 0, size2 + size3);
			free(ptr1);
			free(ptr4);
			free(ptr5);
			break;
		//freeing 2nd chunk when 1st chunk is used & 3rd chunk is used
		case 20:
			free(ptr2);
			check(1, ptr1, 1, size1);
			check(2, ptr2, 0, size2);
			check(3, ptr3, 1, size3);
			free(ptr1);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing 2nd chunk when 1st chunk is free & 3rd chunk is free
		case 21:
			free(ptr1);
			free(ptr3);
			free(ptr2);
			check(1, ptr1, 0, size1 + size2 + size3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing 2nd to last chunk when prev chunk is free & last chunk is used
		case 22:
			free(ptr3);
			free(ptr4);
			check(3, ptr3, 0, size3 + size4);
			check(5, ptr5, 1, size5);
			free(ptr1);
			free(ptr2);
			free(ptr5);
			break;
		//freeing 2nd to last chunk when prev chunk is used & last chunk is free
		case 23:
			free(ptr5);
			free(ptr4);
			check(3, ptr3, 1, size3);
			check(4, ptr4, 0, size4 + size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			break;
		//freeing 2nd to last chunk when prev chunk is used & last chunk is used
		case 24:
			free(ptr4);
			check(3, ptr3, 1, size3);
			check(4, ptr4, 0, size4);
			check(5, ptr5, 1, size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr5);
			break;
		//freeing 2nd to last chunk when prev chunk is free & last chunk is free
		case 25:
			free(ptr3);
			free(ptr5);
			free(ptr4);
			check(3, ptr3, 0, size3 + size4 + size5);
			free(ptr1);
			free(ptr2);
			break;
		//freeing only middle chunk when 1st chunk is free & last chunk is used
		case 26:
			//combine middle 3 chunks
			free(ptr2);
			free(ptr3);
			free(ptr4);
			ptr2 = malloc(size2 + size3 + size4 - HEADER_SIZE);

			free(ptr1);
			free(ptr2);
			check(1, ptr1, 0, size1 + size2 + size3 + size4);
			check(5, ptr5, 1, size5);
			free(ptr5);
			break;
		//freeing only middle chunk when 1st chunk is used & last chunk is free
		case 27:
			//combine middle 3 chunks
			free(ptr2);
			free(ptr3);
			free(ptr4);
			ptr2 = malloc(size2 + size3 + size4 - HEADER_SIZE);
	
			free(ptr5);
			free(ptr2);
			check(1, ptr1, 1, size1);
			check(2, ptr2, 0, size2 + size3 + size4 + size5);
			free(ptr1);
			break;
		//freeing only middle chunk when 1st chunk is used & last chunk is used
		case 28:
			//combine middle 3 chunks
			free(ptr2);
			free(ptr3);
			free(ptr4);
			ptr2 = malloc(size2 + size3 + size4 - HEADER_SIZE);

			free(ptr2);
			check(1, ptr1, 1, size1);
			check(2, ptr2, 0, size2 + size3 + size4);
			check(5, ptr5, 1, size5);
			free(ptr1);
			free(ptr5);
			break;
		//freeing only middle chunk when 1st chunk is free & last chunk is free
		case 29:
			//combine middle 3 chunks
			free(ptr2);
			free(ptr3);
			free(ptr4);
			ptr2 = malloc(size2 + size3 + size4 - HEADER_SIZE);

			free(ptr1);
			free(ptr5);
			free(ptr2);
			check(1, ptr1, 0, size1 + size2 + size3 + size4 + size5);
			break;
		default:
			fprintf(stderr, "Invalid test case.\n");
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/*Checks to see if chunk has the expected used status and size.*/
void check(int chunkNumber, void *chunk, int expectedUsed, size_t expectedSize) {
	int *header = getHeader(chunk);
	int isUsed = header[USED];
	size_t chunkSize = header[SIZE_OF_CHUNK];
	expectedSize -= HEADER_SIZE;
	printf("Chunk %d:\n", chunkNumber);
	printf("Expected used: %d\n", expectedUsed);
	printf("Actual used: %d\n", isUsed);
	printf("Expected size: %zu\n", expectedSize);
	printf("Actual size: %zu\n", chunkSize);
	if((isUsed == expectedUsed) && (chunkSize == expectedSize))
	{
		printf("Test passed.\n\n");
	}
	else 
	{
		printf("Test failed.\n\n");
	}	
}

/*Prints a message indicating that an error message is expected.*/
void expectInvalid() {
	printf("Expected: Inappropriate pointer.\n");
}
