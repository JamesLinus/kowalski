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

#include "kwl_eventinstance.h"
#include "kowalski.h"
#include "kowalski.h"
#include "kwl_audiofileutil.h"
#include "kwl_dspunit.h"
#include "kwl_memory.h"
#include "kwl_engine.h"

#include "kwl_assert.h"
#include <stdlib.h>
#include <string.h>

kwlEngine* engine = NULL;

kwlError error = KWL_NO_ERROR;

static void kwlSetError(kwlError err)
{
    if (err != KWL_NO_ERROR)
    {
        //printf("KWL ERROR: %d\n", err);
    }
    
    if (error == KWL_NO_ERROR)
    {
        error = err;
    }
}

kwlError kwlGetError(void)
{
    kwlError errorToReturn = error;
    error = KWL_NO_ERROR;
    return errorToReturn;
}

void kwlEventSetPitch(kwlEventHandle handle, float pitchInPercent)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    kwlSetError(kwlEngine_eventSetPitch(engine, handle, pitchInPercent));
}

void kwlEventSetGain(kwlEventHandle handle, float gain)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventSetGain(engine, handle, gain, 0));
}

void kwlEventSetLinearGain(kwlEventHandle handle, float gain)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventSetGain(engine, handle, gain, 1));
}


void kwlEventSetPosition(kwlEventHandle handle, float posX, float posY, float posZ)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventSetPosition(engine, handle, posX, posY, posZ));
}

void kwlEventSetVelocity(kwlEventHandle handle, float velX, float velY, float velZ)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventSetVelocity(engine, handle, velX, velY, velZ));
}

void kwlEventSetOrientation(kwlEventHandle handle, float directionX, float directionY, float directionZ)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventSetOrientation(engine, handle, directionX, directionY, directionZ));
}

void kwlEventSetBalance(kwlEventHandle handle, float balance)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventSetBalance(engine, handle, balance));
}

kwlEventHandle kwlEventGetHandle(const char* const eventId)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return KWL_INVALID_HANDLE;
    }
    
    kwlEventHandle handle = 0;
    kwlSetError(kwlEngine_eventGetHandle(engine, eventId, &handle));
    return handle;
}

kwlEventDefinitionHandle kwlEventDefinitionGetHandle(const char* const eventDefinitionID)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return KWL_INVALID_HANDLE;
    }
    
    kwlEventDefinitionHandle handle = 0;
    kwlSetError(kwlEngine_eventDefinitionGetHandle(engine, 
                                                        eventDefinitionID, 
                                                        &handle));
    return handle;
}

kwlEventHandle kwlEventCreateWithFile(const char* const audioFilePath, kwlEventType eventType, int streamFromDisk)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return KWL_INVALID_HANDLE;
    }
    
    kwlEventHandle handle = 0;
    kwlSetError(kwlEngine_eventCreateWithFile(engine, audioFilePath, &handle, eventType, streamFromDisk));
    return handle;
}

kwlEventHandle kwlEventCreateWithBuffer(kwlPCMBuffer* buffer, kwlEventType eventType)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return KWL_INVALID_HANDLE;
    }
    
    kwlEventHandle handle = 0;
    kwlSetError(kwlEngine_eventCreateWithBuffer(engine, buffer, &handle, eventType));
    return handle;
}

void kwlEventRelease(kwlEventHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventRelease(engine, handle));
}

void kwlEventStart(kwlEventHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventStart(engine, handle, 0));
}

void kwlEventStartOneShot(kwlEventDefinitionHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }    
    
    kwlSetError(kwlEngine_eventStartOneShot(engine, handle, 0.0f, 0.0f, 0.0f, 0, NULL, NULL));
}

void kwlEventStartOneShotAt(kwlEventDefinitionHandle handle, float x, float y, float z)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventStartOneShot(engine, handle, x, y, z, 1, NULL, NULL));
}

void kwlEventSetCallback(kwlEventHandle handle, kwlEventStoppedCallack callback, void* userData)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventSetStoppedCallback(engine, handle, callback, userData));
    
}

