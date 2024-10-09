# malloc-214
Alexander Ruan (netID: ajr331)
Sanjeev Harikrishnan (netID: sh1720)

Design:
Each chunk in the heap consists of an 8 byte header followed by the payload. Each chunk's header is treated as an array of two integers. The first integer, header[0], equals 1 when the chunk is used and 0 when it is free. The second integer, header[1], equals the size of the payload. Payload sizes requested by the user are rounded up to the nearest multiple of eight, and when the user requests exactly eight bytes less than the size of a free chunk, eight bytes are added to the payload rather than creating a header for a chunk with no payload.  

Test plan:
In addition to memgrind.c, we created test programs for our malloc, free, and leak detector functions named malloctest.c, freetest.c, and leaktest.c, respectively. Each test program can be compiled using the appropriate make command. Each test program contains several test cases and takes a single integer argument from the command line indicating the test case to be run. Some of the test programs directly include the source code of mymalloc.c rather than just the header file mymalloc.h in order to access internal functions that can read from the header of each chunk as well as constants used to determine the bounds of the heap to allow for edge case testing.

malloctest.c:
    HOW TO USE:
        make malloctest
        ./malloctest <TEST CASE>
        (1 <= <TEST CASE> <= 9)

    SUMMARY:
        The goal of the malloc test plan was to test every edge case possible when using malloc,
        ensuring the method works as intended in every case.

    TEST CASE 1:
        Allocate one singular, big chunk that fills up the heap
    TEST CASE 2:
        Allocate the maximum amount of chunks possible in the heap. 
        This would amount to 32, 16-byte chunks
    TEST CASE 3: 
        Case where user tries to allocate a chunk that is too large for the heap.
        Result should lead to an error
    TEST CASE 4:
        Case where user tries to allocate a chunk that is too large when there is supposed
        space at the end of the heap. 
        PROCESS: 
            Allocates a big chunk that leaves 8 bytes at the end
            of the heap, then try to allocate another, 1 byte chunk. Fails as minimum size of
        chunk is 16 bytes
    TEST CASE 5:
        Case where user tries to allocate a chunk that is too large when the only space
        avaliable for the chunk is in-between two other chunks. 
        PROCESS:
            Allocates 3, 16-byte (minimum sized) chunks into the heap. Then allocate a chunk to fill the rest of the heap. Free the second, small chunk in-between the two other 16-byte chunks. Finally, attempt to
            allocate a chunk which is larger than the space in-between the two, minimum-sized chunks
    TEST CASE 6:    
        Case which checks that malloc will create header at end of chunk given the space.
        PROCESS:
            Allocate a minimum-sized chunk, then check to make sure the header after the chunk
            has the proper information (used and payload).
    TEST CASE 7:
        Case where header creates a header at the end of the heap (with size 0 payload)
        PROCESS:
            Create a large chunk which gives 16 bytes for the heap. Malloc will then
            create a header at the end of the chunk which will have a payload size 0
            as 8-bytes are the minimum for a header, meaning no other objects can be
            added. 
    TEST CASE 8:
        Ensure that header does not override other headers when chunks are connected
        PROCESS:
            Create 2, 16-byte (minimum sized) chunks. Free the first chunk before adding a
            third, 16-byte chunk. Ensure that the third chunk is properly placed at the beginning
            of the heap as a means to ensure header information is correct.
    TEST CASE 9:
        Ensure writing to objects writes to the correct location
        PROCESS:
            Create 8, 504-byte (maximum sized for 8) chunks. Write a distinct byte-pattern for
            the 8 bytes while ensuring that the byte pattern for each chunk is correct.

