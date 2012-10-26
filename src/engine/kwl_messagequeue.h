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

#ifndef KWL_MESSAGE_QUEUE_H
#define KWL_MESSAGE_QUEUE_H

#include "kwl_memory.h"
#include "kwl_assert.h"

/*! \file */ 

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** 
 * The size of the message queues used for sending messages
 * between the engine and mixer threads.
 */
#define KWL_MESSAGE_QUEUE_SIZE 500

/**
 * An enumeration of valid types for messages sent between the mixer and engine threads.
 */
typedef enum
{
    /** A request from the engine thread to start an event. */
    KWL_EVENT_START = 0,
    /** A request from the engine thread to stop an event. */
    KWL_EVENT_STOP,
    /** A request from the engine thread to pause an event. */
    KWL_EVENT_PAUSE,
    /** A request from the engine thread to resume a paused event. */
    KWL_EVENT_RESUME,
    /** A request from the engine thread to retrigger an event. */
    KWL_EVENT_RETRIGGER,
    /** A notification from the mixer thread that an event just finished playing. */
    KWL_EVENT_STOPPED,
    /** Stops all events referencing audio data in a given wave bank. 
     Sent from the engine to the mixer.*/
    KWL_STOP_ALL_EVENTS_REFERENCING_WAVE_BANK,
    /** Unload a given wave bank. Sent from the mixer to the engine once
        all events with audio data from the wave bank in question have been stopped.*/
    KWL_UNLOAD_WAVEBANK,
    /** Stops a given freeform event. Sent from the engine to the mixer prior to unloading a freeform event. */
    KWL_FREEFORM_EVENT_STOP,
    /** Sent from the mixer to the engine, instructing it to unload a freeform event.*/
    KWL_UNLOAD_FREEFORM_EVENT,
    /** Send from the engine to the mixer thread, requesting the mixer to stop all data driven event and mix buses.*/
    KWL_PREPARE_ENGINE_DATA_UNLOAD,
    /** Sent from the mixer to the engine thread indicating that it's safe to unload engine data.*/
    KWL_UNLOAD_ENGINE_DATA,
    /** Sent from the engine to notify the mixer that a new mix bus hierarchy has been loaded.*/
    KWL_SET_MASTER_BUS
     
} kwlMessageType;

/**
 * A message in a message queue.
 */
typedef struct
{
    /** The message type.*/
    kwlMessageType type;
    /** Any data associated with the message.*/
    void* data;
    /** An optional parameter assocaited with the message.*/
    float param;
} kwlMessage;

/**
 * A queue structure used for passing messages between the
 * mixer thread and the engine thread.
 */
typedef struct
{
    /** The message instances of the queue.*/
    kwlMessage* messages;
    /** The size of the message instance array.*/
    int maxQueueSize;
    /** */
    int numMessages;
} kwlMessageQueue;

/**
 * Initializes a message queue.
 * @param The queue to initialize.
 */
void kwlMessageQueue_init(kwlMessageQueue* queue);
    
void kwlMessageQueue_free(kwlMessageQueue* queue);

/**
 * 
 * @param srcQueue 
 * @param destQueue 
 */
void kwlMessageQueue_flushTo(kwlMessageQueue* srcQueue, kwlMessageQueue* destQueue);

/** 
 * Adds a message to a given queue.
 * @param queue The queue to add the message to.
 * @param type The type of the message to add.
 * @param data The data to associated with the added message.
 * @return A non zero integer if the message was successfully added or zero if the target queue is full.
 */
int kwlMessageQueue_addMessage(kwlMessageQueue* queue, kwlMessageType type, void* data);
    
int kwlMessageQueue_addMessageWithParam(kwlMessageQueue* queue, kwlMessageType type, void* data, float param);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */
        
#endif /*KWL_MESSAGE_QUEUE_H*/
