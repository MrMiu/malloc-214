#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.c"

void check(int chunkNumber, void *chunk, int expectedUsed, size_t expectedSize);
void expectInvalid(); 

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "No test case given.\n");
        exit(EXIT_FAILURE);
    }
    int testCase = atoi(argv[1]);

    switch(testCase) {
        // Allocate maximum-sized chunk for entire heap
        case 1:
            void* ptr1 = malloc(MEMLENGTH - HEADER_SIZE);
            check(1, ptr1, 1, MEMLENGTH);
            free(ptr1);
            break;
        // Allocate minimum-sized chunks for entire heap
        case 2:
            int maxAmountOfChunks = MEMLENGTH / (HEADER_SIZE*2);
            // Cannot declare variable amount for array
            void* ptrs[MEMLENGTH / (HEADER_SIZE*2)];
            for (int i = 0; i < maxAmountOfChunks; i++) {
                ptrs[i] = malloc(1);
            }
            for (int i = 0; i < maxAmountOfChunks; i++) {
                check(i, ptrs[i], 1, HEADER_SIZE * 2);
                free(ptrs[i]);
            }
            break;
        // Failed malloc - Too big
        case 3:
            int maxSizeAddOne = (MEMLENGTH - HEADER_SIZE) + 1;
            expectInvalid(maxSizeAddOne);
            void* ptr3 = malloc(maxSizeAddOne);
            free(ptr3);
            break;
        // Failed malloc - Not enough space to allocate at end of heap
        case 4: 
            expectInvalid(HEADER_SIZE);
            void* bigPtr4 = malloc(MEMLENGTH - (HEADER_SIZE * 2));
            void* invalidPtr4 = malloc(HEADER_SIZE);
            free(bigPtr4);
            free(invalidPtr4);
            break;
        // Failed malloc - Not enough space in middle of heap
        case 5: 
            expectInvalid(HEADER_SIZE + 1);
            void *firstPtr5 = malloc(HEADER_SIZE);
            void *secondPtr5 = malloc(HEADER_SIZE);
            void *thirdPtr5 = malloc(HEADER_SIZE);
            // Subtract chunks first through third and header that has to be created
            void *bigPtr5 = malloc(MEMLENGTH - ((HEADER_SIZE*2)*3) - HEADER_SIZE);
            free(secondPtr5);
            void *failPtr = malloc(HEADER_SIZE + 1);
            free(firstPtr5);
            free(thirdPtr5);
            free(bigPtr5);
            free(failPtr);
            break;
        // Ensure header creates another header when given space
        case 6:
            char *ptr6 = malloc(HEADER_SIZE);
            char *nextHeaderPtr6 = heap.bytes + (HEADER_SIZE*3);
            check(1, nextHeaderPtr6, 0, MEMLENGTH - 16);
            free(ptr6);
            break;
        // Ensure header creates header with payload size 0 at end of heap
        case 7: 
            char *ptr7 = malloc(MEMLENGTH - (HEADER_SIZE*2));
            char *nextHeaderPtr7 = heap.bytes + MEMLENGTH;
            check(1, nextHeaderPtr7, 0, HEADER_SIZE);
            free(ptr7);
            break;
        // Ensure header does not overrwrite a header that is directly connected
        case 8:
            char *firstPtr8 = malloc(HEADER_SIZE);
            char *secondPtr8 = malloc(HEADER_SIZE);
            free(firstPtr8);
            char *thirdPtr8 = malloc(HEADER_SIZE);
            check(2, secondPtr8, 1, HEADER_SIZE*2);
            free(secondPtr8);
            free(thirdPtr8);
            break;
        // Several large objects with distinct byte pattern
        case 9:
            char *firstPtr9 = malloc((MEMLENGTH / 8) - 8);
            char *secondPtr9 = malloc((MEMLENGTH / 8) - 8);
            char *thirdPtr9 = malloc((MEMLENGTH / 8) - 8);
            char *fourthPtr9 = malloc((MEMLENGTH / 8) - 8);
            char *fifthPtr9 = malloc((MEMLENGTH / 8) - 8);
            char *sixthPtr9 = malloc((MEMLENGTH / 8) - 8);
            char *seventhPtr9 = malloc((MEMLENGTH / 8) - 8);
            char *eighthPtr9 = malloc((MEMLENGTH / 8) - 8);

            for (int i = 0; i < (MEMLENGTH / 8) - 8; i++) {
                // Write a byte pattern to each pointer
                firstPtr9[i] = '0';
                secondPtr9[i] = '1';
                thirdPtr9[i] = '2';
                fourthPtr9[i] = '3';
                fifthPtr9[i] = '4';
                sixthPtr9[i] = '5';
                seventhPtr9[i] = '6';
                eighthPtr9[i] = '7';

                // Ensure bytes are being properly assigned
                if ((firstPtr9[i] != '0') ||
                    (secondPtr9[i] != '1') ||
                    (thirdPtr9[i] != '2') ||
                    (fourthPtr9[i] != '3') ||
                    (fifthPtr9[i] != '4') ||
                    (sixthPtr9[i] != '5') ||
                    (seventhPtr9[i] != '6') ||
                    (eighthPtr9[i] != '7')) {
                        fprintf(stderr, "BYTES NOT PROPERLY WRITTEN\n");
                        free(firstPtr9);
                        free(secondPtr9);
                        free(thirdPtr9);
                        free(fourthPtr9);
                        free(fifthPtr9);
                        free(sixthPtr9);
                        free(seventhPtr9);
                        free(eighthPtr9);
                        return EXIT_FAILURE;
                }
            }
            printf("SUCCESS\n");
            free(firstPtr9);
            free(secondPtr9);
            free(thirdPtr9);
            free(fourthPtr9);
            free(fifthPtr9);
            free(sixthPtr9);
            free(seventhPtr9);
            free(eighthPtr9);
            break;
        default:
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
    if((isUsed == expectedUsed) && (chunkSize == expectedSize)) {
        printf("Test passed.\n\n");
    } else {
        printf("Test failed.\n\n");
    }   
}

/*Prints a message indicating that an error message is expected.*/
void expectInvalid(size_t size) {
    printf("Expected: Unable to allocate %zu bytes\n", size);
    printf("Expected: Inappropriate pointer.\n");
}
