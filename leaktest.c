#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.h"

#define MEMSIZE 4096
#define HEADERSIZE 8
#define SIZE1 8
#define SIZE2 16
#define SIZE3 24

void check(size_t bytesLeaked, int objectsLeaked); 

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "No test case given.\n");
        exit(EXIT_FAILURE);
    }
    int testCase = atoi(argv[1]);
	void *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;
    switch(testCase) {
        //no objects created
        case 1:
            check(0, 0);
            break;
        //all objects are freed
        case 2:
            ptr1 = malloc(SIZE1);
            ptr2 = malloc(SIZE2);
            ptr3 = malloc(SIZE3);
            free(ptr3);
            free(ptr1);
            free(ptr2);
            check(0, 0);
            break;
        //first object not freed
        case 3:
            ptr1 = malloc(SIZE1);
            check(SIZE1, 1);
            break;
        //last object not freed
        case 4:
            ptr1 = malloc(SIZE1);
            ptr2 = malloc(MEMSIZE - SIZE1 - 2 * HEADERSIZE);
            free(ptr1);
            check(MEMSIZE - SIZE1 - 2 * HEADERSIZE, 1);
            break;
        //adjacent objects not freed
        case 5:
            ptr1 = malloc(SIZE1);
            ptr2 = malloc(SIZE2);
            ptr3 = malloc(SIZE3);
            check(SIZE1 + SIZE2 + SIZE3, 3);
            break;
        //non-adjacent objects not freed
        case 6:
            ptr1 = malloc(SIZE1);
            ptr2 = malloc(SIZE2);
            ptr3 = malloc(SIZE3);
            ptr4 = malloc(SIZE1);
            ptr5 = malloc(SIZE2);
            free(ptr2);
            free(ptr4);
            check(SIZE1 + SIZE3 + SIZE2, 3);
            break;
        //object with size not multiple of 8
        case 7:
            ptr1 = malloc(SIZE1 - 1);
            check(SIZE1, 1);
            break;  
        default:
            fprintf(stderr, "Invalid test case.\n");    
    }
    return EXIT_SUCCESS;
}

/*Prints message indicating expected leaks.*/
void check(size_t bytesLeaked, int objectsLeaked) {
    if((bytesLeaked > 0) || (objectsLeaked > 0)) {
        printf("Expected: %zu bytes leaked in %d objects\n", bytesLeaked, objectsLeaked);
    } else {
        printf("No leaks expected\n");
    } 
}
