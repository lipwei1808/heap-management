/* posix.c: POSIX API Implementation */

#include "malloc/counters.h"
#include "malloc/freelist.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

/**
 * Allocate specified amount memory.
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to the requested amount of memory.
 **/
void *malloc(size_t size) {
    // Initialize counters
    init_counters();

    // Handle empty size
    if (!size) {
        return NULL;
    }

    // TODO: Search free list for any available block with matching size
    Block* block = free_list_search(size);
    if (block == NULL) {
        block = block_allocate(size);
    } else {
        block = block_split(block, size);
        block = block_detach(block);
    }

    // Could not find free block or allocate a block, so just return NULL
    if (!block) {
        return NULL;
    }

    // Check if allocated block makes sense
    assert(block->capacity >= block->size);
    assert(block->size     == size);
    assert(block->next     == block);
    assert(block->prev     == block);

    // Update counters
    Counters[MALLOCS]++;
    Counters[REQUESTED] += size;

    // Return data address associated with block
    return block->data;
}

/**
 * Release previously allocated memory.
 * @param   ptr     Pointer to previously allocated memory.
 **/
void free(void *ptr) {
    if (!ptr) {
        return;
    }

    // Update counters
    Counters[FREES]++;

    // TODO: Try to release block, otherwise insert it into the free list
    bool success = block_release(BLOCK_FROM_POINTER(ptr));
    if (success) return;

    free_list_insert(BLOCK_FROM_POINTER(ptr));
}

/**
 * Allocate memory with specified number of elements and with each element set
 * to 0.
 * @param   nmemb   Number of elements.
 * @param   size    Size of each element.
 * @return  Pointer to requested amount of memory.
 **/
void *calloc(size_t nmemb, size_t size) {
    // TODO: Implement calloc
    size_t s = size * nmemb;
    void* ptr = malloc(s);
    memset(ptr, 0, s);
    Counters[CALLOCS]++;
    return NULL;
}

/**
 * Reallocate memory with specified size.
 * @param   ptr     Pointer to previously allocated memory.
 * @param   size    Amount of bytes to allocate.
 * @return  Pointer to requested amount of memory.
 **/
void *realloc(void *ptr, size_t size) {
    // TODO: Implement realloc
    if (ptr == NULL) {
        return malloc(size);
    }

    Block* block = BLOCK_FROM_POINTER(ptr);
    if (block->capacity >= size) {
        return ptr;
    }

    void* new_block = malloc(size);
    if (new_block == NULL) {
        return NULL;
    }
    memcpy(new_block, ptr, block->size);
    free(block);
    // Counters[REALLOCS]++;
    return new_block;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
