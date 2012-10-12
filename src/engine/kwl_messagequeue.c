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

#include <string.h>
#include "kwl_messagequeue.h"

void kwlMessageQueue_init(kwlMessageQueue* queue)
{
    queue->messages = (kwlMessage*)KWL_MALLOC(KWL_MESSAGE_QUEUE_SIZE * sizeof(kwlMessage), "message queue");
    queue->maxQueueSize = KWL_MESSAGE_QUEUE_SIZE;
    queue->numMessages = 0;
}

void kwlMessageQueue_free(kwlMessageQueue* queue)
{
    KWL_FREE(queue->messages);
    memset(queue, 0, sizeof(kwlMessageQueue));
}

void kwlMessageQueue_flushTo(kwlMessageQueue* srcQueue, kwlMessageQueue* destQueue)
{
    int numFreeMessagesInDestQueue = destQueue->maxQueueSize - destQueue->numMessages;
    int numMessagesToCopy = srcQueue->numMessages;
    
    if (numMessagesToCopy > numFreeMessagesInDestQueue)
    {
        KWL_ASSERT(NULL && "not enough room in destination queue");
        numMessagesToCopy = numFreeMessagesInDestQueue;
    }
    
    kwlMemcpy(&destQueue->messages[destQueue->numMessages], srcQueue->messages, numMessagesToCopy * sizeof(kwlMessage));
    destQueue->numMessages += numMessagesToCopy;
    srcQueue->numMessages = 0;
}

int kwlMessageQueue_addMessage(kwlMessageQueue* queue, kwlMessageType type, void* data)
{
    kwlMessageQueue_addMessageWithParam(queue, type, data, 0.0f);
	return 1; /*TODO: return something sensible here.*/
}

int kwlMessageQueue_addMessageWithParam(kwlMessageQueue* queue, kwlMessageType type, void* data, float param)
{
    if (queue->numMessages >= queue->maxQueueSize)
    {
        KWL_ASSERT(NULL && "message queue full");
        return 0;
    }
    
    queue->messages[queue->numMessages].type = type;
    queue->messages[queue->numMessages].data = data;
    queue->messages[queue->numMessages].param = param;
    queue->numMessages++;
    return 1;
}
