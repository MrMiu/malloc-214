#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "mymalloc.c"

/* First task */

/*
*  malloc() and immediately free() a 1-byte object, 120 times. 
*/
void workloadOne() {
    void* ptr1;
    for (int i = 0; i < 120; i++) {
        ptr1 = malloc(1);
        free(ptr1);
    }
}

/* Second task */

/* 
*  Use malloc() to get 120 1-byte objects, 
*  storing the pointers in an array, 
*  then use free() to deallocate the chunks
*/
void workloadTwo() {
    void* ptrs[120];
    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    } 
    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }
}

/* Third Task */

/* 
*  Create an array of 120 pointers. 
*  Repeatedly make a random choice between allocating a 1-byte object 
*  and adding the pointer to the array and deallocating a previously allocated object 
*  (if any), until you have allocated 120 times. Deallocate any remaining objects. 
*/
void workloadThree() {
    // Array to hold pointers
    void* ptrs[120];
    // Array to keep track of indices of allocated pointers
    short indicesUsed[120];
    // Count of allocated pointers
    short allocatedPointers = 0;  
    // Count of total allocations
    short totalAllocation = 0;    

    short random = 0;
    short indexToFree = 0;

    srand(time(NULL));

    while (totalAllocation < 120) {
        random = rand() % 2;
        // Allocate an object
        if (random == 0) {
            ptrs[totalAllocation] = malloc(1);
            // Track indices
            indicesUsed[allocatedPointers] = totalAllocation;
            totalAllocation++;
            allocatedPointers++;
        // Deallocate an existing object
        } else if (allocatedPointers > 0) {
            // Get a random index to free
            random = rand() % allocatedPointers;
            // Get the index of the pointer to free
            indexToFree = indicesUsed[random];
            
            // Free the allocated pointer
            free(ptrs[indexToFree]);
            
            // Shift remaining indices down so that random will always be consistant
            for (short i = random; i < allocatedPointers - 1; i++) {
                indicesUsed[i] = indicesUsed[i + 1];
            }
            allocatedPointers--; // Decrement the count of allocated pointers
        }
    }

    // Deallocate any remaining objects
    for (short i = 0; i < allocatedPointers; i++) {
        free(ptrs[indicesUsed[i]]);
    }
}

/* Fourth task */

/* Create 5 pointers in random positions with random sizes,
   then deallocate those 5 pointers in a random order for 120 times. */

/* 
*  Required in workload four in order to randomly free pointers.
*  Shuffles elements inside of array 
*/
void shuffle(int* array, int size) {
    int random = 0;
    int temp = 0;
    for (int i = size - 1; i > 0; i--) {
        random = rand() % (i + 1);
        temp = array[i];
        array[i] = array[random];
        array[random] = temp;
    }
}

void workloadFour() {
    short const AMOUNT_OF_PTRS = 8;
    short const MAX_SIZE = (MEMLENGTH - (HEADER_SIZE * AMOUNT_OF_PTRS)) / AMOUNT_OF_PTRS;
    void* ptrs[8];
    
    // indexs to choose for random
    int numbers[] = {0, 1, 2, 3, 4, 5, 6, 7};

    short random = 0;
    srand(time(NULL));

    // Allocate 8 pointers with random size
    for (int j = 0; j < AMOUNT_OF_PTRS; j++) {
        random = rand() % MAX_SIZE;
        ptrs[j] = malloc(random);
    }

    shuffle(numbers, AMOUNT_OF_PTRS);

    // Deallocate 8 pointers in random order
    for (int k = 0; k < AMOUNT_OF_PTRS; k++) {
        int indexToRemove = numbers[k];
        free(ptrs[indexToRemove]);
    }   
}

/* Fifth Task */

/*
* Allocate 120 objects that are 1 byte, 
* then free every other object from the heap
* before freeing the other half
*/
void workloadFive() {
    void* ptrs[120];
    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    }
    // Free the first half
    for (int i = 0; i < 120; i+=2) {
        free(ptrs[i]);
    }
    // Free the second half
    for (int i = 1; i < 120; i+=2) {
        free(ptrs[i]);
    }
}

/* Get the average time taken to run a function (uses 50 trials) */
void getAverageTime(void (*func)()) {
    struct timeval start;
    struct timeval end;

    const short AMOUNT_OF_TRIALS = 50;

    size_t total = 0;

    for (int i = 0; i < AMOUNT_OF_TRIALS; i++) {
        gettimeofday(&start, 0);
        func();
        gettimeofday(&end, 0);
        total += ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
    }
    printf("Average Elapsed Time: %zu microseconds\n", total / AMOUNT_OF_TRIALS);
}

int main(int argc, char **argv)
{
    if(argc < 2)
	{
		fprintf(stderr, "No test case given.\n");
		exit(EXIT_FAILURE);
	}
	int testCase = atoi(argv[1]);

    // Test cases pretain to workload method
	switch(testCase)
	{
        case 1:
            getAverageTime(workloadOne);
            break;
        case 2:
            getAverageTime(workloadTwo);
            break;
        case 3:
            getAverageTime(workloadThree);
            break;
        case 4:
            getAverageTime(workloadFour);
            break;
        case 5:
            getAverageTime(workloadFive);
            break;
        default:
            fprintf(stderr, "Invalid test case.\n");
            return EXIT_FAILURE;
    }
	    
    return EXIT_SUCCESS;
}