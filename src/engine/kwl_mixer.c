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

#include "kwl_asm.h"
#include "kwl_synchronization.h"
#include "kwl_eventinstance.h"
#include "kwl_memory.h"
#include "kwl_messagequeue.h"
#include "kwl_mixbus.h"
#include "kwl_mixer.h"
#include "kwl_sounddefinition.h"
#include "kwl_engine.h"

#include "kwl_assert.h"
#include <math.h>

kwlMixer* kwlMixer_new(void)
{
    kwlMixer* newMixer = (kwlMixer*)KWL_MALLOC(sizeof(kwlMixer), "kwlMixer_new");
    kwlMemset(newMixer, 0, sizeof(kwlMixer));
    
    kwlMessageQueue_init(&newMixer->toEngineQueue);
    kwlMessageQueue_init(&newMixer->toEngineQueueShared);
    kwlMessageQueue_init(&newMixer->fromEngineQueue);

    kwlMixBus_init(&newMixer->freeformEventsBus);
    newMixer->freeformEventsBus.id = "freeform event bus";
    newMixer->freeformEventsBus.totalPitch.valueMixer = 1.0f;
    newMixer->freeformEventsBus.totalGainLeft.valueMixer = 1.0f;
    newMixer->freeformEventsBus.totalGainRight.valueMixer = 1.0f;
    
    return newMixer;
}

void kwlMixer_allocateTempBuffers(kwlMixer* mixer)
{
    int tempBufferSize = sizeof(float) * KWL_TEMP_BUFFER_SIZE_IN_FRAMES * mixer->numOutChannels;
    mixer->tempMixBusBuffer = (float*)KWL_MALLOC(tempBufferSize, "mixer temp buffer");
    mixer->tempEventBuffer = (float*)KWL_MALLOC(tempBufferSize, "mixer temp buffer");
    mixer->outBuffer = (float*)KWL_MALLOC(tempBufferSize, "mixer temp out buffer");
    
    if (mixer->numInChannels > 0)
    {
        mixer->inBuffer = (float*)KWL_MALLOC(tempBufferSize, "mixer temp in buffer");
    }
}

void kwlMixer_free(kwlMixer* mixer)
{
    KWL_ASSERT(mixer != NULL);
    KWL_FREE(mixer->tempEventBuffer);
    KWL_FREE(mixer->tempMixBusBuffer);
    KWL_FREE(mixer->outBuffer);
    
    kwlMessageQueue_free(&mixer->toEngineQueue);
    kwlMessageQueue_free(&mixer->toEngineQueueShared);
    kwlMessageQueue_free(&mixer->fromEngineQueue);
    
    if (mixer->numInChannels > 0)
    {
        KWL_FREE(mixer->inBuffer);
    }
    
    KWL_FREE(mixer);
}

void kwlMixer_updateInput(kwlMixer* mixer)
{
    /* 
        Try to acquire the main lock and fail silently if it's already held. 
        It's better to miss an update here than to wait indefinitely for 
        the engine thread to release the lock, potentially leading to audio dropouts.
     */
    if (kwlMutexLockTryAcquire(mixer->mixerEngineMutexLock) == KWL_LOCK_ACQUIRED)
    {
        mixer->inputDSPUnit.valueMixer = mixer->inputDSPUnit.valueShared;
        
        kwlDSPUnit* dspUnit = (kwlDSPUnit*)mixer->inputDSPUnit.valueMixer;
        if (dspUnit != NULL && dspUnit->updateDSPMixerCallback != NULL)
        {
            dspUnit->updateDSPMixerCallback(dspUnit->data);
        }
        
        kwlMutexLockRelease(mixer->mixerEngineMutexLock);
    }
}

