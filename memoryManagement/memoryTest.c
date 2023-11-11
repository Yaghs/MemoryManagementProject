/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.c to edit this template
 */

#include "fscMalloc.h"
#include "memoryTest.h"
#include <stdio.h>

int memoryTestOne() {
    const int fullAllocSize = 10 * MBToB;
    memoryStructure m;
    void* initialRoot = 0;
    void* firstAlloc = 0;
    void* secondAlloc = 0;
    void* thirdAlloc = 0;
    void* fourthAlloc = 0;
    void* fifthAlloc = 0;
    void* sixthAlloc = 0;
    // allocate 10 MB
    initialRoot = fscMemorySetup(&m, FIRST_FIT_RETURN_FIRST, fullAllocSize);
    if (0 == initialRoot) { // 0 if success
        fprintf(stderr, "Error on line %d\n", __LINE__);
        return __LINE__;
    }
    /* note: I assume that memoryStructure has a variable head that points to the free list */
    printFreeList(stderr, m.head);
    // first, ask for all the space. This should not work
    if (0 != fscMalloc(&m, fullAllocSize)) {
        fprintf(stderr, "Error on line %d\n", __LINE__);
        return __LINE__;
    }
    // For the second allocation, ask for 1mb. 
    secondAlloc = fscMalloc(&m, 1 * MBToB);
    // check the address returned looks like FIRST_FIT_RETURN_FIRST
    if (sizeof (fsc_alloc_header_t) + initialRoot != secondAlloc) {
        fprintf(stderr, "Error on line %d: initial root is %u size is %zu and secondAlloc is %u\n",
                __LINE__, initialRoot, sizeof (fsc_alloc_header_t), secondAlloc);
        return __LINE__;
    }
    /* now request a third block. Ask for 2mb */
    thirdAlloc = fscMalloc(&m, 2 * MBToB);
    // check the address returned looks like FIRST_FIT_RETURN_FIRST
    if (2 * sizeof (fsc_alloc_header_t) + initialRoot + 1 * MBToB != thirdAlloc) {
        fprintf(stderr, "Error on line %d\n",
                __LINE__);
        return __LINE__;
    }
    /* now a fourth block of 3 mb */
    fourthAlloc = fscMalloc(&m, 3 * MBToB);
    // check the address returned looks like FIRST_FIT_RETURN_FIRST
    if (3 * sizeof (fsc_alloc_header_t) + initialRoot + 3 * MBToB != fourthAlloc) {
        fprintf(stderr, "Error on line %d\n",
                __LINE__);
        return __LINE__;
    }
    /* now a fifth block of 1 mb */
    fifthAlloc = fscMalloc(&m, 1 * MBToB);
    if (4 * sizeof (fsc_alloc_header_t) + initialRoot + 6 * MBToB != fifthAlloc) {
        fprintf(stderr, "Error on line %d\n",
                __LINE__);
        return __LINE__;
    }
    /* now memory is [1mb+hdr][2mb+hdr][3mb+hdr][1mb+hdr][<3mb free] */
    /* try 3.5 wont work */
    /* 6th of 3.5 */
    sixthAlloc = fscMalloc(&m, (size_t) (3.5 * MBToB));
    if (0 != sixthAlloc) {
        fprintf(stderr, "Error on line %d\n",
                __LINE__);
        return __LINE__;
    }
    /* free the 3 still no work */
    fscFree(&m, fourthAlloc);
    /* now memory is [1mb+hdr][2mb+hdr][~3mb free][1mb+hdr][<3mb free] */

    printFreeList(stderr, m.head);
    sixthAlloc = fscMalloc(&m, (size_t) (3.5 * MBToB));
    if (0 != sixthAlloc) {
        fprintf(stderr, "Error on line %d\n",
                __LINE__);
        return __LINE__;
    }
    fprintf(stderr, "At line %d\n",__LINE__);
    printFreeList(stderr, m.head);
    /* now free the fifth block */
    /* now memory is [1mb+hdr][2mb+hdr][~3mb free][*** this GUY *** 1mb+hdr][<3mb free] */
    /* I expect lots of coallesce */
    fscFree(&m, fifthAlloc);
    /* now memory is [1mb+hdr][2mb+hdr][~7mb free] */
    printFreeList(stderr, m.head);
    /* now a 6 should alloc, and it should return the same as fourthAlloc */
    sixthAlloc = fscMalloc(&m, (size_t) (6 * MBToB));
    if (fourthAlloc != sixthAlloc) {
        fprintf(stderr, "Error on line %d\n",
                __LINE__);
        return __LINE__;
    }
    return 0;
}

int memoryTestTwo() {
}

int memoryTestThree() {
}