#include <stddef.h>
#include <stdio.h>

#include "mymalloc.h"

#define MEMLENGTH 4096
static union {
  char bytes[MEMLENGTH];
  double not_used;
} heap;

void *mymalloc(size_t size, char *file, int line) {
  size = (size + 7) & ~7;

  // Access two values of starting header
  int isUsed = *(int*) heap.bytes;
  int sizeOfPayload = *(int*) heap.bytes + 4;

  // Used for updating header
  int indexOfHeader = 0;

  // Used to update isUsed and sizeOfPayload while header jumping
  int tempSizeOfPayload = 0;

  // Header jumping: continue going to headers until free header
  // TODO: CHECK TO ENSURE NO INDEX-OUT-BOUNDS WHEN HEADER JUMPING TO THE END
  while ((isUsed != 0) || (sizeOfPayload >= size)) {
    tempSizeOfPayload = sizeOfPayload;
    indexOfHeader = 8 + sizeOfPayload;

    isUsed = (*(int*) heap.bytes) + 8 + sizeOfPayload;
    sizeOfPayload = (*(int*) heap.bytes) + 12 + tempSizeOfPayload;
  }

  // Update current header values
  int *isUsedPtr = heap.bytes + indexOfHeader;
  int *sizeOfPayloadPtr = heap.bytes + indexOfHeader + 4;
  *isUsedPtr = 1;
  *sizeOfPayloadPtr = size; 

  // Checking to ensure if creation of header is needed
  // TODO: Code logic is wrong. Need to ensure there is no header at sizeOfChunkandLocation
  int sizeOfChunkandLocation = indexOfHeader + 8 + sizeOfPayload
  if (MEMLENGTH - (sizeOfChunkandLocation) != 0) {
    // Point these pointers to supposed header in heap
    isUsedPtr = heap.bytes + indexOfHeader + *sizeOfPayloadPtr;
    sizeOfPayloadPtr = heap.bytes + indexOfHeader + 4 + *sizeOfPayloadPtr;
  }

}

int main() {
  malloc(3);
  printf("%d", *(int*) heap.bytes);
  return 0;
}