void kwlEventStartOneShotWithCallback(kwlEventDefinitionHandle eventDefinition, kwlEventStoppedCallack callback, void* userData)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventStartOneShot(engine, eventDefinition, 0.0f, 0.0f, 0.0f, 0, callback, userData));
}

void kwlEventStartOneShotWithCallbackAt(kwlEventDefinitionHandle eventDefinition, float x, float y, float z, kwlEventStoppedCallack callback, void* userData)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventStartOneShot(engine, eventDefinition, x, y, z, 1, callback, userData));
    
}

void kwlEventStartFade(kwlEventHandle handle, float fadeTime)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventStart(engine, handle, fadeTime));
}

void kwlEventStop(kwlEventHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventStop(engine, handle, 0));
}

void kwlEventStopFade(kwlEventHandle handle, float fadeTime)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventStop(engine, handle, fadeTime));
}

void kwlEventPause(kwlEventHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventPause(engine, handle));
}

void kwlEventResume(kwlEventHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_eventResume(engine, handle));
}

int kwlEventIsPlaying(kwlEventHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0;
    }
    
    int ret = 0;
    kwlSetError(kwlEngine_eventIsPlaying(engine, handle, &ret));
    return ret;
}

/** 
 * 
 */
kwlMixBusHandle kwlMixBusGetHandle(const char* const busId)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return KWL_INVALID_HANDLE;
    }
    
    kwlMixBusHandle handle = 0;
    kwlSetError(kwlEngine_mixBusGetHandle(engine, busId, &handle));
    return handle;
}

void kwlMixBusSetGain(kwlMixBusHandle handle, float gain)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_mixBusSetGain(engine, handle, gain, 0));
}

void kwlMixBusSetLinearGain(kwlMixBusHandle handle, float gain)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_mixBusSetGain(engine, handle, gain, 1));
}

void kwlMixBusSetPitch(kwlMixBusHandle handle, float pitch)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_mixBusSetPitch(engine, handle, pitch));
}


kwlMixPresetHandle kwlMixPresetGetHandle(const char* const presetId)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return KWL_INVALID_HANDLE;
    }
    
    kwlMixPresetHandle handle = 0;
    kwlSetError(kwlEngine_mixPresetGetHandle(engine, presetId, &handle));
    return handle;
}

void kwlMixPresetFadeTo(kwlMixPresetHandle presetHandle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_mixPresetSetActive(engine, presetHandle, 1));
}

void kwlMixPresetSet(kwlMixPresetHandle presetHandle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_mixPresetSetActive(engine, presetHandle, 0));
}

void kwlListenerSetPosition(float posX, float posY, float posZ)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_setListenerPosition(engine, posX, posY, posZ));
}

void kwlListenerSetVelocity(float velX, float velY, float velZ)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_setListenerVelocity(engine, velX, velY, velZ));
}

void kwlListenerSetOrientation(float directionX, float directionY, float directionZ,
                               float upX, float upY, float upZ)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_setListenerOrientation(engine, 
                                                      directionX, directionY, directionZ,
                                                      upX, upY, upZ));
}

void kwlSetDistanceAttenuationModel(kwlDistanceAttenuationModel type, 
                                    int clamp,
                                    float maxDistance,
                                    float rolloffFactor,
                                    float referenceDistance)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_setDistanceAttenuationModel(engine,
                                                           type, 
                                                           clamp, 
                                                           maxDistance, 
                                                           rolloffFactor, 
                                                           referenceDistance));
}

void kwlSetDopplerShiftParameters(float speedOfSound, float dopplerScale)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_setDopplerShiftParameters(engine, speedOfSound, dopplerScale));
}

void kwlSetConeAttenuationEnabled(int enableListenerCone, int enableEventCones)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_setConeAttenuationEnabled(engine, enableListenerCone, enableEventCones));
}

