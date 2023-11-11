/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   fscMalloc.h
 * Author: student
 *
 * Created on March 15, 2023, 8:33 PM
 */

#ifndef FSCMALLOC_H
#define FSCMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

 
    #include <stddef.h> /* for size_t */
    #include <stdio.h> /* for FILE* */
    // Internal Types: (from the book, but with better names)
 
    // alloc_header: prepends each returned allocated block
    typedef struct {
        /* the size as the user sees (minus the header) */
        size_t size;
        int magic;
    } fsc_alloc_header_t;
    
    
    // free list node. 
    typedef struct _fsc_node_t {
        /* The size of memory AFTER THIS node */
        size_t size;
        struct _fsc_node_t* next;
    } fsc_free_node_t;
    
    
    // The structure we pass to the functions

    typedef struct {
        /* you should not use global variables. Therefore 
         * you use this structure to retain any context/state your
         * memory allocator needs (e.g., the root of the free list). 
         * Please add anything you need, and
         * the test code will pass this to your functions
         */
        fsc_free_node_t* head;
        int magicNumber;
    } memoryStructure;
    // My stuff

    typedef enum {
        // find the first space big enough in the free list, split into
        // [returned piece][kept on free list]
        FIRST_FIT_RETURN_FIRST,
        // find the first space big enough in the free list, split into
        // [kept on free list][returned piece]
        FIRST_FIT_RETURN_SECOND,
        // find the smallest space big enough for the block, split into
        // [returned piece][kept on free list]
        BEST_FIT_RETURN_FIRST,
        // find the smallest space big enough for the block, split into
        // [kept on free list][returned piece]
        BEST_FIT_RETURN_SECOND,
        // find the biggest space big enough for the block, split into
        // [returned piece][kept on free list]
        WORST_FIT_RETURN_FIRST,
        // find the biggest space big enough for the block, split into
        // [kept on free list][returned piece]
        WORST_FIT_RETURN_SECOND,
        // starting from where the last allocation occurred on the free list, 
        // find the smallest space big enough for the block, split into
        // [returned piece][kept on free list]
        NEXT_FIT_RETURN_FIRST,
        // starting from where the last allocation occurred on the free list, 
        // find the smallest space big enough for the block, split into
        // [kept on free list][returned piece]
        NEXT_FIT_RETURN_SECOND
    } fscAllocationMethod;
    // for convenience--you can specify in MB by multiplying by this
    // e.g., fscMemorySetup(... , 2*MBToB) allocates 2 megabytes
    enum { MBToB = 1048576 };
    // returns the root of the internal free list initially allocated, or 0 if failure
    void* fscMemorySetup(memoryStructure*, fscAllocationMethod, size_t sizeInBytes);
    void* fscMalloc(memoryStructure*, size_t sizeInBytes); // returns memory, 0 if failure
    void fscFree(memoryStructure*, void *); // returns memory to the pool
    void fscMemoryCleanup(memoryStructure*);

    /* a util function I wrote for you to print given the head. You may 
     need to modify calls to this in the test code if your 
     memoryStructure does not have a ptr head for the free list. */
    void printFreeList(FILE * out, fsc_free_node_t* head);



#ifdef __cplusplus
}
#endif

#endif /* FSCMALLOC_H */

