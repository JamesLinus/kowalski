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
#include "kwl_synchronization.h"

#include "kwl_assert.h"
#include <stdio.h>

void kwlSemaphoreInit(kwlSemaphore* semaphore)
{
    KWL_ASSERT(0);//TODO
}

void kwlSemaphoreDestroy(kwlSemaphore* semaphore, const char* const name)
{
    KWL_ASSERT(0);//TODO    
}

void kwlSemaphoreWait(kwlSemaphore* semaphore)
{
    KWL_ASSERT(0);//TODO
}

void kwlSemaphorePost(kwlSemaphore* semaphore)
{
    KWL_ASSERT(0);//TODO
}

void kwlMutexLockInit(kwlMutexLock* lock)
{
    InitializeCriticalSection(&criticalSection);
}

/**
 * 
 */
kwlMutexLockAcquisitionResult kwlMutexLockTryAcquire(kwlMutexLock* lock)
{
    /*TODO: a proper try here*/
    EnterCriticalSection(lock);
    return KWL_LOCK_ACQUIRED;
}

void kwlMutexLockAcquire(kwlMutexLock* lock)
{
    EnterCriticalSection(lock);
}

void kwlMutexLockRelease(kwlMutexLock* lock)
{
    LeaveCriticalSection(&lock);
}
