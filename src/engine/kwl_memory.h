/*
Copyright (c) 2010-2012 Per Gantelius

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#ifndef KWL__MEMORY_H
#define KWL__MEMORY_H

/*! \file */ 

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** */
void* kwlMemcpy(void* to, const void* from, size_t size);
/** */
void* kwlMemset(void* location, int value, size_t size);
void* kwlMalloc(size_t size);
void kwlFree(void* pointer);


#ifndef KWL_DEBUG_MEMORY

/**
 * This macro should be used for all memory allocations in the Kowalski Engine.
 * If the symbol KWL_DEBUG_MEMORY is not defined, KWL_MALLOC reduces to a call
 * to malloc. This macro exists both to allow for allocation tracking (if 
 * KWL_DEBUG_MEMORY is defined) and to make it easy to use custom allocators if need be.
 * The tag parameter is a const* char const string identifying the allocation.
 */
#define KWL_MALLOC(size, tag) kwlMalloc(size)
/**
 * This macro should be used for all memory deletions in the Kowalski Engine.
 * If the symbol KWL_DEBUG_MEMORY is not defined, KWL_FREE reduces to a call
 * to free.
 */
#define KWL_FREE(ptr) kwlFree(ptr)


#else

#define KWL_MALLOC(size, tag) kwlDebugMalloc(size, tag)
#define KWL_FREE(ptr) kwlDebugFree(ptr)
/** The size of the debug allocation tracking table.*/
#define KWL_DEBUG_ALLOCATION_TABLE_SIZE 1000
/** The max length of an allocation tag.*/
#define KWL_DEBUG_ALLOCATION_TAG_SIZE 50
    
/** A table of memory locations associated with individual calls to KWL_MALLOC.*/
void* kwlDebugAllocationAddresses[KWL_DEBUG_ALLOCATION_TABLE_SIZE];
    
/** A table of block sizes associated with individual calls to KWL_MALLOC.*/
int kwlDebugAllocationSizes[KWL_DEBUG_ALLOCATION_TABLE_SIZE];
    
/** A table of tags associated with individual calls to KWL_MALLOC.*/
char kwlDebugAllocationTags[KWL_DEBUG_ALLOCATION_TABLE_SIZE][KWL_DEBUG_ALLOCATION_TAG_SIZE];
    
/** Prints all recorded allocations that have not been deleted.*/    
void kwlDebugPrintAllocationReport();
    
/** Returns the number of currently allocated bytes.*/    
int kwlDebugGetLiveBytes();
    
/** Returns the total number of bytes allocated in this run, including freed blocks.*/    
int kwlDebugGetTotalBytes();
    
/** Allocates a block of memory and records the allocation. */
void* kwlDebugMalloc(size_t size, const char* const tag);
    
/** Deletes a block of memory and records the deletion. Also checks for double deletes. */
void kwlDebugFree(void* pointer);

#endif /*KWL_DEBUG_MEMORY*/

#ifdef __cplusplus
}
#endif /* __cplusplus */    

#endif /*KWL__MEMORY_H*/
