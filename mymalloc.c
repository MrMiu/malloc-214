#include <stddef.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mymalloc.h"

#define HEADER_SIZE (2 * sizeof(int))
#define USED 0
#define SIZE_OF_CHUNK 1 
#define MEMLENGTH 4096

static union {
  char bytes[MEMLENGTH];
  double not_used;
} heap;

void *mymalloc(size_t size, char *file, int line) {
  
  int newSize = (size + 7) & ~7;

  // Access two values of starting header
  int isUsed = *(int*) heap.bytes;
  int sizeOfPayload = *(int*) (heap.bytes + 4);

  // Used for updating header
  int currentHeaderLocation = 0;

  // Check if there is another header after current one
  int avaliableHeader = 0;

  // Find next header location
  int nextHeaderLocation = 0;

  // Header jumping: continue going to headers until free header
  // TODO: Potentially use pointers for currentHeaderLocation and nextHeaderLocation
  while (!(isUsed != 0) && !(sizeOfPayload >= newSize)) {
    currentHeaderLocation = HEADER_SIZE + sizeOfPayload;
    // If header location reaches end of array, cannot allocate bytes
    if (currentHeaderLocation >= MEMLENGTH) {
        size_t totalBytes = size * sizeof(size_t);
        fprintf(stderr, "Unable to allocate %zu bytes (%s:%d)\n", totalBytes, file, line);
        return NULL;
    }

    isUsed = (*(int*) heap.bytes) + currentHeaderLocation;
    sizeOfPayload = (*(int*) heap.bytes) + currentHeaderLocation + 4;
  }

  nextHeaderLocation = currentHeaderLocation + HEADER_SIZE + sizeOfPayload;

  if ((nextHeaderLocation + HEADER_SIZE) < MEMLENGTH) {
	avaliableHeader = 1;
  }

  // Update current header values
  int *isUsedPtr = (int*) heap.bytes + currentHeaderLocation;
  int *sizeOfPayloadPtr = (int*) heap.bytes + currentHeaderLocation + 4;
  *isUsedPtr = 1;
  *sizeOfPayloadPtr = newSize; 

  // CASE 1: If there are no more headers until the end of heap
  // CASE 2: If next header is directly after payload of current chunk
  if ((avaliableHeader == 0) || (nextHeaderLocation == currentHeaderLocation + HEADER_SIZE + newSize)) {
    return isUsedPtr;
  }

  printf("nextHeaderLocation: %d", nextHeaderLocation);
  // CASE 3: Create header inbetween current header and next header
  // Point these pointers to supposed header in heap
  int *nextIsUsedPtr = (int*) heap.bytes + currentHeaderLocation + HEADER_SIZE + newSize;
  int *nextSizeOfPayloadPtr = (int*) heap.bytes + currentHeaderLocation + HEADER_SIZE + 4 + newSize;

  // Create values for new header
  *nextIsUsedPtr = 0;
  *nextSizeOfPayloadPtr = nextHeaderLocation - (currentHeaderLocation + HEADER_SIZE);

  return isUsedPtr;
}

static int initialized = 0;

void initializeHeap();
void coalesce(int *prevHeader, int *nextHeader);
char isUsedChunk(void *ptr);
int *nextHeader(int *header);
int *prevHeader(int *header);
int *getHeader(void *ptr);

/* Sets initialized equal to 1.
Creates a chunk that occupies the entirety of heap.bytes. */
void initializeHeap()
{
	initialized = 1;
	int *header = (int *) heap.bytes;
	header[USED] = 0;
	header[SIZE_OF_CHUNK] = MEMLENGTH - HEADER_SIZE;
}

/* Coalesces the chunks corresponding to prevHeader and nextHeader.
Assumes prevHeader < nextHeader. */
void coalesce(int *prevHeader, int *nextHeader)
{
	prevHeader[SIZE_OF_CHUNK] += HEADER_SIZE + nextHeader[SIZE_OF_CHUNK];
}

/* Returns 1 if ptr points to the beginning of a used chunk. */
char isUsedChunk(void *ptr)
{
	char *headerPointer = heap.bytes;
	char *charPointer = (char *) ptr;
	while(headerPointer < heap.bytes + MEMLENGTH)
	{
		int *header = (int *) headerPointer;
		char *chunk = headerPointer + HEADER_SIZE;
		if(chunk >= charPointer)
		{
			return (chunk == charPointer) && header[USED];
		}
		headerPointer = chunk + header[SIZE_OF_CHUNK];
	}
	return 0;
}

/* Returns the header of the chunk after the given header.
Returns NULL if header is the last header in heap.bytes. */
int *nextHeader(int *header)
{
	char *next = (char *) header + HEADER_SIZE + header[SIZE_OF_CHUNK];
	if(next == heap.bytes + MEMLENGTH)
	{
		return NULL;
	}
	return (int *) next;
}

/* Returns the header of the chunk before the given header.
Returns NULL if header is the first header in heap.bytes. */
int *prevHeader(int *header)
{
	if((char *) header == heap.bytes)
	{
		return NULL;
	}
	char *prev;
	char *next = heap.bytes;
	while(next < (char *) header)
	{
		prev = next;
		int *prevHead = (int *) prev;
		next = (char *) nextHeader(prevHead);
	}
	return (int *) prev;
}

/* Returns the header of the chunk pointed to by ptr. */
int *getHeader(void *ptr)
{
	char *chunk = (char *) ptr;
	char *headerPointer = chunk - HEADER_SIZE;
	return (int *) headerPointer;
}

void myfree(void *ptr, char *file, int line)
{
	if(!initialized)
	{
		initializeHeap();
	}
	if(!isUsedChunk(ptr))
	{
		fprintf(stderr, "Inappropriate pointer (%s:%d)\n", file, line);
		exit(2);
	}
	int *header = getHeader(ptr);
	header[USED] = 0;
	int *next = nextHeader(header);
	if((next != NULL) && !next[USED])
	{
		coalesce(header, next);
	}
	int *prev = prevHeader(header);
	if((prev != NULL) && !prev[USED])
	{
		coalesce(prev, header);
	}
}

int main() {
    initializeHeap();
    char* asdf = malloc(sizeof(char) * 4088);
    asdf[0] = 'A';
    printf("%s\n", asdf);
    return 0;
}