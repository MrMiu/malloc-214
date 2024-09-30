#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HEADER_SIZE 2 * sizeof(int)
#define MEMLENGTH 4096
static union {
  char bytes[MEMLENGTH];
  double not_used;
} heap;
static int initialized = 0;

void initializeHeap()
{
	initialized = 1;
	int *header = (int *) heap.bytes;
	header[0] = 0;
	header[1] = MEMLENGTH - HEADER_SIZE;
}

void myfree(void *ptr, char *file, int line)
{
	if(!initialized)
	{
		initializeHeap();
	}
	int isAllocatedChunk = 0;
	char *headerPointer = heap.bytes;
	while(headerPointer < heap.bytes + MEMLENGTH)
	{
		int *header = (int *) headerPointer;
		int *used = header;
		char *chunk = headerPointer + HEADER_SIZE;
		if(chunk >= ptr)
		{
			isAllocatedChunk = (chunk == ptr) && (*used);
			break;
		}
		int *sizeOfChunk = header + 1;
		headerPointer = chunk + *sizeOfChunk;
	}
	if(!isAllocatedChunk)
	{
		fprintf(stderr, "Inappropriate pointer (%s:%d)\n", file, line);
		exit(2);
	}
	char *chunk = (char *) ptr;
	int *header = (int *) (startOfChunk - HEADER_SIZE);
	header[0] = 0;
	if(chunk + header[1] < heap.bytes + MEMLENGTH)
	{
		int *nextHeader = (int *) (chunk + header[1]);
		if(!nextHeader[0])
		{
			header[1] += + HEADER_SIZE + nextHeader[1];
		}
	}
	char *prev;
	char *next = heap.bytes;
	while(next < header)
	{
		prev = next;
		int *prevHeader = (int *) prev;
		int prevChunkSize = prevHeader[1];
		next = prev + HEADER_SIZE + prevChunkSize;
	}
	int *prevHeader = (int *) prev;
	if(!prevHeader[0])
	{
		prevHeader[1] += HEADER_SIZE + header[1];
	}
}
