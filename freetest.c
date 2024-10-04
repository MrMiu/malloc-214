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
			printf("Test failed.\n"); //control flow should not reach this point
			break;
		//freeing pointer to heap.bytes
		case 2:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(start);
			printf("Test failed.\n");
			break;
		//freeing pointer to end of heap.bytes:
		case 3:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(end);
			printf("Test failed.\n");
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
			printf("Test failed.\n");
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
			printf("Test failed.\n");
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
			printf("Test failed.\n");
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
			printf("Test failed.\n");
		//freeing pointer to middle of header
		case 8:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			expectInvalid();
			free(getHeader(ptr1) + HEADER_SIZE - 1);
			printf("Test failed.\n");
		//freeing chunk at beginning with used chunk after
		case 9:
			free(ptr1);
			check(1, ptr1, 0, size1);
			check(2, ptr2, 1, size2);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing chunk at beginning with free chunk after
		case 10:
			free(ptr2);
			free(ptr1);
			check(1, ptr1, 0, size1 + size2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing chunk at end with free chunk before
		case 11:
			free(ptr4);
			free(ptr5);
			check(4, ptr4, 0, size4 + size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			break;
		//freeing chunk at end with used chunk before
		case 12:
			free(ptr5);
			check(4, ptr4, 1, size4);
			check(5, ptr5, 0, size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			break;
		//freeing chunk in middle with used chunk before & after
		case 13:
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
		case 14:
			free(ptr4);
			free(ptr3);
			check(2, ptr2, 1, size2);
			check(3, ptr3, 0, size3 + size4);
			free(ptr1);
			free(ptr2);
			free(ptr5);
			break;
		//freeing chunk in middle with free chunk before & used chunk after
		case 15:
			free(ptr2);
			free(ptr3);
			check(2, ptr2, 0, size2 + size3);
			check(4, ptr4, 1, size4);
			free(ptr1);
			free(ptr4);
			free(ptr5);
			break;
		//freeing chunk in middle with free chunk before & after
		case 16:
			free(ptr2);
			free(ptr4);
			free(ptr3);
			check(2, ptr2, 0, size2 + size3 + size4);
			free(ptr1);
			free(ptr5);
			break;
		//freeing 2nd chunk when 1st chunk is free
		case 17:
			free(ptr1);
			free(ptr2);
			check(1, ptr1, 0, size1 + size2);
			check(3, ptr3, 1, size3);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing 2nd chunk when 1st chunk is used
		case 18:
			free(ptr2);
			check(1, ptr1, 1, size1);
			check(2, ptr2, 0, size2);
			check(3, ptr3, 1, size3);
			free(ptr1);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			break;
		//freeing 2nd to last chunk when last chunk is free
		case 19:
			free(ptr5);
			free(ptr4);
			check(4, ptr4, 0, size4 + size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			break;
		//freeing 2nd to last chunk when last chunk is used
		case 20:
			free(ptr4);
			check(4, ptr4, 0, size4);
			check(5, ptr5, 1, size5);
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr5);
			break;
		default:
			free(ptr1);
			free(ptr2);
			free(ptr3);
			free(ptr4);
			free(ptr5);
			fprintf(stderr, "Invalid test case.\n");	
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