void kwlMixer_updateOutput(kwlMixer* const mixer)
{
    /* 
       Try to acquire the main lock and fail silently if it's already held. 
       It's better to miss an update here than to wait indefinitely for 
       the engine thread to release the lock, potentially leading to audio dropouts.
     */
    if (kwlMutexLockTryAcquire(mixer->mixerEngineMutexLock) == KWL_LOCK_ACQUIRED)
    {   
        /*copy incoming messages*/
        kwlMessageQueue_flushTo(&mixer->engine->toMixerQueueShared, &mixer->fromEngineQueue);
        /*copy buffered outgoing*/
        kwlMessageQueue_flushTo(&mixer->toEngineQueue, &mixer->toEngineQueueShared);
        
        /*update data driven mix buses and events*/
        int i;
        for (i = 0; i < mixer->numMixBuses; i++)
        {
            kwlMixBus* bus = &mixer->mixBuses[i];
            
            /*update mix bus values*/
            bus->totalGainLeft.valueMixer = bus->totalGainLeft.valueShared;
            bus->totalGainRight.valueMixer = bus->totalGainRight.valueShared;
            bus->totalPitch.valueMixer = bus->totalPitch.valueShared;
            bus->dspUnit.valueMixer = bus->dspUnit.valueShared;
        
            /*update parameters of playing events*/
            kwlEventInstance* eventList = bus->eventList;
            while (eventList != NULL)
            {
                eventList->gainLeft.valueMixer = eventList->gainLeft.valueShared;
                eventList->gainRight.valueMixer = eventList->gainRight.valueShared;
                eventList->pitch.valueMixer = eventList->pitch.valueShared;
                eventList->dspUnit.valueMixer = eventList->dspUnit.valueShared;
                if (eventList->dspUnit.valueMixer != NULL)
                {
                    kwlDSPUnit* dspUnit = (kwlDSPUnit*)eventList->dspUnit.valueMixer;
                    dspUnit->updateDSPMixerCallback(dspUnit->data);
                }
                
                eventList = eventList->nextEvent_mixer;
            }
        }
        
        /* update freeform events */
        kwlEventInstance* eventList = mixer->freeformEventsBus.eventList;
        while (eventList != NULL)
        {
            eventList->gainLeft.valueMixer = eventList->gainLeft.valueShared;
            eventList->gainRight.valueMixer = eventList->gainRight.valueShared;
            eventList->pitch.valueMixer = eventList->pitch.valueShared;
            if (eventList->dspUnit.valueMixer!= NULL)
            {
                kwlDSPUnit* dspUnit = (kwlDSPUnit*)eventList->dspUnit.valueMixer;
                dspUnit->updateDSPMixerCallback(dspUnit->data);
            }
            
            eventList = eventList->nextEvent_mixer;
        }
        
        /*update master dsp unit, if any.*/
        mixer->outputDSPUnit.valueMixer = mixer->outputDSPUnit.valueShared;
        kwlDSPUnit* dspUnit = (kwlDSPUnit*)mixer->outputDSPUnit.valueMixer;
        if (dspUnit != NULL && dspUnit->updateDSPMixerCallback != NULL)
        {
            dspUnit->updateDSPMixerCallback(dspUnit->data);
        }
        
        /*update levels and sync information*/
        mixer->numFramesMixed.valueShared = mixer->numFramesMixed.valueMixer;
        
        mixer->latestBufferAbsPeakLeft.valueShared = mixer->latestBufferAbsPeakLeft.valueMixer;
        mixer->latestBufferAbsPeakRight.valueShared = mixer->latestBufferAbsPeakRight.valueMixer;
        mixer->clipFlag.valueShared = mixer->clipFlag.valueMixer;
        mixer->isLevelMeteringEnabled.valueMixer = mixer->isLevelMeteringEnabled.valueShared;
        mixer->isPaused.valueMixer = mixer->isPaused.valueShared;
    
        kwlMutexLockRelease(mixer->mixerEngineMutexLock);
    }
}