void kwlListenerSetConeParameters(float innerConeAngle, float outerConeAngle, float outerConeGain)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_setListenerConeParameters(engine, innerConeAngle, outerConeAngle, outerConeGain));
}

void kwlMixerResume(void)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_resume(engine));
}

void kwlMixerPause(void)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_pause(engine));
}

float kwlGetLevelLeft(void)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0.0f;
    }
    
    float levelLeft = 0.0f;
    float levelRight = 0.0f;
    kwlSetError(kwlEngine_getOutLevels(engine, &levelLeft, &levelRight));
    return levelLeft;
}

float kwlGetLevelRight(void)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0.0f;
    }
    
    float levelLeft = 0.0f;
    float levelRight = 0.0f;
    kwlSetError(kwlEngine_getOutLevels(engine, &levelLeft, &levelRight));
    return levelRight;
}

int kwlHasClipped(void)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0;
    }
    
    int hasClipped = 0;
    kwlSetError(kwlEngine_hasClipped(engine, &hasClipped));
    return hasClipped;
}

void kwlLevelMeteringSetEnabled(int enabled)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    engine->mixer->isLevelMeteringEnabled.valueEngine = enabled;
}


void kwlUpdate(float timeStepSec)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    if (timeStepSec < 0.0f)
    {
        kwlSetError(KWL_INVALID_PARAMETER_VALUE);
        return;
    }
    
    kwlSetError(kwlEngine_update(engine, timeStepSec));
}

kwlWaveBankHandle kwlWaveBankLoad(const char* const path)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return KWL_INVALID_HANDLE;
    }
    kwlWaveBankHandle handle = 0;
    kwlSetError(kwlEngine_loadWaveBank(engine, path, &handle, 0));
    return handle;
}

int kwlWaveBankIsLoaded(kwlWaveBankHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0;
    }
    
    int isLoaded = 0;
    kwlSetError(kwlEngine_waveBankIsLoaded(engine, handle, &isLoaded));
    return isLoaded;
}

int kwlWaveBankIsReferencedByPlayingEvent(kwlWaveBankHandle handle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0;
    }
    
    int isReferenced = 0;
    kwlSetError(kwlEngine_waveBankIsReferencedByPlayingEvent(engine, handle, &isReferenced));
    return isReferenced;
}

void kwlWaveBankUnload(kwlWaveBankHandle waveBankHandle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    kwlSetError(kwlEngine_requestUnloadWaveBank(engine, waveBankHandle, 0));
}

void kwlWaveBankUnloadBlocking(kwlWaveBankHandle waveBankHandle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    kwlSetError(kwlEngine_requestUnloadWaveBank(engine, waveBankHandle, 1));
}

unsigned int kwlGetNumFramesMixed()
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0.0f;
    }
    
    unsigned int numFramesMixed = 0;
    kwlSetError(kwlEngine_getNumFramesMixed(engine, &numFramesMixed));
    return numFramesMixed;
}

int kwlIsEngineInitialized()
{
    return engine != NULL;
}

/** */
void kwlInitialize(int sampleRate, int numOutputChannels, int numInputChannels, int bufferSize)
{
    if (engine != 0)
    {
        kwlSetError(KWL_ENGINE_ALREADY_INITIALIZED);
        return;
    }
    
    if (numOutputChannels != 1 && numOutputChannels != 2)
    {
        kwlSetError(KWL_UNSUPPORTED_NUM_OUTPUT_CHANNELS);
        return;
    }
    
    if (numInputChannels < 0 || numInputChannels > 2)
    {
        kwlSetError(KWL_UNSUPPORTED_NUM_INPUT_CHANNELS);
        return;
    }
    
    if (sampleRate <= 0 || bufferSize <= 0)
    {
        kwlSetError(KWL_INVALID_PARAMETER_VALUE);
        return;
    }

    /*create the sound engine instance*/
    engine = (kwlEngine*)KWL_MALLOC((sizeof(kwlEngine)), "kwlInitialize");
    kwlMemset(engine, 0, sizeof(kwlEngine));
    kwlEngine_init(engine);
    
    /*and initialise it*/
    kwlSetError(kwlEngine_initialize(engine, sampleRate, numOutputChannels, numInputChannels, bufferSize));
}

