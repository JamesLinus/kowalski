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

#include "kwl_memory.h"

#include "kwl_assert.h"
#include <stdlib.h>
#include <string.h>

void* kwlMemcpy(void* to, const void* from, size_t size)
{
    return memcpy(to, from, size);
}

void* kwlMemset(void* location, int value, size_t size)
{
    return memset(location, value, size);
}

void* kwlMallocAndZero(size_t size)
{
    void* ptr = kwlMalloc(size);
    kwlMemset(ptr, 0, size);
    return ptr;
}

void* kwlRealloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

void* kwlMalloc(size_t size)
{
    return malloc(size);
}

void kwlFree(void* pointer)
{
    free(pointer);
}

#ifdef KWL_DEBUG_MEMORY
int liveBytes = 0;
int totalBytes = 0;

void* kwlDebugMallocAndZero(size_t size, const char* const tag)
{
    void* ptr = kwlDebugMalloc(size, tag);
    kwlMemset(ptr, 0, size);
    return ptr;
}

void* kwlDebugRealloc(void* ptr, size_t size, const char* const tag)
{
    if (ptr == NULL)
    {
        return kwlDebugMalloc(size, tag);
    }
    
    /*find a free slot in the allocation table*/
    int allocationSlotIndex = -1;
    for (int i = 0; i < KWL_DEBUG_ALLOCATION_TABLE_SIZE; i++)
    {
        if (kwlDebugAllocationAddresses[i] == ptr)
        {
            allocationSlotIndex = i;
            break;
        }
    }
    
    KWL_ASSERT(allocationSlotIndex >= 0 && "reallocating untracked pointer");
    
    void* newPtr = realloc(ptr, size);
    
    size_t oldSize = kwlDebugAllocationSizes[allocationSlotIndex];
    size_t delta = size - oldSize;
    
    kwlDebugAllocationSizes[allocationSlotIndex] += delta;
    kwlDebugAllocationAddresses[allocationSlotIndex] = newPtr;
    for (int i = 0; i < KWL_DEBUG_ALLOCATION_TAG_SIZE - 1; i++)
    {
        kwlDebugAllocationTags[allocationSlotIndex][i] = tag[i];
        if (tag[i] == '\0')
        {
            break;
        }
    }
    
    liveBytes += delta;
    totalBytes += delta;
    
    return newPtr;
}

void* kwlDebugMalloc(size_t size, const char* const tag)
{
    if (size == 0)
    {
        return NULL;
    }
    
    /*find a free slot in the allocation table*/
    int allocationSlotIndex = -1;
    for (int i = 0; i < KWL_DEBUG_ALLOCATION_TABLE_SIZE; i++)
    {
        if (kwlDebugAllocationAddresses[i] == NULL)
        {
            allocationSlotIndex = i;
            break;
        }
    }
    KWL_ASSERT(allocationSlotIndex >= 0 && "no free allocation table slots");
    /*printf("size = %d\n", size);*/
    /*allocate the block*/
    void* ptr = malloc(size);
    
    /*record the allocation*/
    kwlDebugAllocationAddresses[allocationSlotIndex] = ptr;
    for (int i = 0; i < KWL_DEBUG_ALLOCATION_TAG_SIZE - 1; i++)
    {
        kwlDebugAllocationTags[allocationSlotIndex][i] = tag[i];
        if (tag[i] == '\0')
        {
            break;
        }
    }
    
    kwlDebugAllocationTags[allocationSlotIndex][KWL_DEBUG_ALLOCATION_TAG_SIZE - 1] = '\0';
    kwlDebugAllocationSizes[allocationSlotIndex] = size;
    liveBytes += size;
    totalBytes += size;
    /*printf("malloc: live bytes = %d\n", liveBytes);*/
    /*return a pointer to the allocated block*/
    return ptr;
}

void kwlDebugFree(void* pointer)
{
    if (pointer == NULL)
    {
        return;
    }
    
    /*record the deletion*/
    int allocationSlotIndex = -1;
    for (int i = 0; i < KWL_DEBUG_ALLOCATION_TABLE_SIZE; i++)
    {
        if (kwlDebugAllocationAddresses[i] == pointer)
        {
            allocationSlotIndex = i;
            break;
        }
    }
    /*no matching slot found. this means that this is a double free
      or that the given address points to a block of memory that was
      not allocated using KWL_ALLOC.*/
    KWL_ASSERT(allocationSlotIndex >= 0 && "double free?");
    
    /*clear the allocation table entry*/
    kwlDebugAllocationAddresses[allocationSlotIndex] = NULL;
    for (int i = 0; i < KWL_DEBUG_ALLOCATION_TAG_SIZE; i++)
    {
        kwlDebugAllocationTags[allocationSlotIndex][i] = '\0';
    }
    
    liveBytes -= kwlDebugAllocationSizes[allocationSlotIndex];
    /*printf("free: live bytes = %d\n", liveBytes);*/
    /*free the memory block*/
    free(pointer);
}

int kwlDebugGetLiveBytes(void)
{
    return liveBytes;
}

int kwlDebugGetTotalBytes(void)
{
    return totalBytes;
}


void kwlDebugPrintAllocationReport()
{
    printf("Live allocations, %d bytes total :\n", liveBytes);
    printf("-----------------------------------------\n");
    int i;
    for (i = 0; i < KWL_DEBUG_ALLOCATION_TABLE_SIZE; i++)
    {
        if (kwlDebugAllocationAddresses[i] != NULL)
        {
            printf("%s (%d bytes)\n", &kwlDebugAllocationTags[i][0], kwlDebugAllocationSizes[i]);
        }
    }
    printf("-----------------------------------------\n");
}
#else

#endif /*KWL_DEBUG_MEMORY*/

