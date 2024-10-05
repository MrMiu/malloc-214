#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "mymalloc.c"

int main()
{
    void *ptr1;
    void* ptrs[120];

    struct timeval start;
    struct timeval end;

    gettimeofday(&start, 0);

	// First task: malloc() and immediately free() a 1-byte object, 120 times.
    for (int i = 0; i < 120; i++) {
        ptr1 = malloc(1);
        free(ptr1);
    }

    /* Second task: 
    * Use malloc() to get 120 1-byte objects, 
    * storing the pointers in an array, 
    * then use free() to deallocate the chunks
    */
    for (int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    } 
    for (int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }

    /* Third Task: Create an array of 120 pointers. 
     * Repeatedly make a random choice between allocating a 1-byte object 
     * and adding the pointer to the array and deallocating a previously allocated object 
     * (if any), until you have allocated 120 times. Deallocate any remaining objects.
    */


    /* FOURTH TASK: 
    
    */

    gettimeofday(&end, 0);
	size_t e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);

	printf("elapsed time: %zu microseconds\n", e_usec);
    return EXIT_SUCCESS;
}