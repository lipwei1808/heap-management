/* block.c: Block Structure */

#include "malloc/block.h"
#include "malloc/counters.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <limits.h>

/**
 * Allocate a new block on the heap using sbrk:
 *
 *  1. Determined aligned amount of memory to allocate.
 *  2. Allocate memory on the heap.
 *  3. Set allocage block properties.
 *
 * @param   size    Number of bytes to allocate.
 * @return  Pointer to data portion of newly allocate block.
 **/
Block *	block_allocate(size_t size) {
    if (size >= LONG_MAX) {
        return NULL;
    }
    // Allocate block
    intptr_t allocated = sizeof(Block) + ALIGN(size);
    Block *  block     = sbrk(allocated);
    if (block == SBRK_FAILURE) {
    	return NULL;
    }

    // Record block information
    block->capacity = ALIGN(size);
    block->size     = size;
    block->prev     = block;
    block->next     = block;

    // Update counters
    Counters[HEAP_SIZE] += allocated;
    Counters[BLOCKS]++;
    Counters[GROWS]++;
    return block;
}

/**
 * Attempt to release memory used by block to heap:
 *
 *  1. If the block is at the end of the heap.
 *  2. The block capacity meets the trim threshold.
 *
 * @param   block   Pointer to block to release.
 * @return  Whether or not the release completed successfully.
 **/
bool	block_release(Block *block) {
    // TODO: Implement block release
    // If block is within the heap
    size_t allocated = block->capacity + sizeof(Block);
    if (block->next != block || allocated < TRIM_THRESHOLD) { 
        return false;
    }

    // If block is end of heap
    if (sbrk(-1 * (sizeof(Block) + block->capacity)) == SBRK_FAILURE) {
        return false;
    }
    Counters[BLOCKS]--;
    Counters[SHRINKS]++;
    Counters[HEAP_SIZE] -= allocated;
    return true;
}

/**
 * Detach specified block from its neighbors.
 *
 * @param   block   Pointer to block to detach.
 * @return  Pointer to detached block.
 **/
Block * block_detach(Block *block) {
    // TODO: Detach block from neighbors by updating previous and next block
    if (block != NULL) {
        Block* prev = block->prev;
        Block* next = block->next;
        prev->next = next;
        next->prev = prev;
        block->next = block;
        block->prev = block;
    }
    return block;
}

/**
 * Attempt to merge source block into destination.
 *
 *  1. Compute end of destination and start of source.
 *
 *  2. If they both match, then merge source into destionation by giving the
 *  destination all of the memory allocated to source.
 *
 * @param   dst     Destination block we are merging into.
 * @param   src     Source block we are merging from.
 * @return  Whether or not the merge completed successfully.
 **/
bool	block_merge(Block *dst, Block *src) {
    // TODO: Implement block merge
    char* endOfDst = (char*)dst + sizeof(Block) + dst->capacity;
    char* startOfSrc = (char*)src;
    if (endOfDst != startOfSrc) {
        return false;
    }

    dst->capacity += src->capacity + sizeof(Block);
    Counters[MERGES]++;
    Counters[BLOCKS]--;
    return true;
}

/**
 * Attempt to split block with the specified size:
 *
 *  1. Check if block capacity is sufficient for requested aligned size and
 *  header Block.
 *
 *  2. Split specified block into two blocks.
 *
 * @param   block   Pointer to block to split into two separate blocks.
 * @param   size    Desired size of the first block after split.
 * @return  Pointer to original block (regardless if it was split or not).
 **/
Block * block_split(Block *block, size_t size) {
    // TODO: Implement block split
    if (block->capacity < ALIGN(size) + sizeof(Block)) {
        return block;
    }

    // Get ptr to nextBlock after split
    Block* nextBlock = (Block*)((uintptr_t)block + sizeof(Block) + ALIGN(size));
    
    // Update pointer links
    nextBlock->prev = block;
    block->next = nextBlock;

    // Update sizes of blocks
    nextBlock->capacity = block->capacity - ALIGN(size) - sizeof(Block);
    nextBlock->size = block->size - ALIGN(size) - sizeof(Block);

    block->capacity = ALIGN(size);
    block->size = size;
    Counters[SPLITS]++;
    Counters[BLOCKS]++;
    return block;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
