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
#include "kwl_audiodata.h"
#include "kwl_audiofileutil.h"
#include "kwl_synchronization.h"
#include "kwl_decoder.h"
#include "kwl_eventinstance.h"
#include "kwl_eventdefinition.h"
#include "kwl_memory.h"
#include "kwl_messagequeue.h"
#include "kwl_positionalaudiolistener.h"
#include "kwl_mixer.h"
#include "kwl_sound.h"
#include "kwl_engine.h"
#include "kwl_wavebank.h"

#include "kwl_assert.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

int kwlEngine_isFreeformEventHandle(kwlEngine* engine, kwlEventHandle handle)
{
    int mask = 1 << 31;
    int isFreeForm = handle & mask;
    return isFreeForm;
}

int kwlEngine_getFreeformIndexFromHandle(kwlEngine* engine, kwlEventHandle handle)
{
    return handle & 0xffff;
}

kwlEventInstance* kwlEngine_getEventFromHandle(kwlEngine* engine, kwlEventHandle handle)
{
    kwlEventInstance* event = NULL;
    
    if (kwlEngine_isFreeformEventHandle(engine, handle))
    {
        int freeformEventIndex = handle & 0xffff;
        if (freeformEventIndex >= 0 &&
            freeformEventIndex < engine->freeformEventArraySize)
        {
            event = engine->freeformEvents[freeformEventIndex];
        }
    }
    else
    {
        int eventDefinitionIndex = handle & 0xffff;
        int eventInstanceIndex = (handle & 0x7fff) >> 16;
        
        if (eventDefinitionIndex >= 0 && 
            eventDefinitionIndex < engine->engineData.numEventDefinitions)
        {
            if (eventInstanceIndex >= 0 && 
                eventInstanceIndex < engine->engineData.eventDefinitions[eventDefinitionIndex].instanceCount)
            {
                event = &engine->engineData.events[eventDefinitionIndex][eventInstanceIndex];
            }
        }
    }    
    /* A return value of NULL is valid and means that the event handle was invalid.*/
    return event;
}

/** */
kwlEventHandle computeEventHandle(int eventDefinitionOrFreeformIndex, int eventInstanceIndex, int isFreeForm)
{
    /*
     The event handle uniquely indentifies an event instance that could either 
     be loaded from data or created in code as a freeform event. A data event
     is uniquely defined by an event definition index and an event instance index
     and a freeform event by its index into the array of freeform events.
     
     An event handle is a 32 bit int encoded as follows:
     
     bit number |        32             |  31  |  ...  |  17  |  16  |  ...  |  1  |
                |-----------------------|-------------------------------------------
                |0 for data events      |these 15 bits encode |these 16 bits encode the 
                |1 for freeform events  |the event instance   |event definition index for data
                |                       |index and are zero   |events and the array index for 
                |                       |for freeform events  |freeform events.
     
     This encoding scheme means that the maximum number of event instances per definition is 32767
     and the maximum number of event definitions and freeform events is 65535.
     
     KWL_INVALID_HANDLE = 0xffffffff is used to represent invalid handles.
     */
    
    KWL_ASSERT(eventDefinitionOrFreeformIndex >= 0);
    KWL_ASSERT(eventInstanceIndex >= 0);
    KWL_ASSERT(eventDefinitionOrFreeformIndex < (1 << 16));
    KWL_ASSERT(eventInstanceIndex < (1 << 15));
    
    int bit32 = (isFreeForm != 0 ? 1 : 0) << 31;
    int bits31To17 = eventInstanceIndex << 16;
    int bits1To16 = eventDefinitionOrFreeformIndex;
    
    int handle = bit32 | bits31To17 | bits1To16;
    /*printf("computing event handle: dIdx=%d, iIdx=%d, handle=%d\n", eventDefinitionIndex, eventInstanceIndex, handle);*/
    return handle;
}

kwlWaveBankHandle kwlEngine_getHandleFromWaveBank(kwlEngine* engine, kwlWaveBank* waveBank)
{
    if (waveBank == NULL)
    {
        return KWL_INVALID_HANDLE;
    }
    
    for (int i = 0; i < engine->engineData.numWaveBanks; i++)
    {
        /*Pointer equality is sufficient*/
        if (&engine->engineData.waveBanks[i] == waveBank)
        {
            return i;
        }
    }
    
    return KWL_INVALID_HANDLE;
}

/** */
void kwlEngine_init(kwlEngine* engine)
{
    /*create message queues*/
    kwlMessageQueue_init(&engine->toMixerQueue);
    kwlMessageQueue_init(&engine->toMixerQueueShared);
    kwlMessageQueue_init(&engine->fromMixerQueue);
    
    kwlMessageQueue_init(&engine->wavebankLoadingQueue);
    kwlMessageQueue_init(&engine->wavebankLoadingQueueShared);
    
    /*create the software mixer*/
    engine->mixer = kwlMixer_new();
    engine->mixer->engine = engine;
    
    /*init positional audio listener and settings */
    kwlPositionalAudioListener_setDefaults(&engine->listener);
    kwlPositionalAudioSettings_setDefaults(&engine->positionalAudioSettings);
    
    engine->engineData.isLoaded = 0;
    engine->playingEventList = NULL;
    engine->engineData.numMixBuses = 0;
    engine->engineData.mixBuses = NULL;    
    engine->engineData.masterBus = NULL;
    
    engine->freeformEventArraySize = 0;
    engine->freeformEvents = NULL;
    
    int KWL_NUM_DECODERS = 10;
    engine->numDecoders = KWL_NUM_DECODERS;
    engine->decoders = (kwlDecoder*)KWL_MALLOC(sizeof(kwlDecoder) * KWL_NUM_DECODERS, "decoders");
    kwlMemset(engine->decoders, 0, sizeof(kwlDecoder) * KWL_NUM_DECODERS);
    
    //set up main mutex lock
    kwlMutexLockInit(&engine->mixerEngineMutexLock);
    engine->mixer->mixerEngineMutexLock = &engine->mixerEngineMutexLock;
    
    //set up wavebank loading mutex
    kwlMutexLockInit(&engine->wavebankLoadingMutexLock);
}

void kwlEngine_free(kwlEngine* engine)
{
    KWL_ASSERT(engine != NULL);
    
    kwlMessageQueue_free(&engine->toMixerQueue);
    kwlMessageQueue_free(&engine->toMixerQueueShared);
    kwlMessageQueue_free(&engine->fromMixerQueue);
    
    KWL_FREE(engine->decoders);
}

kwlError kwlEngine_loadWaveBank(kwlEngine* engine, 
                                     const char* const waveBankPath, 
                                     kwlWaveBankHandle* handle,
                                     int threaded)
{
    if (!engine->engineData.isLoaded)
    {
        return KWL_ENGINE_DATA_NOT_LOADED;
    }
    
    /*TODO: handle the case of more than one wave bank sharing a piece of audio data.*/

    /* Check that we have a valid wave bank binary file and that its entries match those
       in engine data.*/
    kwlWaveBank* matchingWaveBank = NULL;
    kwlError verifyResult = kwlWaveBank_verifyWaveBankBinary(engine, waveBankPath, &matchingWaveBank);
    
    if (verifyResult != KWL_NO_ERROR)
    {
        return verifyResult;
    }
    KWL_ASSERT(matchingWaveBank);

    /*If we made it this far, the wave bank binary data lines up with a wave
     bank structure of the engine so we're ready to load the audio data.*/
    kwlError result = kwlWaveBank_loadAudioData(matchingWaveBank, waveBankPath, threaded);
        
    /*Only care about the handle if this is a blocking call. For non-blocking calls,
      it gets passed to the loading finished callback.*/
    if (threaded == 0)
    {
        KWL_ASSERT(handle != NULL);
        KWL_ASSERT(matchingWaveBank != NULL);
        
        if (result == KWL_NO_ERROR)
        {
            *handle = kwlEngine_getHandleFromWaveBank(engine, matchingWaveBank);
        }
        else
        {
            *handle = KWL_INVALID_HANDLE;
        }
    }
    
    return result;
}