void kwlMixer_processMessages(kwlMixer* const mixer)
{
    int numMessages = mixer->fromEngineQueue.numMessages;
    int i;
    for (i = 0; i < numMessages; i++)    
    {
        kwlMessage* message = &mixer->fromEngineQueue.messages[i];
        kwlMessageType type = message->type;
        void* messageData = message->data;
        //printf("mixer: processing incoming message %d/%d of type %d\n", i, numMessages, 
        //       type);
        
        if (type == KWL_EVENT_START ||
            type == KWL_EVENT_RETRIGGER)
        {   
            KWL_ASSERT(messageData != NULL && "message data is null");
            kwlEventInstance* event = (kwlEventInstance*)message->data;
            /*Find the bus to put the event in.*/
            kwlMixBus* targetBus = event->definition_mixer->mixBus;
            if (targetBus == NULL)
            {
                /*If the bus in the event definition is null, we're dealing with a freeform event.*/
                targetBus = &mixer->freeformEventsBus;
            }
            KWL_ASSERT(targetBus != NULL && "target bus is null");
            
            const int streamFromDisk = event->decoder != NULL;
            const int retrigger = (type == KWL_EVENT_RETRIGGER);

            kwlEventInstance_start(event);
            int shouldStop = 0; /*could be non-zero if the event is missing audio data*/
            if (streamFromDisk == 0)
            {
                KWL_ASSERT(event->definition_mixer->streamAudioData == NULL);
                shouldStop = kwlSoundDefinition_pickNextBufferForEvent(event->definition_mixer->sound, event, 1);
                /*KWL_ASSERT(result == 0 && "event should not signal stop on picking first buffer");*/
            }
            else 
            {
                KWL_ASSERT(event->definition_mixer->sound == NULL);
                //shouldStop = kwlDecoder_decodeNewBufferForEvent(event->decoder, event, 1);
            }
            
            /* check if this event should fade in */
            float fadeOutTime = message->param;
            if (fadeOutTime > 0.0f)
            {
                event->fadeGain = 0.0f;
                event->fadeGainIncrPerFrame = 1.0f / (fadeOutTime * mixer->sampleRate);
            }
            else
            {
                event->fadeGain = 1.0f;
                event->fadeGainIncrPerFrame = 0.0f;
            }
            
            if (shouldStop != 0)
            {
                event->playbackState = KWL_STOP_REQUESTED;
            }
            
            /*add the event to its bus.*/
            if (retrigger == 0)
            {
                kwlMixBus_addEvent(targetBus, event);
            }
        }
        else if (type == KWL_PREPARE_ENGINE_DATA_UNLOAD)
        {
            kwlMixer_stopAllDataDrivenEvents(mixer);
            //printf("mixer: got KWL_PREPARE_ENGINE_DATA_UNLOAD, sending KWL_UNLOAD_ENGINE_DATA back to engine\n");
            //int result = kwlMessageQueue_addMessage(&mixer->toEngineQueue, KWL_UNLOAD_ENGINE_DATA, NULL);
            //KWL_ASSERT(result == 1 && "mixer: outgoing message queue exhausted ");
            KWL_ASSERT(mixer->resetMixBusesRequested == 0);
            mixer->resetMixBusesRequested = 1;
        }
        else if (type == KWL_EVENT_STOP)
        {
            KWL_ASSERT(messageData != NULL);
            kwlEventInstance* event = (kwlEventInstance*)message->data;
            float fadeOutTime = message->param;
            if (event->isPaused)
            {
                /*Always stop paused events immediately.*/
                event->playbackState = KWL_STOP_REQUESTED;
            }
            else if (fadeOutTime > 0.0f)
            {
                /*Start the fade out. The event will get removed from the mixer when
                  the fade gain reaches 0.*/
                event->fadeGainIncrPerFrame = -1.0f / (fadeOutTime * mixer->sampleRate);
            }
            else if (event->definition_mixer->sound != NULL)
            {
                if (event->definition_mixer->sound->playbackMode == KWL_IN_RANDOM_OUT ||
                    event->definition_mixer->sound->playbackMode == KWL_IN_RANDOM_NO_REPEAT_OUT ||
                    event->definition_mixer->sound->playbackMode == KWL_IN_SEQUENTIAL_OUT)
                {
                    event->playbackState = KWL_PLAY_LAST_BUFFER_AND_STOP_REQUESTED;
                }
                else
                {
                    event->playbackState = KWL_STOP_REQUESTED;
                }
            }
            else
            {
                event->playbackState = KWL_STOP_REQUESTED;
            }
            
        }
        else if (type == KWL_EVENT_PAUSE)
        {
            kwlEventInstance* event = (kwlEventInstance*)message->data;
            event->isPaused = 1;
        }
        else if (type == KWL_EVENT_RESUME)
        {
            kwlEventInstance* event = (kwlEventInstance*)message->data;
            event->isPaused = 0;
        }
        else if (type == KWL_FREEFORM_EVENT_STOP)
        {
            kwlEventInstance* event = (kwlEventInstance*)message->data;
            //printf("stopping freeform event %s\n", event->definition_mixer->id);
            event->playbackState = KWL_STOP_AND_UNLOAD_REQUESTED;
        }
        else if (type == KWL_STOP_ALL_EVENTS_REFERENCING_WAVE_BANK)
        {
            kwlWaveBank* waveBank = (kwlWaveBank*)message->data;
            kwlMixer_stopAllEventsReferencingWaveBank(mixer, waveBank);
            /*printf("stopped all events referencing %s\n", waveBank->id);*/
            /* Send a message to the engine thread indicating that it's safe to unload the wave bank.
               IMPORTANT NOTE: This relies on kwlMixer_updateOutput being called BEFORE kwlMixer_processMessages*/
            int result = kwlMessageQueue_addMessage(&mixer->toEngineQueue, KWL_UNLOAD_WAVEBANK, waveBank);
            KWL_ASSERT(result == 1 && "mixer: outgoing message queue exhausted ");
        }
        else if (type == KWL_SET_MASTER_BUS)
        {
            kwlMixBus* newBusArray = (kwlMixBus*)message->data;
            int numBuses = (int)message->param;
            kwlMixer_setMixBusArray(mixer, newBusArray, numBuses);
        }
        else
        {
            KWL_ASSERT(NULL && "unknown message type");
        }
    }
    
    mixer->fromEngineQueue.numMessages = 0;
}

