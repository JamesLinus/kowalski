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
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

int debugSemaphoreCount = 0;
int debugThreadCount = 0;

kwlSemaphore* kwlSemaphoreOpen(const char* const name)
{
    errno = 0;
    kwlSemaphore* ret = NULL;
    /*
     * Passing O_CREAT | O_EXCL generates an error
     * if a semaphore with the given name alread exits.
     */
    ret = sem_open(name, O_CREAT | O_EXCL, 0777, 0);
    
    if (ret == SEM_FAILED && errno == EEXIST)
    {
        sem_unlink(name);
        ret = sem_open(name, O_CREAT | O_EXCL, 0777, 0);
    }
    
    if (ret == SEM_FAILED)
    {
        switch (errno) {
            case EACCES:
                KWL_ASSERT(0);
                break;
            case EEXIST:
                KWL_ASSERT(0);
                break;
            case EINVAL:
                KWL_ASSERT(0);
                break;
            case EMFILE:
                KWL_ASSERT(0);
                break;
            case ENAMETOOLONG:
                KWL_ASSERT(0);
                break;
            case ENFILE:
                KWL_ASSERT(0);
                break;
            case ENOENT:
                KWL_ASSERT(0);
                break;
            case ENOMEM:
                KWL_ASSERT(0);
                break;
            default:
                break;
        }
    }
    
    KWL_ASSERT(ret != SEM_FAILED);
    debugSemaphoreCount++;
    return ret;
}

void kwlSemaphoreDestroy(kwlSemaphore* semaphore, const char* const name)
{
    int rc = sem_unlink(name);
    
    KWL_ASSERT(rc == 0);
    
    rc = sem_close(semaphore);

    if (rc < 0)
    {
        switch (errno) {
            case EBUSY:
                KWL_ASSERT(0 && "EBUSY");
                break;
            default:
                break;
        }
    }
    
    KWL_ASSERT(rc == 0);
    debugSemaphoreCount--;

}

void kwlSemaphoreWait(kwlSemaphore* semaphore)
{
    int rc = sem_wait(semaphore);
    //printf("errno %d\n", errno);
    switch (errno) {
        case EBADF:
            KWL_ASSERT(0);
            break;
        case EAGAIN:
            KWL_ASSERT(0);
            break;
        case EINVAL:
            KWL_ASSERT(0);
            break;
        case ENOSYS:
            KWL_ASSERT(0);
            break;
        case EDEADLK:
            KWL_ASSERT(0);
            break;
        case EINTR:
            KWL_ASSERT(0);
            break;
        default:
            break;
    }        
    KWL_ASSERT(rc == 0);
}

void kwlSemaphorePost(kwlSemaphore* semaphore)
{
    int rc = sem_post(semaphore);
    
    if (rc != 0)
    {
        switch (errno) {
            case EINVAL:
                KWL_ASSERT(0);
                break;
            case ENOSYS:
                KWL_ASSERT(0);
                break;
            case EBADF:
                KWL_ASSERT(0 && "bad file descriptor/invalid semaphore");
            default:
                break;
        }            
    }
    KWL_ASSERT(rc == 0);
}

void kwlMutexLockInit(kwlMutexLock* lock)
{
    int rc = pthread_mutex_init(lock, NULL);
    KWL_ASSERT(rc == 0);
}

/**
 * 
 */
kwlMutexLockAcquisitionResult kwlMutexLockTryAcquire(kwlMutexLock* lock)
{
    int rc = pthread_mutex_trylock(lock);
    KWL_ASSERT(rc == 0 || rc == EBUSY);
    return rc == EBUSY ? KWL_LOCK_BUSY : KWL_LOCK_ACQUIRED;
}

void kwlMutexLockAcquire(kwlMutexLock* lock)
{
    int rc = pthread_mutex_trylock(lock);
    KWL_ASSERT(rc == 0 || rc == EBUSY);
}

void kwlMutexLockRelease(kwlMutexLock* lock)
{
    int rc = pthread_mutex_unlock(lock);
    KWL_ASSERT(rc == 0);
}

void kwlThreadCreate(kwlThread* thread, kwlThreadEntryPoint entryPoint, void* data)
{
    int rc = pthread_create(thread, NULL, entryPoint, data);
    KWL_ASSERT(rc == 0);
    debugThreadCount++;
}

void kwlThreadJoin(kwlThread* thread)
{
    void* valuePtr;
    int rc = pthread_join(*thread, &valuePtr);
    switch (rc) {
        case EINVAL:
            //The implementation has detected that the value specified by thread does 
            //not refer to a joinable thread.
            KWL_ASSERT(0);
            break;
        case ESRCH:
            //No thread could be found corresponding to that specified by the given thread ID.
            KWL_ASSERT(0);
            break;
        case EDEADLK:
            //A deadlock was detected or the value of thread specifies the calling thread.    
            KWL_ASSERT(0);
            break;
        default:
            break;
    }
    
    KWL_ASSERT(rc == 0);
    
    debugThreadCount--;
}