freetest.c:
    HOW TO USE:
        make freetest
        ./freetest <TEST CASE>
        (1 <= <TEST CASE> <= 20)
    
    SUMMARY:
        This program first makes a series of malloc calls that partition the heap into 5 chunks of different sizes. Paritioning the heap into five chunks allows for separate testing of the behavior of free when called on chunks located at the beginning or edge of the heap, chunks adjacent to these chunks, and chunks located in the middle. The program has 20 test cases. The first 8 cases call free on inappropriate pointers and verify that an error message is given. The last 12 cases call free on various subsets of the five chunks in different orders to ensure proper freeing and coalescing in all possible cases (coalescing with a free chunk after, before, both, or neither). At the end of each test (except the first eight), the size and used status stored in each header are checked and compared to their expected values. All pointers are guaranteed to be free before the program exits in order to avoid memory leaks; leak detection is tested separately in leaktest.c.
    
    TEST CASE 1:
        Frees a pointer to a location before the beginning of the heap.
    TEST CASE 2:
        Frees a pointer to the beginning of the heap.
    TEST CASE 3:
        Frees pointer to the end of the heap.
    TEST CASE 4:
        Frees a pointer to a location after the end of the heap.
    TEST CASE 5:
        Frees a pointer that is already free (double freeing).
    TEST CASE 6:
        Frees a pointer to the middle of a chunk.
    TEST CASE 7:
        Frees a pointer to the beginning of a header.
    TEST CASE 8:
        Frees a pointer to the middle of a header.
    TEST CASE 9:
        Frees the first chunk in the heap when the second chunk is used.
    TEST CASE 10:
        Frees the first chunk in the heap when the second chunk is free.
    TEST CASE 11:
        Frees the last chunk in the heap when the previous chunk is free.
    TEST CASE 12:
        Frees the last chunk in the heap when the previous chunk is used.
    TEST CASE 13:
        Frees the middle chunk in the heap when all other chunks are used.
    TEST CASE 14:
        Frees the middle chunk in the heap when the chunk before is used and the chunk after is free.
    TEST CASE 15:
        Frees the middle chunk in the heap when the chunk before is free and the chunk after is used.
    TEST CASE 16:
        Frees the middle chunk in the heap when the chunks before and after are free.
    TEST CASE 17:
        Frees the second chunk in the heap when the first chunk is free and the third chunk is used.
    TEST CASE 18:
        Frees the second chunk in the heap when the first chunk is used and the third chunk is used.
    TEST CASE 19:
        Frees the second-to-last chunk in the heap when the last chunk is free.
    TEST CASE 20:
        Frees the second-to-last chunk in the heap when the last chunk is used.

leaktest.c
    HOW TO USE:
        make leaktest
        ./leaktest <TEST CASE>
        (1 <= <TEST CASE> <= 7)
    
    SUMMARY:
        This program consists of seven test cases that each call malloc multiple times and leave different numbers of objects with different numbers of bytes unfreed. A message is then printed indicating the expected number of leaked objects and bytes, and then the program exits, allowing the leak checker to print the actual number of leaked objects and bytes, if necessary.
    
    TEST CASE 1:
        Does not create any objects, makes sure leak detector prints nothing.
    TEST CASE 2:
        Creates 3 objects and frees all of them, makes sure leak detector prints nothing.
    TEST CASE 3:
        Creates one object and leaves it unfreed.
    TEST CASE 4:
        Creates multiple objects that take up the entire heap and leaves the last one unfreed.
    TEST CASE 5:
        Creates multiple objects and leaves two adjacent objects unfreed.
    TEST CASE 6:
        Creates multiple objects and leaves two non-adjacent objects unfreed (two unfreed chunks with a free chunk in between).
    TEST CASE 7:
        Creates an object with a size of 7 and leaves it unfreed. Ensures that leak checker detects an object of size 8 (rounds up).

memgrind.c:
    HOW TO USE:
        make memgrind
        ./memgrind <TEST CASE>
        (1 <= <TEST CASE> <= 9)

    TEST CASE 1:
        malloc() and immediately free() a 1-byte object, 120 times.
    TEST CASE 2:
        Use malloc() to get 120 1-byte objects, storing the pointers in an array, 
        then use free() to deallocate the chunks.
    TEST CASE 3:
        Create an array of 120 pointers. Repeatedly make a random choice between 
        allocating a 1-byte object and adding the pointer to the array 
        and deallocating a previously allocated object (if any), until you have allocated 120 times. 
        Deallocate any remaining objects.
    TEST CASE 4:
        Create 5 pointers in random positions with random sizes,
        then deallocate those 5 pointers in a random order.
        PURPOSE:
            This was created as a more consistant version of TEST CASE 3, while still
            involving the creation and removal of multiple objects. This is also able
            to deallocate objects in-between other objects which our implementation of
            TEST CASE 3 cannot do.
    TEST CASE 5:
        Allocate 120 objects that are 1 byte, then free every other object from the heap
        before free the second half from the middle towards the two ends of the heap
        PURPOSE:
            The test case mentioned is mainly to test how fast coalesce works in our free.
            By creating multiple gaps within a full heap, and then freeing objects starting
            from the middle, that will maximize the amount of coalesce within our heap.