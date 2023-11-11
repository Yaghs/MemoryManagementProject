/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.c to edit this template
 */

#include "fscMalloc.h"
#include <assert.h>
#include <sys/mman.h>
#include <stdlib.h> /* for rand() */
#include <time.h> /* for the init of rand */
#include <stddef.h> /* for size_t */


//The First Fit algorithm is a memory allocation algorithm that allocates memory blocks to processes or programs as they are requested. The algorithm searches for the first free partition that is large enough to accommodate the process. 
//The First Fit algorithm is part of the broader category of partitioning algorithms, where the system's memory is divided into fixed-sized or variable-sized partitions. 
//The First Fit algorithm is fast because it uses the first available hole that is large enough to accommodate the process. However, it has some disadvantages, including: 
//Poor performance in highly fragmented memory
//May lead to poor memory utilization
//May allocate larger blocks of memory than required

// define MAGIC_NUMBER 0xFACEBOOC is used for memory corruption detection by serving as a unique identifier to verify the integrity of allocated memory blocks.

// you will need this to initialize memory 

#define MAGIC_NUMBER 0xFACEB00C // Define a magic number for validation

// Function to initialize the memory structure
void* fscMemorySetup(memoryStructure* m, fscAllocationMethod am, size_t sizeInBytes) {
    // Only support FIRST_FIT_RETURN_FIRST allocation method
    if (FIRST_FIT_RETURN_FIRST != am) {
        fprintf(stderr, "This code only supports the FIRST_FIT_RETURN_FIRST allocation method\n");
        return 0;
    }

    // Allocate the initial memory block
    m->head = (fsc_free_node_t*)malloc(sizeInBytes);
    if (m->head == NULL) {
        return 0; // Return null if allocation fails
    }
    
    //Size_t is an unsigned integer type that is defined in the stddef.h header file. It is used to represent the size of objects in bytes. The sizeof
    //operator yields a result of some unsigned integer type. On a 32-bit
    //system, size_t will be 32 bits, on a 64-bit
    //system it will be 64 bits. It means a variable of size_t type can safely store a pointer.
    
    //EX: int array[] = {1, 2, 3, 4, 5};
         //size_t s = sizeof(array) / sizeof(array[0]);
         //printf("Size of array: %zu\n", s);
         //return 0;
    
    
    m->head->size = sizeInBytes - sizeof(fsc_free_node_t); // Set the size of the free block
    m->head->next = NULL; // There is no next free block yet since thats this is our very first block
    m->magicNumber = MAGIC_NUMBER; // Set the magic number for validation

    return m->head + 1; // Return a pointer to the memory after the free node
}

// Function to allocate memory from the managed structure
void* fscMalloc(memoryStructure* m, size_t requestedSizeInBytes) {
    
    //NOTE: YOU COULD USE assert(m->magicNumber == MAGIC_NUMBER) and its a more complex way of Verifying the magic number
    //But lazy programming for the win
     if (m->magicNumber != MAGIC_NUMBER) {
        return NULL;
    }
     // Ironically from what I learned on how Malloc is done, it functions similarly to a linked list except the list is a block of memory waiting for allocation
    fsc_free_node_t* current = m->head; // Start from the head of the free list
    fsc_free_node_t* prev = NULL; // Previous node pointer

    size_t totalSizeNeeded = requestedSizeInBytes + sizeof(fsc_alloc_header_t); // Calculate total size needed including header

    // Iterate over the free list to find a suitable block
    while (current != NULL) {
        // Check if current block is big enough
        if (current->size >= totalSizeNeeded) {
            // Calculate remaining size after allocation
            size_t remainingSize = current->size - totalSizeNeeded;

            // Set up the header for the allocated block
            fsc_alloc_header_t* header = (fsc_alloc_header_t*)((char*)current + sizeof(fsc_free_node_t));
            header->size = requestedSizeInBytes;
            header->magic = MAGIC_NUMBER;

            // Adjust the free list
            if (remainingSize >= sizeof(fsc_free_node_t)) {
                // Create a new free block with the remaining space
                fsc_free_node_t* newFreeNode = (fsc_free_node_t*)((char*)current + totalSizeNeeded);
                newFreeNode->size = remainingSize - sizeof(fsc_free_node_t);
                newFreeNode->next = current->next;
                current->size = remainingSize;

                if (prev) {
                    prev->next = newFreeNode;
                } else {
                    m->head = newFreeNode;
                }
            } else {
                // Not enough space to create a new free block, allocate the whole block
                if (prev) {
                    prev->next = current->next;
                } else {
                    m->head = current->next;
                }
            }
            // Return a pointer to the allocated memory space, just after the header.
            // Casting to (void*) makes it a general-purpose pointer, usable for any data type.
            return (void*)(header + 1); // Return a pointer to the memory after the header
        }

        prev = current; // Move to the next node
        current = current->next;
    }

    return NULL; // No suitable block found
}

// Function to free allocated memory
void fscFree(memoryStructure* m, void* returnedMemory) {
     if (m->magicNumber != MAGIC_NUMBER) {
         printf("Validation failed\n");
         return;
    }

    if (!returnedMemory) {
        return; // Standard free() behavior on NULL
    }

    // Get the header and the node of the block to be freed
    fsc_alloc_header_t* header = (fsc_alloc_header_t*)((char*)returnedMemory - sizeof(fsc_alloc_header_t));
    if (m->magicNumber != MAGIC_NUMBER) {
         printf("Validation failed\n");
         return;
    }

    fsc_free_node_t* nodeToFree = (fsc_free_node_t*)((char*)header - sizeof(fsc_free_node_t));
    nodeToFree->size = header->size + sizeof(fsc_alloc_header_t);

    // Insert the block back into the free list in the correct position
    fsc_free_node_t* current = m->head;
    fsc_free_node_t* prev = NULL;

    // Find the correct position to insert the freed block
    while (current != NULL && current < nodeToFree) {
        prev = current;
        current = current->next;
    }

    // Coalesce with next block if adjacent
    if (current && (char*)nodeToFree + nodeToFree->size == (char*)current) {
        nodeToFree->size += current->size + sizeof(fsc_free_node_t);
        nodeToFree->next = current->next;
    } else {
        nodeToFree->next = current;
    }

    // Insert the new or coalesced free block into the list
    if (prev) {
        prev->next = nodeToFree;
        // Coalesce with previous block if adjacent
        if ((char*)prev + prev->size == (char*)nodeToFree) {
            prev->size += nodeToFree->size + sizeof(fsc_free_node_t);
            prev->next = nodeToFree->next;
        }
    } else {
        m->head = nodeToFree;
    }
}

// Function to clean up the memory structure
void fscMemoryCleanup(memoryStructure* m) {
     if (m->magicNumber != MAGIC_NUMBER) {
         printf("Validation failed\n");
         return;
    }
    free(m->head); // Free the head of the free list
    m->head = NULL; // Set the head to NULL
    m->magicNumber = 0; // Reset the magic number
}

// Function to print the current state of the free list
void printFreeList(FILE * out, fsc_free_node_t* head) {
    fsc_free_node_t* current = head; // Start from the head of the list
    fprintf(out, "About to dump the free list:\n");
    // Iterate over the free list and print each node's details
    while (current != NULL) {
        fprintf(out,
                "Node address: %p\tNode size (stored): %zu\tNode size (actual): %zu\tNode next: %p\n",
                (void*)current,
                current->size,
                current->size + sizeof(fsc_free_node_t),
                (void*)current->next);
        current = current->next; // Move to the next node
    }
}
