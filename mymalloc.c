#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define HEADER_SIZE 2 * sizeof(int)
#define USED 0
#define SIZE_OF_CHUNK 1 
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
	header[USED] = 0;
	header[SIZE_OF_CHUNK] = MEMLENGTH - HEADER_SIZE;
}

void coalesce(int *prevHeader, int *nextHeader)
{
	prevHeader[SIZE_OF_CHUNK] += HEADER_SIZE + nextHeader[SIZE_OF_CHUNK];
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
	header[USED] = 0;
	if(chunk + header[SIZE_OF_CHUNK] < heap.bytes + MEMLENGTH)
	{
		int *nextHeader = (int *) (chunk + header[SIZE_OF_CHUNK]);
		if(!nextHeader[USED])
		{
			coalesce(header, nextHeader);
		}
	}
	char *prev;
	char *next = heap.bytes;
	while(next < header)
	{
		prev = next;
		int *prevHeader = (int *) prev;
		next = prev + HEADER_SIZE + prevHeader[SIZE_OF_CHUNK];
	}
	int *prevHeader = (int *) prev;
	if(!prevHeader[USED])
	{
		coalesce(prevHeader, header);
	}
}