kwlError kwlEngine_waveBankIsLoaded(kwlEngine* engine, kwlWaveBankHandle handle, int* isLoaded)
{
    if (engine->engineData.isLoaded == 0)
    {
        return KWL_ENGINE_DATA_NOT_LOADED;
    }
    
    if (handle < 0 || handle >= engine->engineData.numWaveBanks || handle == KWL_INVALID_HANDLE)
    {
        return KWL_INVALID_WAVE_BANK_HANDLE;
    }
    
    *isLoaded = engine->engineData.waveBanks[handle].isLoaded;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_waveBankIsReferencedByPlayingEvent(kwlEngine* engine, kwlWaveBankHandle handle, int* isReferenced)
{
    if (engine->engineData.isLoaded == 0)
    {
        return KWL_ENGINE_DATA_NOT_LOADED;
    }
    
    if (handle < 0 || handle >= engine->engineData.numWaveBanks || handle == KWL_INVALID_HANDLE)
    {
        return KWL_INVALID_WAVE_BANK_HANDLE;
    }
    
    kwlWaveBank* waveBank = &engine->engineData.waveBanks[handle];
    *isReferenced = 0;
    
    /*early exit if the wave bank is not loaded*/
    if (waveBank->isLoaded == 0)
    {
        return KWL_NO_ERROR;
    }
    
    /*loop over all currently playing events and see if any of them 
     references the wavebank.*/
    kwlEventInstance* e = engine->playingEventList;
    while (e != NULL)
    {
        if (e->definition_engine->referencedWaveBanks == NULL ||
            e->definition_engine->mixBus == NULL)
        {
            /*dont check freeform event.*/
        }
        else
        {    
            const int numBanks = e->definition_engine->numReferencedWaveBanks;
            kwlWaveBank** banks = e->definition_engine->referencedWaveBanks;
            
            for (int i = 0; i < numBanks; i++)
            {
                if (banks[i] == waveBank)
                {
                    *isReferenced = 1;
                    return KWL_NO_ERROR;
                }
            }
        }
        
        e = e->nextEvent_engine;
    }

    return KWL_NO_ERROR;
}

kwlError kwlEngine_requestUnloadWaveBank(kwlEngine* engine, kwlWaveBankHandle handle, int blockUntilUnloaded)
{   
    if (engine->engineData.isLoaded == 0)
    {
        return KWL_ENGINE_DATA_NOT_LOADED;
    }
    
    if (handle < 0 || handle >= engine->engineData.numWaveBanks || handle == KWL_INVALID_HANDLE)
    {
        return KWL_INVALID_WAVE_BANK_HANDLE;
    }
    
    /*Send a message to the mixer to stop all playing events referencing audio data from the wave bank.
      Once the events are stopped, the mixer sends a message back to the engine indicating that it
      is safe to unload the wavebank.*/
    kwlWaveBank* waveBankToUnload = &engine->engineData.waveBanks[handle];
    
    if (waveBankToUnload->isLoaded == 0)
    {
        return KWL_NO_ERROR;
    }
    
    int result = kwlMessageQueue_addMessage(&engine->toMixerQueue, KWL_STOP_ALL_EVENTS_REFERENCING_WAVE_BANK, waveBankToUnload);
    if (result == 0)
    {
        return KWL_MESSAGE_QUEUE_FULL;
    }
    
    if (blockUntilUnloaded != 0)
    {
        //TODO: do this in a better way. this will cause a freeze if the audio thread is not running
        /*If a blocking unload was requested, wait for the wavebank to get unloaded before
         returning.*/
        while (waveBankToUnload->isLoaded != 0)
        {
            kwlUpdate(0);
        }
    }
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_eventGetHandle(kwlEngine* engine, const char* const eventID, kwlEventHandle* handle)
{
    *handle = KWL_INVALID_HANDLE;
    
    if (!engine->engineData.isLoaded)
    {
        return KWL_ENGINE_DATA_NOT_LOADED;
    }
    
    KWL_ASSERT(engine->engineData.events != NULL);
    KWL_ASSERT(engine->engineData.eventDefinitions != NULL);
    
    const int numEventDefinitions = engine->engineData.numEventDefinitions;
    int i;
    for (i = 0; i < numEventDefinitions; i++)
    {
        if (strcmp(eventID, engine->engineData.eventDefinitions[i].id) == 0)
        {
            const int numInstances = engine->engineData.eventDefinitions[i].instanceCount;
            int j;
            for (j = 0; j < numInstances; j++)
            {
                kwlEventInstance* const eventj = &engine->engineData.events[i][j];
                if (eventj->isAssociatedWithHandle == 0)
                {
                    *handle = computeEventHandle(i, j, 0);
                    eventj->isAssociatedWithHandle = 1;
                    return KWL_NO_ERROR;
                }
            }
            
            return KWL_NO_FREE_EVENT_INSTANCES;
        }
    }
    
    return KWL_UNKNOWN_EVENT_DEFINITION_ID;
}

kwlError kwlEngine_eventDefinitionGetHandle(kwlEngine* engine, 
                                                 const char* const eventDefinitionID, 
                                                 kwlEventDefinitionHandle* handle)
{
    *handle = KWL_INVALID_HANDLE;
    
    if (!engine->engineData.isLoaded)
    {
        return KWL_ENGINE_DATA_NOT_LOADED;
    }
    
    int i;
    for (i = 0; i < engine->engineData.numEventDefinitions; i++)
    {
        if (strcmp(eventDefinitionID, engine->engineData.eventDefinitions[i].id) == 0)
        {
            *handle = i;
            return KWL_NO_ERROR;
        }
    }
    
    return KWL_UNKNOWN_EVENT_DEFINITION_ID;
}

void kwlEngine_addFreeformEvent(kwlEngine* engine, kwlEventInstance* event, kwlEventHandle* handle)
{
    /*find a free slot in the freeform event array. reallocate the array if needed */
    int slotIdx = -1;
    int i;
    for (i = 0; i < engine->freeformEventArraySize; i++)
    {
        if (engine->freeformEvents[i] == NULL)
        {
            slotIdx = i;
            break;
        }
    }
    
    if (slotIdx < 0)
    {
        /*no free slots, reallocate the array 
         and pick the newly allocated slot*/
        engine->freeformEventArraySize++;
        engine->freeformEvents = (kwlEventInstance**)realloc(engine->freeformEvents, 
                                                     engine->freeformEventArraySize * sizeof(kwlEventInstance*));
        engine->freeformEvents[engine->freeformEventArraySize - 1] = NULL;
        
        slotIdx = engine->freeformEventArraySize - 1;
    }
    
    *handle = computeEventHandle(slotIdx, 0, 1);
    //printf("slot idx %d, handle %d, event %d\n", slotIdx, *handle, createdEvent);
    
    engine->freeformEvents[slotIdx] = event;
}

kwlError kwlEngine_eventCreateWithBuffer(kwlEngine* engine, kwlPCMBuffer* buffer, 
                                              kwlEventHandle* handle, kwlEventType type)
{
    *handle = KWL_INVALID_HANDLE;
    kwlEventInstance* createdEvent = NULL;
    kwlError result = kwlEventInstance_createFreeformEventFromBuffer(&createdEvent, buffer, type);
    
    if (result == KWL_NO_ERROR)
    {
        KWL_ASSERT(createdEvent != NULL);
        kwlEngine_addFreeformEvent(engine, createdEvent, handle);
    }
    
    return result;
}

kwlError kwlEngine_eventCreateWithFile(kwlEngine* engine, const char* const audioFilePath, 
                                            kwlEventHandle* handle, kwlEventType type, int streamFromDisk)
{
    kwlEventInstance* createdEvent = NULL;
    kwlError result = kwlEventInstance_createFreeformEventFromFile(&createdEvent, audioFilePath, type, streamFromDisk);
    
    if (result == KWL_NO_ERROR)
    {
        KWL_ASSERT(createdEvent != NULL);
        kwlEngine_addFreeformEvent(engine, createdEvent, handle);
    }
    
    return result;
}

kwlError kwlEngine_unloadFreeformEvent(kwlEngine* engine, kwlEventInstance* event)
{
    /*printf("kwlEngine_unloadFreeformEvent: %s\n", event->definition_engine->id);*/
    KWL_ASSERT(event != NULL);
    KWL_ASSERT(event->isPlaying == 0);
    
    int matchFound = 0;
    int eventIndex;
    for (eventIndex = 0; eventIndex < engine->freeformEventArraySize; eventIndex++)
    {
        if (engine->freeformEvents[eventIndex] == event)
        {
            matchFound = 1;
            break;
        }
    }
    
    KWL_ASSERT(matchFound != 0 && "trying to free a freeform event that is not in the engine's list");
    /*Clear the event slot so it can be reused. */
    engine->freeformEvents[eventIndex] = NULL;
    
    /*Release event data.*/
    kwlEventInstance_releaseFreeformEvent(event);
    
    return KWL_NO_ERROR;
}


kwlError kwlEngine_eventRelease(kwlEngine* engine, kwlEventHandle handle)
{
    kwlEventInstance* eventToRelease = kwlEngine_getEventFromHandle(engine, handle);
    if (eventToRelease == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    /*If this is a freeform event, dispose of any data allocated for it.*/
    if (kwlEngine_isFreeformEventHandle(engine, handle))
    {   
        if (eventToRelease->isPlaying == 0)
        {
            kwlEngine_unloadFreeformEvent(engine, eventToRelease);
        }
        else
        {
            /*Send a message to the mixer instructing it to stop the event we want to unload.
              Once the event is stopped, a KWL_UNLOAD_FREEFORM_EVENT message triggering the actual
              unloading will be send back to the engine.*/
            int result = kwlMessageQueue_addMessage(&engine->toMixerQueue, 
                                                    KWL_FREEFORM_EVENT_STOP, 
                                                    eventToRelease);
            if (result == 0)
            {
                KWL_ASSERT(0);
                return KWL_MESSAGE_QUEUE_FULL;
            }
        }
    }
    /*For data driven events, just mark the event as not associated with a handle.*/
    else
    {
        /*TODO: reset other stuff here?*/
        eventToRelease->userGain = 1.0f;
        eventToRelease->userPitch = 1.0f;
        eventToRelease->dspUnit.valueEngine = NULL;
        eventToRelease->isAssociatedWithHandle = 0;
    }
    
    /*Clear callbacks*/
    eventToRelease->stoppedCallback = NULL;
    eventToRelease->stoppedCallbackUserData = NULL;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_mixBusGetHandle(kwlEngine* engine, const char* const busId, kwlMixBusHandle* handle)
{
    *handle = KWL_INVALID_HANDLE;
    if (!engine->engineData.isLoaded)
    {
        return KWL_ENGINE_DATA_NOT_LOADED;
    }
    
    KWL_ASSERT(engine->engineData.mixBuses != NULL);
    
    const int numMixBuses = engine->engineData.numMixBuses;
    
    int i;
    for (i = 0; i < numMixBuses; i++)
    {
        if (strcmp(busId, engine->engineData.mixBuses[i].id) == 0)
        {
            /*the mix bus handle is just the index into the mix bus array*/
            *handle = i;
            return KWL_NO_ERROR;
        }
    }
    
    return KWL_UNKNOWN_MIX_BUS_ID;
}

kwlMixBus* kwlEngine_getMixBusFromHandle(kwlEngine* engine, kwlMixBusHandle handle)
{
    if (handle < 0 || handle >= engine->engineData.numMixBuses || handle == KWL_INVALID_HANDLE) 
    {
        return NULL;
    }
    
    return &engine->engineData.mixBuses[handle];
}

kwlError kwlEngine_mixBusSetGain(kwlEngine* engine, kwlMixBusHandle handle, float gain, int isLinearGain)
{
    /*TODO: thread safe access to numMixBuses??*/
    /*TODO: return no engine data error?*/
    kwlMixBus* const mixBus = kwlEngine_getMixBusFromHandle(engine, handle);
    if (mixBus == NULL)
    {
        return KWL_INVALID_MIX_BUS_HANDLE;
    }
    if (gain < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    /*TODO: support left and right gain separately?*/
    float linGain = isLinearGain == 1 ? gain : logGainToLinGain(gain);
    mixBus->userGainLeft = linGain;
    mixBus->userGainRight = linGain;
        
    return KWL_NO_ERROR;
}


kwlError kwlEngine_mixBusSetPitch(kwlEngine* engine, kwlMixBusHandle handle, float pitch)
{
    /*TODO: thread safe access to numMixBuses??*/
    /*TODO: return no engine data error?*/
    kwlMixBus* const mixBus = kwlEngine_getMixBusFromHandle(engine, handle);
    if (mixBus == NULL)
    {
        return KWL_INVALID_MIX_BUS_HANDLE;
    }
    if (pitch < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    mixBus->userPitch = pitch;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_mixPresetGetHandle(kwlEngine* engine, const char* const presetId, kwlMixBusHandle* handle)
{
    int i;
    for (i = 0; i < engine->engineData.numMixPresets; i++)
    {
        if (strcmp(presetId, engine->engineData.mixPresets[i].id) == 0)
        {
            *handle = i;
            return KWL_NO_ERROR;
        }
    }
    
    *handle = -1;
    return KWL_UNKNOWN_MIX_PRESET_ID;
}

kwlError kwlEngine_mixPresetSetActive(kwlEngine* engine, kwlMixPresetHandle handle, int doFade)
{
    if (handle < 0 || handle >= engine->engineData.numMixPresets || handle == KWL_INVALID_HANDLE)
    {
        return KWL_INVALID_MIX_PRESET_HANDLE;
    }
    
    int i;
    for (i = 0; i < engine->engineData.numMixPresets; i++)
    {
        float w = (i == handle ? 1.0f : 0.0f);
        engine->engineData.mixPresets[i].targetWeight = w;
        
        /*if we're not doing a fade, just set the weight straight away*/
        if (doFade == 0)
        {
            engine->engineData.mixPresets[i].weight = w;
        }
    }
    
    return KWL_NO_ERROR;
}


kwlError kwlEngine_setListenerPosition(kwlEngine* engine, float posX, float posY, float posZ)
{
    engine->listener.positionX = posX;
    engine->listener.positionY = posY;
    engine->listener.positionZ = posZ;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_setListenerVelocity(kwlEngine* engine, float velX, float velY, float velZ)
{
    engine->listener.velocityX = velX;
    engine->listener.velocityY = velY;
    engine->listener.velocityZ = velZ;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_setListenerOrientation(kwlEngine* engine, float dirX, float dirY, float dirZ,
                                               float upX, float upY, float upZ)
{
    if ((dirX == 0.0f && dirY == 0.0f && dirZ == 0.0f) ||
        (upX == 0.0f && upY == 0.0f && upZ == 0.0f))
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    float upLengthInv = kwlFastInverseSqrt(upX * upX + upY * upY + upZ * upZ);
    engine->listener.upX = upX * upLengthInv;
    engine->listener.upY = upY * upLengthInv;
    engine->listener.upZ = upZ * upLengthInv;
    
    float dirLengthInv = kwlFastInverseSqrt(dirX * dirX + dirY * dirY + dirZ * dirZ);
    engine->listener.directionX = dirX * dirLengthInv;
    engine->listener.directionY = dirY * dirLengthInv;
    engine->listener.directionZ = dirZ * dirLengthInv;
    
    engine->listener.rightX = engine->listener.directionY * engine->listener.upZ -
                              engine->listener.directionZ * engine->listener.upY;
    engine->listener.rightY = engine->listener.directionZ * engine->listener.upX -
                              engine->listener.directionX * engine->listener.upZ;
    engine->listener.rightZ = engine->listener.directionX * engine->listener.upY -
                              engine->listener.directionY * engine->listener.upX;
    
    return KWL_NO_ERROR;    
}

kwlError kwlEngine_setDistanceAttenuationModel(kwlEngine* engine,
                                                    kwlDistanceAttenuationModel type, 
                                                    int clamp,
                                                    float maxDistance,
                                                    float rolloffFactor,
                                                    float referenceDistance)
{
    if (rolloffFactor < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    if (referenceDistance <= 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    if (referenceDistance >= maxDistance && maxDistance > 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    engine->positionalAudioSettings.clamp = clamp;
    engine->positionalAudioSettings.referenceDistance = referenceDistance;
    engine->positionalAudioSettings.rolloffFactor = rolloffFactor;
    engine->positionalAudioSettings.referenceDistance = referenceDistance;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_setDopplerShiftParameters(kwlEngine* engine, float speedOfSound, float dopplerScale)
{
    if (speedOfSound <= 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    if (dopplerScale > 1.0f || dopplerScale < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    engine->positionalAudioSettings.speedOfSound = speedOfSound;
    engine->positionalAudioSettings.dopplerScale = dopplerScale;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_setConeAttenuationEnabled(kwlEngine* engine, int listenerCone, int eventCones)
{
    engine->positionalAudioSettings.isEventConeAttenuationEnabled = eventCones;
    engine->positionalAudioSettings.isListenerConeAttenuationEnabled = listenerCone;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_setListenerConeParameters(kwlEngine* engine, 
                                                  float innerAngle, 
                                                  float outerAngle, 
                                                  float outerGain)
{
    if (innerAngle < 0 || innerAngle > 360)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    if (outerAngle < 0 || outerAngle > 360)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    if (outerAngle < innerAngle)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    if (outerGain < 0)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    const float degToRad = 0.017453292f;
    float cosInner = cosf(degToRad * innerAngle / 2.0f);
    float cosOuter = cosf(degToRad * outerAngle / 2.0f);
    
    engine->listener.outerConeGain = outerGain;
    engine->listener.innerConeCosAngle = cosInner;
    engine->listener.outerConeCosAngle = cosOuter;
    
    return KWL_NO_ERROR;
}

float kwlEngine_getDistanceGain(kwlEngine* engine, float distanceInv)
{
    KWL_ASSERT(distanceInv >= 0.0f);
    kwlDistanceAttenuationModel model = engine->positionalAudioSettings.distanceModel;
    int clamp = engine->positionalAudioSettings.clamp;
    float distance = distanceInv = 1 / distanceInv; /*TODO: user distanceInv directly?*/
    float refDist = engine->positionalAudioSettings.referenceDistance;
    float rolloff = engine->positionalAudioSettings.rolloffFactor;
    float maxDist = engine->positionalAudioSettings.maxDistance;

    if (maxDist > 0.0f && distance > maxDist)
    {
        /*The event is too far away.*/
        return 0.0f;
    }
    
    switch (model)
    {
        case KWL_CONSTANT:
            return 1.0f;
        case KWL_INV_DISTANCE:
        {
            float gain = refDist / (refDist + rolloff * (distance - refDist));
            if (gain > 1.0f && clamp != 0)
            {
                gain = 1.0f;
            }
            return gain;
        }
        case KWL_LINEAR:
        {
            float gain = (1 - rolloff * (distance - refDist) / (maxDist - refDist));
            if (gain < 0.0f)
            {
                gain = 0.0f;
            }
            else if (gain > 1.0f && clamp != 0)
            {
                gain = 1.0f;
            }
            return gain;
        }
    }
    
    KWL_ASSERT(0 && "unknown distance attenuation model");
    return 1.0f;
}

void kwlEngine_updateMixPresets(kwlEngine* engine, float timeStepSec)
{
    /*TODO: read from project data?*/
    engine->engineData.mixPresetFadeTime = 1.0f;
    
    /*update mix preset weights towards the target values*/
    float dWeight = engine->engineData.mixPresetFadeTime > 0.0f ? timeStepSec / engine->engineData.mixPresetFadeTime : 1.0f;
    int i;
    for (i = 0; i < engine->engineData.numMixPresets; i++)
    {
        kwlMixPreset* preseti = &engine->engineData.mixPresets[i];
        //printf("updating %s to weight ", preseti->id);
        
        float delta = preseti->weight < preseti->targetWeight ? dWeight : - dWeight;
        if (preseti->weight != preseti->targetWeight)
        {
            preseti->weight += delta;
            if (preseti->weight < 0)
            {
                preseti->weight = 0;
            }
            else if (preseti->weight > 1)
            {
                preseti->weight = 1;
            }
        }
        //printf("%f by %f\n", preseti->weight, delta);
    }
    
    if (0)
    {
        for (i = 0; i < engine->engineData.numMixPresets; i++)
        {
            kwlMixPreset* preseti = &engine->engineData.mixPresets[i];
            
            printf("%s weight %f        ", preseti->id, preseti->weight);
        }
        printf("\n");
    }
    
    /* Blend mix preset parameter sets.  
       First reset all mix bus parameters...*/
    int mixBusIndex;
    for (mixBusIndex = 0; mixBusIndex < engine->engineData.numMixBuses; mixBusIndex++)
    {
        engine->engineData.mixBuses[mixBusIndex].mixPresetGainLeft = 0.0f;
        engine->engineData.mixBuses[mixBusIndex].mixPresetGainRight = 0.0f;
        engine->engineData.mixBuses[mixBusIndex].mixPresetPitch = 0.0f;
    }
    
    /*...then accumulate the values from each mix preset.*/
    float debugWeightSum[7] = {0, 0, 0, 0, 0, 0, 0};
    int j;
    for (j = 0; j < engine->engineData.numMixPresets; j++)
    {
        float presetweight = engine->engineData.mixPresets[j].weight;
        for (int paramSetIndex = 0; paramSetIndex < engine->engineData.numMixBuses; paramSetIndex++)
        {
            kwlMixBusParameters* params = &engine->engineData.mixPresets[j].parameterSets[paramSetIndex];
            int busIndex = params->mixBusIndex;
            KWL_ASSERT(busIndex < engine->engineData.numMixBuses && busIndex >= 0);
            engine->engineData.mixBuses[busIndex].mixPresetGainLeft += params->logGainLeft * presetweight;
            engine->engineData.mixBuses[busIndex].mixPresetGainRight += params->logGainRight * presetweight;
            engine->engineData.mixBuses[busIndex].mixPresetPitch += params->pitch * presetweight;
            debugWeightSum[busIndex] += presetweight;
        }
    }
    /*
    printf("debug weight sums: ");
    for (int b = 0; b < 7; b++)sfsdf
    {
        df
        printf("%f, ", debugWeightSum[b]);
    }
    printf("\n");*/
    
    /*Finally, convert from adjusted gain to linear gain.*/
    for (mixBusIndex = 0; mixBusIndex < engine->engineData.numMixBuses; mixBusIndex++)
    {
        engine->engineData.mixBuses[mixBusIndex].mixPresetGainLeft = 
            logGainToLinGain(engine->engineData.mixBuses[mixBusIndex].mixPresetGainLeft);
        engine->engineData.mixBuses[mixBusIndex].mixPresetGainRight = 
            logGainToLinGain(engine->engineData.mixBuses[mixBusIndex].mixPresetGainRight);
        
        /*printf("setting gain of bus %s to %f\n", 
               engine->mixBuses[mixBusIndex].id,
               engine->mixBuses[mixBusIndex].mixPresetGainLeft);*/
    }
}

float kwlEngine_getConeGain(kwlEngine* engine, float cosAngle, float cosInner, float cosOuter, float outerGain)
{
    float coneGain = 1.0f;
    if (cosAngle < cosOuter)
    {
        coneGain = outerGain; 
    }
    else if (cosAngle >= cosOuter &&
             cosAngle < cosInner)   
    {
        const float delta = cosInner - cosOuter;
        float param = 1.0f;
        
        if (delta != 0)
        {
            param = (cosAngle - cosOuter) / delta;
        }
        coneGain = outerGain + param * (1 - outerGain);
    }
    
    return coneGain;
}

void kwlEngine_updateEvents(kwlEngine* engine)
{
    /*cache some listener variables*/
    const float posXListener = engine->listener.positionX;
    const float posYListener = engine->listener.positionY;
    const float posZListener = engine->listener.positionZ;
    
    const float dirXListener = engine->listener.directionX;
    const float dirYListener = engine->listener.directionY;
    const float dirZListener = engine->listener.directionZ;
    
    const float rightXListener = engine->listener.rightX;
    const float rightYListener = engine->listener.rightY;
    const float rightZListener = engine->listener.rightZ;
    
    const float velXListener = engine->listener.velocityX;
    const float velYListener = engine->listener.velocityY;
    const float velZListener = engine->listener.velocityZ;
    
    const float cosInnerListener = engine->listener.innerConeCosAngle;
    const float cosOuterListener = engine->listener.outerConeCosAngle;
    const float outerGainListener = engine->listener.outerConeGain;
    
    const float speedOfSound = engine->positionalAudioSettings.speedOfSound;
    const float dopplerScale = engine->positionalAudioSettings.dopplerScale;
    
    const int eventConesEnabled = engine->positionalAudioSettings.isEventConeAttenuationEnabled;
    const int isDirectionalListener = engine->positionalAudioSettings.isListenerConeAttenuationEnabled &&
                                      engine->listener.outerConeGain != 1.0f; 
    
    /*recalculate positional gain and pitch of currently playing events*/
    kwlEventInstance* eventList = engine->playingEventList;
    while (eventList != NULL)
    {   
        kwlEventDefinition* definition = eventList->definition_engine;
        if (definition->isPositional)
        {
            /*compute a a normalized vector from the listener to the event*/
            float dx = posXListener - eventList->positionX;
            float dy = posYListener - eventList->positionY;
            float dz = posZListener - eventList->positionZ;
            const float distInv = kwlFastInverseSqrt(dx * dx + dy * dy + dz * dz);
            dx *= distInv;
            dy *= distInv;
            dz *= distInv;
            
            const float distanceAttenuation = kwlEngine_getDistanceGain(engine, distInv);
            
            /*pan. TODO: equal enery pan?*/
            float dot = -dx * rightXListener +
                        -dy * rightYListener +
                        -dz * rightZListener;
            float panLeft = 0.2f + (-dot > 0 ? -dot : 0);
            float panRight = 0.2f + (-dot < 0 ? dot : 0);
            
            /*cone attenuation*/
            int isDirectionalEvent = definition->outerConeGain != 1.0f;
            float coneGain = 1.0f;
            if (isDirectionalEvent && eventConesEnabled)
            {
                const float cosInner = definition->innerConeCosAngle;
                const float cosOuter = definition->outerConeCosAngle;
                const float outerGain = definition->outerConeGain;
                
                /*There are three ange intervals to consider:
                 - 0-inner cone angle: apply unit gain.
                 - inner cone angle - outer cone angle: 
                   interpolate between unit gain and outer cone gain
                 - outer cone angle - 180: apply outer cone gain
                 */
                float dotProd = eventList->directionX * dx +
                                eventList->directionY * dy +
                                eventList->directionZ * dz;
                
                coneGain = kwlEngine_getConeGain(engine, dotProd, cosInner, cosOuter, outerGain);
            }
            
            if (isDirectionalListener)
            {
                float dotProd = -dirXListener * dx +
                                -dirYListener * dy +
                                -dirZListener * dz;
                
                float listenerConeGain = 
                                kwlEngine_getConeGain(engine, 
                                                           dotProd, 
                                                           cosInnerListener, 
                                                           cosOuterListener, 
                                                           outerGainListener);
                coneGain *= listenerConeGain;
            }
            
            /*doppler shift:
             project velocities onto the unit vector 
             pointing from the listener to the event*/
            float vListener = velXListener * dx +    
                              velYListener * dy + 
                              velZListener * dz;
            float vEvent = eventList->velocityX * dx +    
            eventList->velocityY * dy + 
            eventList->velocityZ * dz;
            
            float dopplerShift = (1 - dopplerScale) + dopplerScale * (speedOfSound - vListener) / (speedOfSound - vEvent);
            if (dopplerShift < 0)
            {
                dopplerShift = 0.0001f;/*TODO: handle this properly*/
            }
            
            float positionalGainLeft = coneGain * distanceAttenuation * panLeft;
            float positionalGainRight = coneGain * distanceAttenuation * panRight;

            eventList->gainLeft.valueEngine = 
                eventList->definition_engine->gain * eventList->userGain * positionalGainLeft;
            eventList->gainRight.valueEngine = 
                eventList->definition_engine->gain * eventList->userGain * positionalGainRight;
            eventList->pitch.valueEngine = 
                eventList->definition_engine->pitch * eventList->userPitch * dopplerShift;
        }
        else 
        {
            float balanceGainLeft = 1 - eventList->balance;
            float balanceGainRight = 1 + eventList->balance;
            
            eventList->gainLeft.valueEngine = 
                eventList->definition_engine->gain * eventList->userGain * balanceGainLeft;
            eventList->gainRight.valueEngine = 
                eventList->definition_engine->gain * eventList->userGain * balanceGainRight;
            eventList->pitch.valueEngine = 
                eventList->definition_engine->pitch * eventList->userPitch;
        }
        
        if (eventList->dspUnit.valueMixer != NULL)
        {
            kwlDSPUnit* dspUnit = (kwlDSPUnit*)eventList->dspUnit.valueMixer;
            dspUnit->updateDSPEngineCallback(dspUnit->data);
        }
        
        eventList = eventList->nextEvent_engine;
    }
}


kwlError kwlEngine_update(kwlEngine* engine, float timeStepSec)
{
    kwlEngine_updateEvents(engine);        
    kwlEngine_updateMixPresets(engine, timeStepSec);
        
    kwlDSPUnit* inputDspUnit = (kwlDSPUnit*)engine->mixer->inputDSPUnit.valueEngine;
    if (inputDspUnit != NULL)
    {
        inputDspUnit->updateDSPEngineCallback(inputDspUnit->data);
    }
    
    kwlDSPUnit* outputDspUnit = (kwlDSPUnit*)engine->mixer->outputDSPUnit.valueEngine;
    if (outputDspUnit != NULL)
    {
        outputDspUnit->updateDSPEngineCallback(outputDspUnit->data);
    }
    
    /*************************************************************************
      The following section of code manipulates variables that are accessed from 
      both the engine thread and the mixer thread, so a lock is required to 
      protect them. A minimum amount of work should be done in this section.
     **************************************************************************/
    kwlMutexLockAcquire(&engine->mixerEngineMutexLock);
    
    /*Flush any locally buffered messages to the outgoing queue that is shared
      between the mixer thread and the engine thread.*/
    kwlMessageQueue_flushTo(&engine->toMixerQueue, &engine->toMixerQueueShared);
    
    /*copy any pending messages from the mixer*/
    kwlMessageQueue_flushTo(&engine->mixer->toEngineQueueShared, &engine->fromMixerQueue);
    
    /*update the mixer parameters of currently playing events */
    kwlEventInstance* eventList = engine->playingEventList;
    while (eventList != NULL)
    {
        eventList->gainLeft.valueShared = eventList->gainLeft.valueEngine;
        eventList->gainRight.valueShared = eventList->gainRight.valueEngine;
        eventList->pitch.valueShared = eventList->pitch.valueEngine;
        eventList->dspUnit.valueShared = eventList->dspUnit.valueEngine;
        
        eventList = eventList->nextEvent_engine;
    }
    
    const int numMixBuses = engine->engineData.numMixBuses;
    int i;
    for (i = 0; i < numMixBuses; i++)
    {
        kwlMixBus* busi = &engine->engineData.mixBuses[i];
        busi->totalGainLeft.valueShared = busi->mixPresetGainLeft * busi->userGainLeft;
        busi->totalGainRight.valueShared = busi->mixPresetGainRight * busi->userGainRight;
        busi->totalPitch.valueShared = busi->mixPresetPitch * busi->userPitch;
        busi->dspUnit.valueShared = busi->dspUnit.valueEngine;
    }
    
    engine->mixer->inputDSPUnit.valueShared = 
        engine->mixer->inputDSPUnit.valueEngine; 
    
    engine->mixer->outputDSPUnit.valueShared = 
        engine->mixer->outputDSPUnit.valueEngine; 
    
    engine->mixer->isLevelMeteringEnabled.valueShared = 
        engine->mixer->isLevelMeteringEnabled.valueEngine;
    
    engine->mixer->latestBufferAbsPeakLeft.valueEngine = 
        engine->mixer->latestBufferAbsPeakLeft.valueShared;
    engine->mixer->latestBufferAbsPeakRight.valueEngine = 
        engine->mixer->latestBufferAbsPeakRight.valueShared;
    engine->mixer->clipFlag.valueEngine = 
        engine->mixer->clipFlag.valueShared;
    engine->mixer->isPaused.valueShared = engine->mixer->isPaused.valueEngine;
    
    engine->mixer->numFramesMixed.valueEngine = engine->mixer->numFramesMixed.valueShared;
    
    /**************************************************************************
     done manipulating shared data. release the lock
     **************************************************************************/
    kwlMutexLockRelease(&engine->mixerEngineMutexLock);
    
    /*process messages from the mixer*/
    int numMessages = engine->fromMixerQueue.numMessages;
    
    int unloadEngineDataRequested = 0;
    for (i = 0; i < numMessages; i++)
    {
        kwlMessage* message = &engine->fromMixerQueue.messages[i];
        kwlMessageType type = message->type;
        void* messageData = message->data;
        /*printf("engine: processing incoming message %d/%d of type %d\n", i + 1, numMessages, type);*/
        
        if (type == KWL_EVENT_STOPPED ||
            type == KWL_UNLOAD_FREEFORM_EVENT)
        {
            kwlEventInstance* event = (kwlEventInstance*)messageData;
            event->isPlaying = 0;
            if (event->decoder != NULL)
            {
                kwlDecoder_deinit(event->decoder);
            }
            printf("    %s: %s\n", type == KWL_EVENT_STOPPED ? "event stopped" : "unload freeform event", 
                   event->definition_engine->id);
            kwlEngine_removeEventFromPlayingList(engine, event);
            
            if (type == KWL_UNLOAD_FREEFORM_EVENT)
            {
                kwlEngine_unloadFreeformEvent(engine, event);
            }
            
            if (event->stoppedCallback != NULL)
            {
                event->stoppedCallback(event->stoppedCallbackUserData);
            }
        }
        else if (type == KWL_UNLOAD_WAVEBANK)
        {
            kwlWaveBank* waveBank = (kwlWaveBank*)messageData;
            printf("    unload wave bank: %s\n", waveBank->id);
            kwlWaveBank_unload(waveBank);
        }
        else if (type == KWL_UNLOAD_ENGINE_DATA)
        {
            /*Unload engine data after all messages have been processed.*/
            KWL_ASSERT(unloadEngineDataRequested == 0);
            printf("received KWL_UNLOAD_ENGINE_DATA\n");
            unloadEngineDataRequested = 1;
        }
    }
    
    if (unloadEngineDataRequested != 0)
    {
        kwlEngineData_unload(&engine->engineData);
    }
    
    engine->fromMixerQueue.numMessages = 0;

    return KWL_NO_ERROR;
}

kwlError kwlEngine_resume(kwlEngine* engine)
{
    engine->mixer->isPaused.valueEngine = 0;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_pause(kwlEngine* engine)
{
    engine->mixer->isPaused.valueEngine = 1;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_startEventInstance(kwlEngine* engine, 
                                           kwlEventInstance* eventToPlay, 
                                           float fadeInTimeSec)
{
    /* If the event is not playing. */
    if (eventToPlay->isPlaying == 0)
    {
        /* If this is a streaming event...*/
        if (eventToPlay->definition_engine->streamAudioData != NULL)
        {
            if (eventToPlay->definition_engine->streamAudioData->isLoaded == 0)
            {
                return KWL_NO_ERROR; /*TODO: return some other error here?*/
            }
            /*...find a decoder and initialise it*/
            int freeDecoderIdx = -1;
            int i;
            for (i = 0; i < engine->numDecoders; i++)
            {
                if (engine->decoders[i].codecData == NULL)
                {
                    freeDecoderIdx = i;
                    break;
                }
            }
            if (freeDecoderIdx < 0)
            {
                return KWL_NO_FREE_DECODERS;
            }
            eventToPlay->decoder = &engine->decoders[freeDecoderIdx];
            kwlError initResult = kwlDecoder_init(eventToPlay->decoder, 
                                                  eventToPlay);

            if (initResult != KWL_NO_ERROR)
            {
                return initResult;
            }
        }
            
        /*mark the event as playing and send a start message to the mixer.*/
        eventToPlay->isPlaying = 1;
        kwlEngine_addEventToPlayingList(engine, eventToPlay);
        int result = kwlMessageQueue_addMessageWithParam(&engine->toMixerQueue, 
                                                         KWL_EVENT_START, 
                                                         eventToPlay, 
                                                         fadeInTimeSec);
        
        if (result == 0)
        {
            return KWL_MESSAGE_QUEUE_FULL;
        }
    }
    /* If the event is playing and is not a streaming event, retrigger it. */
    else if (eventToPlay->definition_engine->streamAudioData == NULL)
    {
        /*the event is already flagged as playing. this means that it is either:
         1. playing in the mixer (the most likely case)
         2. not playing in the mixer but the stopped notification has not yet reached the engine. */
        
        /*mark the event as playing and send a retrigger message to the mixer.*/
        eventToPlay->isPlaying = 1;
        //kwlEngine_addEventToPlayingList(engine, eventToPlay);
        int result = kwlMessageQueue_addMessageWithParam(&engine->toMixerQueue, 
                                                         KWL_EVENT_RETRIGGER, 
                                                         eventToPlay, 
                                                         fadeInTimeSec);
        
        if (result == 0)
        {
            return KWL_MESSAGE_QUEUE_FULL;
        }
    }
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_eventStart(kwlEngine* engine, const int handle, float fadeInTimeSec)
{
    kwlEventInstance* eventToPlay = kwlEngine_getEventFromHandle(engine, handle);
    
    if (eventToPlay == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (fadeInTimeSec < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    return kwlEngine_startEventInstance(engine, eventToPlay, fadeInTimeSec);
    
}

kwlError kwlEngine_eventStartOneShot(kwlEngine* engine,
                                     kwlEventDefinitionHandle handle,
                                     float x, float y, float z,
                                     int startAtPosition,
                                     kwlEventStoppedCallack stoppedCallback,
                                     void* stoppedCallbackUserData)
{
    /* Check handle*/
    if (handle == KWL_INVALID_HANDLE ||
        handle < 0 ||
        handle >= engine->engineData.numEventDefinitions)
    {
        return KWL_INVALID_EVENT_DEFINITION_HANDLE;
    }
    
    kwlEventDefinition* definition = &engine->engineData.eventDefinitions[handle];
    
    if (startAtPosition != 0 && definition->isPositional == 0)
    {
        return KWL_EVENT_IS_NOT_POSITIONAL;
    }
    else if (startAtPosition == 0 && definition->isPositional != 0)
    {
        /*If positional events are played "non-positionally", put them at the origin.*/
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }
    
    /* Look for a free event instance that is not currently playing.*/
    int instanceCount = definition->instanceCount;
    kwlEventInstance* instanceToStart = NULL;
    int numStealableInstances = 0;
    int i;
    for (i = 0; i < instanceCount; i++)
    {
        if (engine->engineData.events[handle][i].isAssociatedWithHandle == 0)
        {
            numStealableInstances++;
            if (engine->engineData.events[handle][i].isPlaying == 0)
            {
                instanceToStart = &engine->engineData.events[handle][i];
            }
        }
    }
    
    /*printf("instance to start: %d, n stealable=%d\n", 
             instanceToStart, 
             numStealableInstances);*/
    
    if (numStealableInstances == 0)
    {
        /*All instances of the event definition are currently associated with
          handles.*/
        return KWL_NO_FREE_EVENT_INSTANCES;
    }
    
    if (instanceToStart == NULL)
    {
        /*No free instance was found, but there are instances to steal.*/
        kwlEventInstanceStealingMode stealingMode = definition->stealingMode;
        if (stealingMode == KWL_DONT_STEAL)
        {
            /*Fail silently if instance stealing is not allowed.*/
            return KWL_NO_ERROR;
        }
        else if (stealingMode == KWL_STEAL_RANDOM)
        {
            /*Steal a randomly selected instance.*/
            int stealIndex = rand() % numStealableInstances;
            int stealableIndex = 0;

            for (i = 0; i < instanceCount; i++)
            {
                if (engine->engineData.events[handle][i].isAssociatedWithHandle == 0 &&
                    engine->engineData.events[handle][i].isPlaying == 1)
                {
                    if (stealableIndex == stealIndex)
                    {
                        instanceToStart = &engine->engineData.events[handle][i];
                        break;
                    }
                    stealableIndex++;
                }
            }
        }
        else if (stealingMode == KWL_STEAL_QUIETEST)
        {
            /*Find the instance with the lowest gain and steal it.*/
            float minGain = -1;

            for (i = 0; i < instanceCount; i++)
            {
                if (engine->engineData.events[handle][i].isAssociatedWithHandle == 0 &&
                    engine->engineData.events[handle][i].isPlaying == 1)
                {
                    /*Compare against the average channel gain of the instance.*/
                    float gain = engine->engineData.events[handle][i].gainLeft.valueEngine +
                                 engine->engineData.events[handle][i].gainRight.valueEngine;
                    if (minGain < 0 || gain < minGain)
                    {
                        instanceToStart = &engine->engineData.events[handle][i];
                    }
                }
            }
        }
        
        //since this instance is about to be stolen and thus stopped,
        //fire the stopped callback
        if (instanceToStart->stoppedCallback != NULL)
        {
            instanceToStart->stoppedCallback(instanceToStart->stoppedCallbackUserData);
        }
    }
    
    KWL_ASSERT(instanceToStart != NULL && "no one-shot instance found");
    if (startAtPosition)
    {
        instanceToStart->positionX = x;
        instanceToStart->positionY = y;
        instanceToStart->positionZ = z;
        
        instanceToStart->velocityX = 0.0f;
        instanceToStart->velocityY = 0.0f;
        instanceToStart->velocityZ = 0.0f;
    }
    
    instanceToStart->stoppedCallback = stoppedCallback;
    instanceToStart->stoppedCallbackUserData = stoppedCallbackUserData;
    
    return kwlEngine_startEventInstance(engine, instanceToStart, 0.0f);
}

kwlError kwlEngine_eventSetStoppedCallback(kwlEngine* engine, const int handle, 
                                                kwlEventStoppedCallack stoppedCallback,
                                                void* stoppedCallbackUserData)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, handle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (stoppedCallback == NULL)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    event->stoppedCallback = stoppedCallback;
    event->stoppedCallbackUserData = stoppedCallbackUserData;
    
    return KWL_NO_ERROR;
}

/** */
kwlError kwlEngine_eventStop(kwlEngine* engine, const int handle, float fadeOutTimeSec)
{
    kwlEventInstance* eventToStop = kwlEngine_getEventFromHandle(engine, handle);
    if (eventToStop == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (fadeOutTimeSec < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    int result = kwlMessageQueue_addMessageWithParam(&engine->toMixerQueue, KWL_EVENT_STOP, eventToStop, fadeOutTimeSec);
    if (result == 0)
    {
        return KWL_MESSAGE_QUEUE_FULL;
    }
    
    return KWL_NO_ERROR;
}

/** */
kwlError kwlEngine_eventPause(kwlEngine* engine, const int handle)
{
    kwlEventInstance* eventToPause = kwlEngine_getEventFromHandle(engine, handle);
    if (eventToPause == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    int result = kwlMessageQueue_addMessage(&engine->toMixerQueue, KWL_EVENT_PAUSE, eventToPause);
    if (result == 0)
    {
        return KWL_MESSAGE_QUEUE_FULL;
    }
    
    return KWL_NO_ERROR;
}

/** */
kwlError kwlEngine_eventResume(kwlEngine* engine, const int handle)
{
    kwlEventInstance* eventToResume = kwlEngine_getEventFromHandle(engine, handle);
    if (eventToResume == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    int result = kwlMessageQueue_addMessage(&engine->toMixerQueue, KWL_EVENT_RESUME, eventToResume);
    if (result == 0)
    {
        return KWL_MESSAGE_QUEUE_FULL;
    }
    
    return KWL_NO_ERROR;
}

/** */
kwlError kwlEngine_eventIsPlaying(kwlEngine* engine, const int handle, int* isPlaying)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, handle);
    if (event == NULL)
    {
        *isPlaying = 0;
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    *isPlaying = event->isPlaying;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_eventSetPitch(kwlEngine* engine, kwlEventHandle eventHandle, float pitch)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, eventHandle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (pitch < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    event->userPitch = pitch;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_eventSetPosition(kwlEngine* engine, kwlEventHandle handle, float posX, float posY, float posZ)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, handle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (event->definition_engine->isPositional == 0)
    {
        return KWL_EVENT_IS_NOT_POSITIONAL;
    }
    
    event->positionX = posX;
    event->positionY = posY;
    event->positionZ = posZ;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_eventSetOrientation(kwlEngine* engine, kwlEventHandle handle, float directionX, float directionY, float directionZ)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, handle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (event->definition_engine->isPositional == 0)
    {
        return KWL_EVENT_IS_NOT_POSITIONAL;
    }
    
    if (directionX == 0.0f && directionY == 0.0f && directionZ == 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    float length = kwlFastInverseSqrt(directionX * directionX + directionY * directionY + directionZ * directionZ);
    
    event->directionX = directionX / length;
    event->directionY = directionY / length;
    event->directionZ = directionZ / length;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_eventSetVelocity(kwlEngine* engine, kwlEventHandle handle, float velX, float velY, float velZ)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, handle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (event->definition_engine->isPositional == 0)
    {
        return KWL_EVENT_IS_NOT_POSITIONAL;
    }
    
    event->velocityX = velX;
    event->velocityY = velY;
    event->velocityZ = velZ;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_eventSetBalance(kwlEngine* engine, kwlEventHandle handle, float balance)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, handle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (event->definition_engine->isPositional != 0)
    {
        return KWL_EVENT_IS_NOT_NONPOSITIONAL;
    }
    
    if (balance > 1.0f || balance < -1.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    event->balance = balance;
    
    return KWL_NO_ERROR;
}
    
kwlError kwlEngine_eventSetGain(kwlEngine* engine, kwlEventHandle eventHandle, float gain, int isLinearGain)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, eventHandle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    if (gain < 0.0f)
    {
        return KWL_INVALID_PARAMETER_VALUE;
    }
    
    event->userGain = isLinearGain == 1 ? gain : logGainToLinGain(gain);
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_attachDSPUnitToEvent(kwlEngine* engine, kwlEventHandle eventHandle, kwlDSPUnit* dspUnit)
{
    kwlEventInstance* event = kwlEngine_getEventFromHandle(engine, eventHandle);
    if (event == NULL)
    {
        return KWL_INVALID_EVENT_INSTANCE_HANDLE;
    }
    
    event->dspUnit.valueEngine = dspUnit;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_attachDSPUnitToMixBus(kwlEngine* engine, kwlMixBusHandle mixBusHandle, kwlDSPUnit* dspUnit)
{
    kwlMixBus* bus = kwlEngine_getMixBusFromHandle(engine, mixBusHandle);
    if (bus == NULL)
    {
        return KWL_INVALID_MIX_BUS_HANDLE;
    }
    
    bus->dspUnit.valueEngine = dspUnit;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_attachDSPUnitToInput(kwlEngine* engine, kwlDSPUnit* dspUnit)
{
    engine->mixer->inputDSPUnit.valueEngine = dspUnit;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_attachDSPUnitToOutput(kwlEngine* engine, kwlDSPUnit* dspUnit)
{
    engine->mixer->outputDSPUnit.valueEngine = dspUnit;
    return KWL_NO_ERROR;
}


void debugPrintEventList(kwlEventInstance* event)
{
    kwlEventInstance* temp = event;
    while (temp != NULL)
    {
        printf("    %s\n", temp->definition_engine->id);
        temp  = temp->nextEvent_engine;
    }
}

/** */
void kwlEngine_addEventToPlayingList(kwlEngine* engine, kwlEventInstance* eventToAdd)
{
    printf("about to add %s to list:\n", eventToAdd->definition_engine->id);
    //debugPrintEventList(engine->playingEventList);
    
    /*verify that the event is not already in the list*/
    /*kwlEventInstance* temp = engine->playingEventList;
    if (temp != NULL)
    {
        while (temp != NULL)
        {
            KWL_ASSERT(temp != eventToAdd && "event is already in the 'playing' list");
            temp = temp->nextEvent_engine;
        }
    }*/
    
    /*add the event to the linked list of playing events*/
    KWL_ASSERT(eventToAdd->nextEvent_mixer == NULL);
    if (engine->playingEventList == NULL)        
    {
        engine->playingEventList = eventToAdd;
    }
    else
    {
        kwlEventInstance* e = engine->playingEventList;
        while (e->nextEvent_engine != NULL)
        {
            e = e->nextEvent_engine;
        }
        e->nextEvent_engine = eventToAdd;
    }
    
    eventToAdd->nextEvent_engine = NULL;
    
    //printf("added %s to list:\n", eventToAdd->definition_engine->id);
    //debugPrintEventList(engine->playingEventList);
}

/** */
void kwlEngine_removeEventFromPlayingList(kwlEngine* engine, kwlEventInstance* event)
{
    kwlEventInstance* prevEvent = NULL;
    kwlEventInstance* eventi = engine->playingEventList;
    
    while (eventi != event)
    {
        prevEvent = eventi;
        eventi = eventi->nextEvent_engine;
    }
    
    if (prevEvent == NULL)
    {
        engine->playingEventList = eventi->nextEvent_engine;
    }
    else
    {
        prevEvent->nextEvent_engine = eventi->nextEvent_engine;
    }
    
    event->nextEvent_engine = NULL;
    
    //printf("about to remove %s from list:\n", event->definition_engine->id);
    /*
    debugPrintEventList(engine->playingEventList);
    
    //verify that the event is already in the list
    kwlEventInstance* temp = engine->playingEventList;
    KWL_ASSERT(temp != NULL && "trying to remove event from empty list");
    
    int foundMatch = 0;
    if (temp != NULL)
    {
        while (temp != NULL)
        {
            if (temp == eventToRemove)
            {
                foundMatch = 1;
                break;
            }
            temp = temp->nextEvent_engine;
        }
    }
    KWL_ASSERT(foundMatch && "event to be removed is not in the 'playing' list");
    
    //remove the event from the linked list of playing events.
    kwlEventInstance* event = engine->playingEventList;
    if (event == eventToRemove)
    {
        engine->playingEventList = event->nextEvent_engine;
    }
    else 
    {
        while (event->nextEvent_engine != eventToRemove)
        {
            event = event->nextEvent_engine;
        }
        KWL_ASSERT(event->nextEvent_engine == eventToRemove);
        event->nextEvent_engine = eventToRemove->nextEvent_engine;
    }
    
    printf("removed %s from list:\n", eventToRemove->definition_engine->id);
    debugPrintEventList(engine->playingEventList);*/
}

/*****************************************************************************
 * Data loading/unloading methods 
 *****************************************************************************/

kwlError kwlEngine_loadEngineData(kwlEngine* engine, kwlInputStream* stream)
{
    return kwlEngineData_load(&engine->engineData, stream);

}

/** */
kwlError kwlEngine_initialize(kwlEngine* engine, 
                                   int sampleRate, 
                                   int numOutChannels,
                                   int numInChannels,
                                   int bufferSize)
{    
    //TODO: set this elsewhere?
    engine->mixer->sampleRate = sampleRate;
    engine->mixer->numOutChannels = numOutChannels;
    engine->mixer->numInChannels = numInChannels;
    engine->isInputEnabled = numInChannels > 0 ? 1 : 0;
    
    kwlMixer_allocateTempBuffers(engine->mixer);
    
    kwlError result = kwlEngine_hostSpecificInitialize(engine, sampleRate, numOutChannels, numInChannels, bufferSize);
    
    return result;
}

kwlError kwlEngine_isLoaded(kwlEngine* engine, int* ret)
{
    *ret = engine->engineData.isLoaded;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_engineDataLoad(kwlEngine* engine, const char* const dataFile)
{
    if (engine->engineData.isLoaded != 0)
    {
        return KWL_ENGINE_ALREADY_LOADED;
    }
    
    kwlInputStream stream;
    kwlError result = kwlInputStream_initWithFile(&stream, dataFile);

    if (result != KWL_NO_ERROR)
    {
        kwlInputStream_close(&stream);
        return result;
    }
    
    result = kwlEngine_loadEngineData(engine, &stream);
    
    kwlInputStream_close(&stream);
    
    if (result != KWL_NO_ERROR)
    {
        return result;
    }
    
    /*If we made it here, loading went well. Notify the mixer that the mix 
      bus hierarchy has been loaded.*/
    int success = kwlMessageQueue_addMessageWithParam(&engine->toMixerQueue, 
                                                      KWL_SET_MASTER_BUS, 
                                                      engine->engineData.mixBuses, 
                                                      engine->engineData.numMixBuses);
    KWL_ASSERT(success != 0);
    return KWL_NO_ERROR;
}

kwlError kwlEngine_unloadEngineDataBlocking(kwlEngine* engine)
{
    if (engine->engineData.isLoaded == 0)
    {
        return KWL_NO_ERROR;
    }
    
    /*Send a message to the mixer requesting it to: 
     - stop any playing data driven events
     - change its mix bus array to only contain the always valid master bus.
      The mixer then sends a KWL_UNLOAD_ENGINE_DATA message back to the engine thread
      that triggers the actual unloading.*/
    int result = kwlMessageQueue_addMessage(&engine->toMixerQueue, KWL_PREPARE_ENGINE_DATA_UNLOAD, NULL);
    //printf("posted KWL_PREPARE_ENGINE_DATA_UNLOAD\n");
    KWL_ASSERT(result != 0);
    
    /*Block until the engine data has been unloaded.*/
    while (engine->engineData.isLoaded != 0)
    {
        /*printf("waiting for mixer to stop data driven events and clear mix buses\n");*/
        kwlUpdate(0);
    }
    
    return KWL_NO_ERROR;
}

/** */
void kwlEngine_deinitialize(kwlEngine* engine)
{
    /* Unload any engine data and wave banks*/
    kwlEngineDataUnload();
    /* Shut down the sound system.*/
    kwlEngine_hostSpecificDeinitialize(engine);
}

kwlError kwlEngine_getNumFramesMixed(kwlEngine* engine, unsigned int* numFrames)
{
    if (engine->lastNumFramesMixed == 0)
    {
        *numFrames = 0;
    }
    else
    {
        *numFrames = engine->mixer->numFramesMixed.valueEngine - engine->lastNumFramesMixed;
    }
    
    engine->lastNumFramesMixed = engine->mixer->numFramesMixed.valueEngine;
    
    return KWL_NO_ERROR;
}

kwlError kwlEngine_hasClipped(kwlEngine* engine, int* hasClipped)
{
    if (engine->mixer->isLevelMeteringEnabled.valueEngine == 0)  
    {
        *hasClipped = 0;
        return KWL_LEVEL_METERING_DISABLED;
    }
    
    *hasClipped = engine->mixer->clipFlag.valueEngine;
    return KWL_NO_ERROR;
}

kwlError kwlEngine_getOutLevels(kwlEngine* engine, float* leftLevel, float* rightLevel)
{    
    if (engine->mixer->isLevelMeteringEnabled.valueEngine == 0)  
    {
        *rightLevel = 0.0f;
        *leftLevel = 0.0f;
        return KWL_LEVEL_METERING_DISABLED;
    }
    
    *leftLevel = engine->mixer->latestBufferAbsPeakLeft.valueEngine;
    *rightLevel = engine->mixer->latestBufferAbsPeakRight.valueEngine;
    
    return KWL_NO_ERROR;
}
