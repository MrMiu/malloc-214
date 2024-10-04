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


static int initialized = 0;

void initializeHeap();
void coalesce(int *prevHeader, int *nextHeader);
char isUsedChunk(void *ptr);
int *nextHeader(int *header);
int *prevHeader(int *header);
int *getHeader(void *ptr);
void detectLeak();

// Detect the amount of objects left in heap at the end of program along with amount of bytes
void detectLeak() {
	char* currentHeaderLocation = heap.bytes;

	// Get values from header
	int isUsed = *(int*) currentHeaderLocation;
	int sizeOfPayload = *(int*) (currentHeaderLocation + sizeof(int));

	// Track bytes leaked and objects leaked
	int bytesLeaked = 0;
	int objectsLeaked = 0;
	
	// Header jumping till the end of heap
	while (currentHeaderLocation != NULL) {
		isUsed = *(int*) currentHeaderLocation;
		sizeOfPayload = *(int*) (currentHeaderLocation + sizeof(int));
		if (isUsed) {
			bytesLeaked = sizeOfPayload;
			objectsLeaked++;
		}

		currentHeaderLocation = (char*) (nextHeader((int*) currentHeaderLocation));
	}

	if (bytesLeaked || objectsLeaked) {
		fprintf(stderr, "mymalloc: %d bytes leaked in %d objects\n", bytesLeaked, objectsLeaked);
	}
}

/* Sets initialized equal to 1.
Creates a chunk that occupies the entirety of heap.bytes. */
void initializeHeap()
{
	initialized = 1;
	int *header = (int *) heap.bytes;
	header[USED] = 0;
	header[SIZE_OF_CHUNK] = MEMLENGTH - HEADER_SIZE;
	atexit(detectLeak);
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

void *mymalloc(size_t size, char *file, int line) {
	// Ensure heap is initalized
	if (!initialized) {
		initializeHeap();
	}

	// Resized size to be a multiple of 8
    size_t newSize = (size + 7) & ~7;

    // Holds index of current header
    char* currentHeaderLocation = heap.bytes;

	// Ensure we do not reach end of heap
    while (currentHeaderLocation < &heap.bytes[MEMLENGTH]) {
		// Value of isUsed in current header
        int isUsed = *(int*) currentHeaderLocation;
		// Value of sizeOfPayload in current header
        int sizeOfPayload = *(int*) (currentHeaderLocation + sizeof(int));

		// Header jumping: continue going to headers until free header
        if (!(isUsed) && (sizeOfPayload >= newSize)) {
            // If the chunk is large enough, split it if there is remaining space
            int remainingSize = sizeOfPayload - newSize - HEADER_SIZE;

            // Update values of header
            *(int*) currentHeaderLocation = 1;
            *(int*) (currentHeaderLocation + sizeof(int)) = newSize;

            // If there's remaining space, create a new free header
            if (remainingSize > 0) {
                char *nextHeaderLocation = currentHeaderLocation + HEADER_SIZE + newSize;

                *(int*) nextHeaderLocation = 0;
                *(int*) (nextHeaderLocation + sizeof(int)) = remainingSize;
            }

            return currentHeaderLocation + HEADER_SIZE;
        }

        // Move to the next header
        currentHeaderLocation = (char*) (nextHeader((int*) currentHeaderLocation));
		// No more headers
		if (currentHeaderLocation == NULL) {
            break;
        }
    }

    // If no suitable chunk was found
    fprintf(stderr, "Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
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