void kwlMixer_stopAllDataDrivenEvents(kwlMixer* mixer)
{
    /* loop over playing events to see if any should be stopped.*/
    int numMixBuses = mixer->numMixBuses;
    int busIndex;
    for (busIndex = 0; busIndex < numMixBuses; busIndex++)
    {
        kwlMixBus* const mixBusi = &mixer->mixBuses[busIndex];
        kwlEventInstance* event = mixBusi->eventList;
        while (event != NULL)
        {
            if (event->definition_mixer->numReferencedWaveBanks != 0 &&
                event->definition_mixer->referencedWaveBanks != NULL)
            {
                event->playbackState = KWL_STOP_REQUESTED;
            }
            
            event = event->nextEvent_mixer;
        }
    }
}

void kwlMixer_stopAllEventsReferencingWaveBank(kwlMixer* mixer, kwlWaveBank* waveBank)
{
    /* loop over playing events to see if any should be stopped.*/
    int numMixBuses = mixer->numMixBuses;
    int busIndex;
    for (busIndex = 0; busIndex < numMixBuses; busIndex++)
    {
        kwlMixBus* const mixBusi = &mixer->mixBuses[busIndex];
        kwlEventInstance* event = mixBusi->eventList;
        while (event != NULL)
        {
            int numReferencedWaveBanks = event->definition_mixer->numReferencedWaveBanks;
            int i;
            for (i = 0; i < numReferencedWaveBanks; i++)
            {
                if (event->definition_mixer->referencedWaveBanks[i] == waveBank)
                {
                    event->playbackState = KWL_STOP_REQUESTED;
                    break;
                }
            }            
            event = event->nextEvent_mixer;
        }
    }
}

void kwlMixer_stopAllEvents(kwlMixer* mixer)
{
    /* loop over playing events to see if any should be stopped.*/
    int numMixBuses = mixer->numMixBuses;
    int busIndex;
    for (busIndex = 0; busIndex < numMixBuses; busIndex++)
    {
        kwlMixBus* const mixBusi = &mixer->mixBuses[busIndex];
        kwlEventInstance* event = mixBusi->eventList;
        while (event != NULL)
        {
            event->playbackState = KWL_STOP_REQUESTED;
            event = event->nextEvent_mixer;
        }
    }
}

void kwlMixer_sendEventStoppedMessage(kwlMixer* mixer, kwlEventInstance* event)
{
    //printf("kwlMixer_sendEventStoppedMessage: %s\n", event->definition_mixer->id);
    
    /*Mark the event as not paused.*/
    event->isPaused = 0; 
    
    /*Send an event stopped message*/
    kwlMessageType messageType = 
        event->playbackState == KWL_STOP_AND_UNLOAD_REQUESTED ? 
                                KWL_UNLOAD_FREEFORM_EVENT : KWL_EVENT_STOPPED;
    /*printf("kwlMixer_sendEventStoppedMessage: %s, unload req %d\n", 
           event->definition_mixer->id, event->playbackState == KWL_STOP_AND_UNLOAD_REQUESTED);*/
    int result = kwlMessageQueue_addMessage(&mixer->toEngineQueue, messageType, event);
    KWL_ASSERT(result == 1 && "mixer: outgoing message queue exhausted ");
}

void kwlMixer_setMixBusArray(kwlMixer* mixer, kwlMixBus* buses, int numBuses)
{
    KWL_ASSERT(numBuses > 0);
    KWL_ASSERT(mixer->masterBus == NULL && "master bus already set");
    int i;
    for (i = 0; i < numBuses; i++)
    {
        if (buses[i].isMaster)
        {
            mixer->masterBus = &buses[i];
            break;
        }
    }
    KWL_ASSERT(mixer->masterBus != NULL && "no master bus found");
    mixer->numMixBuses = numBuses;
    mixer->mixBuses = buses;
}

