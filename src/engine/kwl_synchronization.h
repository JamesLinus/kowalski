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

#ifndef KWL_LOCK_H
#define KWL_LOCK_H

/*! \file */ 

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* 
 * Typedefs allowing platform independent representation of
 * synchronization primitives. TODO: make locks etc opaque types 
 * and make one c-file per platform
 */

#ifdef _WIN32
    #include <windows.h>
    typedef CRITICAL_SECTION kwlMutexLock;
    //TODO kwlSemaphore
    //TODO kwlThread
#else
    #include <semaphore.h>
    #include <pthread.h>
    #include <errno.h>
    typedef sem_t kwlSemaphore;
    typedef pthread_mutex_t kwlMutexLock;
    typedef pthread_t kwlThread;
#endif //_WIN32

/**
 * Possible mutex acquisition outcomes.
 */
typedef enum kwlMutexLockAcquisitionResult
{
    /** The mutex lock was not acquired because it is currently held by another thread. */
    KWL_LOCK_BUSY = 0,
    /** The mutex lock was successfully acquired.*/
    KWL_LOCK_ACQUIRED
} kwlMutexLockAcquisitionResult;
    
/** 
 * A helper struct meant to make it easier to identify and handle 
 * long longs shared between the engine and the mixer thread
 */
typedef struct kwlSharedLongLong
{
    /** Only accessed from the mixer thread.*/
    long long valueMixer;
    /** 
     * Accessed from both the engine and the mixer thread. 
     * A lock must be acquired before accessing this variable.
     */
    long long valueShared;
    /** Only accessed from the engine thread.*/
    long long valueEngine;
} kwlSharedLongLong;

/** 
 * A helper struct meant to make it easier to identify and handle 
 * long longs shared between the engine and the mixer thread
 */
typedef struct kwlSharedVoidPointer
{
    /** Only accessed from the mixer thread.*/
    void* valueMixer;
    /** 
     * Accessed from both the engine and the mixer thread. 
     * A lock must be acquired before accessing this variable.
     */
    void* valueShared;
    /** Only accessed from the engine thread.*/
    void* valueEngine;
} kwlSharedVoidPointer;

    
/** 
 * A helper struct meant to make it easier to identify and handle 
 * chars shared between the engine and the mixer thread
 */
typedef struct kwlSharedChar
{
    /** Only accessed from the mixer thread.*/
    char valueMixer;
    /** 
     * Accessed from both the engine and the mixer thread. 
     * A lock must be acquired before accessing this variable.
     */
    char valueShared;
    /** Only accessed from the engine thread.*/
    char valueEngine;
} kwlSharedChar;
    
/** 
 * A helper struct meant to make it easier to identify and handle 
 * floats shared between the engine and the mixer thread
 */
typedef struct kwlSharedFloat
{
    /** Only accessed from the mixer thread.*/
    float valueMixer;
    /** 
     * Accessed from both the engine and the mixer thread. 
     * A lock must be acquired before accessing this variable.
     */
    float valueShared;
    /** Only accessed from the engine thread.*/
    float valueEngine;
} kwlSharedFloat;

/** 
 * A helper struct meant to make it easier to identify and handle 
 * ints shared between the engine and the mixer thread
 */
typedef struct kwlSharedInt
{
    /** Only accessed from the mixer thread.*/
    int valueMixer;
    /** 
     * Accessed from both the engine and the mixer thread. 
     * A lock must be acquired before accessing this variable.
     */
    int valueShared;
    /** Only accessed from the engine thread.*/
    int valueEngine;
}  kwlSharedInt;

/**
 * 
 */    
kwlSemaphore* kwlSemaphoreOpen(const char* const name);

/**
 * 
 */    
void kwlSemaphoreDestroy(kwlSemaphore* semaphore, const char* const name);    
    
/**
 * 
 */    
void kwlSemaphoreWait(kwlSemaphore* semaphore);

/**
 * 
 */    
void kwlSemaphorePost(kwlSemaphore* semaphore);
       
/**
 *
 */
void kwlMutexLockInit(kwlMutexLock* lock);

/**
 * 
 */
kwlMutexLockAcquisitionResult kwlMutexLockTryAcquire(kwlMutexLock* lock);
    
/**
 * 
 */
void kwlMutexLockAcquire(kwlMutexLock* lock);
    
/**
 *
 */
void kwlMutexLockRelease(kwlMutexLock* lock);
    
typedef void * (*kwlThreadEntryPoint)(void* data);
    
void kwlThreadCreate(kwlThread* thread, kwlThreadEntryPoint entryPoint, void* data);
    
void kwlThreadJoin(kwlThread* thread);

#ifdef __cplusplus
}
#endif /* __cplusplus */
        

#endif /*KWL_LOCK_H*/