/** */
void kwlEngineDataLoad(const char* const dataPath)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_engineDataLoad(engine, dataPath));
}

/** */
void kwlEngineDataUnload()
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_unloadEngineDataBlocking(engine));
}

/** */
int kwlEngineDataIsLoaded()
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0;
    }
    
    int ret = 0;
    kwlSetError(kwlEngine_isLoaded(engine, &ret));
    return ret;
}

/** */
void kwlDeinitialize()
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    /*shut down the sound engine*/
    kwlEngine_deinitialize(engine);
    /*delete the sound engine instance*/
    kwlEngine_free(engine);
    engine = NULL;
}

/** */
void kwlDSPUnitAttachToEvent(kwlDSPUnit* dspUnit, kwlEventHandle eventHandle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_attachDSPUnitToEvent(engine, eventHandle, dspUnit));
}


void kwlDSPUnitAttachToMixBus(kwlDSPUnit* dspUnit, kwlMixBusHandle mixBusHandle)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_attachDSPUnitToMixBus(engine, mixBusHandle, dspUnit));
}

void kwlDSPUnitAttachToInput(kwlDSPUnit* dspUnit)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_attachDSPUnitToInput(engine, dspUnit));
}

void kwlDSPUnitAttachToOutput(kwlDSPUnit* dspUnit)
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return;
    }
    
    kwlSetError(kwlEngine_attachDSPUnitToOutput(engine, dspUnit));
}

int kwlIsInputEnabled()
{
    if (engine == NULL)
    {
        kwlSetError(KWL_ENGINE_IS_NOT_INITIALIZED);
        return 0;
    }
    
    return engine->isInputEnabled;
}

kwlDSPUnitHandle kwlDSPUnitCreateCustom(void* userdata, 
                                        kwlDSPCallback process, 
                                        kwlDSPUpdateCallback updateEngine, 
                                        kwlDSPUpdateCallback updateMixer, 
                                        kwlDSPCleanupCallback cleanup)
{
    if (process == NULL)
    {
        kwlSetError(KWL_INVALID_PARAMETER_VALUE);
        return NULL;
    }
    
    /*This method can be called regardless of the state of the engine */
    kwlDSPUnit* newDSPUnit = (kwlDSPUnit*)KWL_MALLOC(sizeof(kwlDSPUnit), "custom DSP unit");
    kwlMemset(newDSPUnit, 0, sizeof(kwlDSPUnit));
    
    newDSPUnit->type = KWL_CUSTOM_DSP_UNIT;
    newDSPUnit->data = userdata;
    newDSPUnit->dspCallback = process;
    newDSPUnit->updateDSPEngineCallback = updateEngine;
    newDSPUnit->updateDSPMixerCallback = updateMixer;
    
    return newDSPUnit;
}

kwlError kwlPCMBufferLoad(const char* const path, kwlPCMBuffer* buffer)
{
    /** Reset input struct. */
    buffer->numFrames = 0;
    buffer->numChannels = 0;
    buffer->pcmData = NULL;
    
    /** Load audio data into a kwlAudioData struct.*/
    kwlAudioData audioData;
    kwlError result = kwlLoadAudioFile(path, &audioData, 0);
    
    if (result != KWL_NO_ERROR)
    {
        kwlSetError(result);
        return result;
    }
    
    /** Copy all non-internal fields to the target struct.*/
    buffer->numFrames = audioData.numFrames;
    buffer->numChannels = audioData.numChannels;
    buffer->pcmData = (short*)audioData.bytes;
    
    return KWL_NO_ERROR;
}

void kwlPCMBufferFree(kwlPCMBuffer* buffer)
{
    if (buffer->pcmData != NULL)
    {
        KWL_FREE(buffer->pcmData);
    }
    
    buffer->numFrames = 0;
    buffer->numChannels = 0;
    buffer->pcmData = NULL;
}