void kwlMixer_resetMixBuses(kwlMixer* mixer)
{
    /*Reset any data driven mix buses in preparation for engine data unloading.*/
    mixer->numMixBuses = 0;
    mixer->mixBuses = NULL;
    mixer->masterBus = NULL;
}

void kwlMixer_render(kwlMixer* mixer, 
                             float* outBuffer, 
                             int numFrames)
{    
    /*process any new messages from the engine thread before rendering.*/
    kwlMixer_processMessages(mixer);
    
    /*Update the parameters of the mix buses and currently playing events.*/
    kwlMixer_updateOutput(mixer);
    
    /*Clear the output buffer.*/
    const int numOutChannels = mixer->numOutChannels;
    const int numSamples = numFrames * numOutChannels;
    kwlClearFloatBuffer(outBuffer, numSamples);
    
    /*Perform mixing if the mixer is not paused.*/
    if (mixer->isPaused.valueMixer == 0)
    {
        /* 
         There are two root mix buses: one for freeform events and one for
         data driven events.
         */
        for (int i = 0; i < 2; i++)
        {
            kwlMixBus* bus = i == 0 ? &mixer->freeformEventsBus : mixer->masterBus;
            if (bus != NULL)
            {
                kwlMixBus_render(bus,
                                 mixer,
                                 numOutChannels, 
                                 numFrames, 
                                 mixer->tempMixBusBuffer, 
                                 mixer->tempEventBuffer, 
                                 outBuffer, 
                                 bus->totalPitch.valueMixer, 
                                 bus->totalGainLeft.valueMixer, 
                                 bus->totalGainRight.valueMixer);
            }
        }
        
        /*Clamp out buffer to [-1, 1]*/
        kwlClampBuffer(outBuffer, numFrames * numOutChannels);
        
        /*record output peak levels if metering is enabled*/
        if (mixer->isLevelMeteringEnabled.valueMixer)
        {
            const int numOutSamples = numFrames * numOutChannels;
            mixer->latestBufferAbsPeakLeft.valueMixer = 
                kwlGetBufferAbsMax(outBuffer, numOutSamples, 0, numOutChannels);
            
            if (numOutChannels > 1)
            {
                mixer->latestBufferAbsPeakRight.valueMixer = 
                    kwlGetBufferAbsMax(outBuffer, numOutSamples, 1, numOutChannels);
            }
            mixer->clipFlag.valueMixer = 0;
            if (mixer->latestBufferAbsPeakLeft.valueMixer >= 1.0f ||
                mixer->latestBufferAbsPeakRight.valueMixer >= 1.0f)
            {
                mixer->clipFlag.valueMixer = 1;
            }
        }
        
        /*Update the number of mixed frames*/
        mixer->numFramesMixed.valueMixer += numFrames;
    }
    else
    {
        /* If the mixer is paused, make sure the level meters are zero.*/
        mixer->latestBufferAbsPeakLeft.valueMixer = 0.0f;
        mixer->latestBufferAbsPeakRight.valueMixer = 0.0f;
        mixer->clipFlag.valueMixer = 0;
    }
    
    /*Reset the mix buses if requested in preparation for engine data unloading.*/
    if (mixer->resetMixBusesRequested != 0)
    {
        kwlMixer_resetMixBuses(mixer);
        mixer->resetMixBusesRequested = 0;
        int result = kwlMessageQueue_addMessage(&mixer->toEngineQueue, KWL_UNLOAD_ENGINE_DATA, NULL);
        KWL_ASSERT(result == 1 && "mixer: outgoing message queue exhausted ");
    }
    
    /*pass the filled buffer through the master dsp unit, if any*/
    kwlDSPUnit* dspUnit = (kwlDSPUnit*)mixer->outputDSPUnit.valueMixer;
    if (dspUnit != NULL)
    {
        (*dspUnit->dspCallback)(outBuffer,
                                mixer->numOutChannels,
                                numFrames, 
                                dspUnit->data);
    }
}

void kwlMixer_processInputBuffer(kwlMixer* mixer, 
                                         const float* inBuffer,
                                         int numFrames)
{
    kwlMixer_updateInput(mixer);
    
    kwlDSPUnit* dspUnit = (kwlDSPUnit*)mixer->inputDSPUnit.valueMixer;
    
    if (inBuffer != NULL && 
        dspUnit != NULL &&
        mixer->numInChannels > 0)
    {
        (*dspUnit->dspCallback)(inBuffer,
                                mixer->numInChannels,
                                numFrames, 
                                dspUnit->data);
    }
}
