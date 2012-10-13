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
#ifndef KWL__SOUND_ENGINE_H
#define KWL__SOUND_ENGINE_H

/*! \file */ 

#include "kowalski.h"
#include "kwl_audiodata.h"
#include "kwl_enginedata.h"
#include "kwl_dspunit.h"
#include "kwl_eventinstance.h"
#include "kwl_inputstream.h"
#include "kwl_synchronization.h"
#include "kwl_mixpreset.h"
#include "kwl_positionalaudiolistener.h"
#include "kwl_positionalaudiosettings.h"
#include "kwl_mixer.h"
#include "kwl_sound.h"
#include "kwl_wavebank.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
/***********************************************************************
 * Sound engine struct.
 ***********************************************************************/
/** A struct representing the singleton Kowalski sound engine. */
typedef struct kwlEngine
{
    /** The software mixer responsible for generating the final output buffers. */
    kwlMixer* mixer;
    
    /** A message queue that incoming messages from the mixer thread get copied to and then processed. */
    kwlMessageQueue fromMixerQueue;
    /** A message queue for outgoing messages to the mixer thread. */
    kwlMessageQueue toMixerQueue;
    /** A message queue that outgoing messages get copied to and then grabbed from the mixer thread. 
     * NOTE: this queue is accessed from both the engine thread and the mixer thread so the main lock
     * must be acquired prior to any manipulation to protect the data.
     */
    kwlMessageQueue toMixerQueueShared;
    /** 
     * A mutex lock used to protect data shared between the engine and mixer threads, 
     * like message queues.
     */
    kwlMutexLock mixerEngineMutexLock;
    
    /**
     * A mutex lock protecting the queue used for wavebank loading messages.
     */
    kwlMutexLock wavebankLoadingMutexLock;
    kwlMessageQueue wavebankLoadingQueueShared;
    kwlMessageQueue wavebankLoadingQueue;
    
    /** A struct containing information about the current 3D audio listener. */
    kwlPositionalAudioListener listener;
    
    /** The current size of the array of freeform events, i.e events created in code.*/
    int freeformEventArraySize;
    /** An array of freeform events, i.e events created in code. This array is dynamically resized and may contain null entries. */
    struct kwlEventInstance** freeformEvents;
    
    int isInputEnabled;
    
    long long lastNumFramesMixed;
    /** */
    int numDecoders;
    /** */
    struct kwlDecoder* decoders;
    
    /** 
     * A linked list of currently playing events, ie events for which a 'start event' message has been sent and
     * an 'event stopped' message has not yet been received. 
     */
    struct kwlEventInstance* playingEventList;
    
    /** A collection of positional audio parameters.*/
    kwlPositionalAudioSettings positionalAudioSettings;
    
    /** The currently loaded engine data.*/
    kwlEngineData engineData;

} kwlEngine; 
    
/** Initializes a newly allocated sound engine instance. */
void kwlEngine_init(kwlEngine* engine);
    
/** Deletes a given sound engine instance. */
void kwlEngine_free(kwlEngine* engine);

/***********************************************************************
 * Event methods
 ***********************************************************************/
/** */
kwlError kwlEngine_eventGetHandle(kwlEngine* engine, const char* const eventId, int* handle);

/** */
kwlError kwlEngine_eventDefinitionGetHandle(kwlEngine* engine, 
                                                 const char* const eventDefinitionID, 
                                                 kwlEventDefinitionHandle* handle);

/** */
kwlError kwlEngine_eventCreateWithBuffer(kwlEngine* engine, kwlPCMBuffer* buffer, 
                                              kwlEventHandle* handle, kwlEventType type);
    
/** */
kwlError kwlEngine_eventCreateWithFile(kwlEngine* engine, const char* const audioFilePath, 
                                            kwlEventHandle* handle, kwlEventType type, int streamFromDisk);
    
/** */
kwlError kwlEngine_eventRelease(kwlEngine* engine, kwlEventHandle handle);

/** */
kwlError kwlEngine_unloadFreeformEvent(kwlEngine* engine, struct kwlEventInstance* event);

/** */
kwlError kwlEngine_eventStart(kwlEngine* engine, const int handle, float fadeInTimeSec);
    
/** */
kwlError kwlEngine_startEventInstance(kwlEngine* engine, struct kwlEventInstance* event, float fadeInTimeSec);
    
/** */
kwlError kwlEngine_eventStop(kwlEngine* engine, const int handle, float fadeOutTimeSec);

/** */
kwlError kwlEngine_eventStartOneShot(kwlEngine* engine, 
                                          kwlEventDefinitionHandle handle, 
                                          float x, float y, float z, 
                                          int startAtPosition,
                                          kwlEventStoppedCallack stoppedCallback,
                                          void* stoppedCallbackUserData);

kwlError kwlEngine_eventSetStoppedCallback(kwlEngine* engine, const int handle, 
                                                kwlEventStoppedCallack stoppedCallback,
                                                void* stoppedCallbackUserData);
    
/** */
kwlError kwlEngine_eventPause(kwlEngine* engine, const int handle);
    
/** */
kwlError kwlEngine_eventResume(kwlEngine* engine, const int handle);    
    
/** */
kwlError kwlEngine_eventIsPlaying(kwlEngine* engine, const int handle, int* isPlaying);
    
/** */
kwlError kwlEngine_eventSetPitch(kwlEngine* engine, kwlEventHandle event, float pitchPercent);
    
/** */
kwlError kwlEngine_eventSetPosition(kwlEngine* engine, kwlEventHandle handle, float posX, float posY, float posZ);
    
/** */
kwlError kwlEngine_eventSetOrientation(kwlEngine* engine, kwlEventHandle handle, float directionX, float directionY, float directionZ);
    
/** */
kwlError kwlEngine_eventSetVelocity(kwlEngine* engine, kwlEventHandle handle, float velX, float velY, float velZ);
    
/** */
kwlError kwlEngine_eventSetBalance(kwlEngine* engine, kwlEventHandle handle, float balance);
    
/** */
kwlError kwlEngine_eventSetGain(kwlEngine* engine, kwlEventHandle eventHandle, float gain, int isLinearGain);
    
/** Adds a given event to the linked list of currently playing events. */
void kwlEngine_addEventToPlayingList(kwlEngine* engine, struct kwlEventInstance* eventToAdd);
    
/** Removes a given event from the linked list of currently playing events. */
void kwlEngine_removeEventFromPlayingList(kwlEngine* engine, struct kwlEventInstance* eventToRemove);
    
/** Returns the event corresponding to a given handle or NULL if the handle is invalid.*/
struct kwlEventInstance* kwlEngine_getEventFromHandle(kwlEngine* engine, kwlEventHandle handle);
    
/** Returns non-zero if the given handle corresponds to a freeform event, zero otherwise.*/
int kwlEngine_isFreeformEventHandle(kwlEngine* engine, kwlEventHandle handle);   

/** */
int kwlEngine_getFreeformIndexFromHandle(kwlEngine* engine, kwlEventHandle handle);
    
/** */
void kwlEngine_updateEvents(kwlEngine* engine);

/** */
float kwlEngine_getConeGain(kwlEngine* engine, float cosAngle, float cosInner, float cosOuter, float outerGain);    

/***********************************************************************
 * DSP units
 ***********************************************************************/    
kwlError kwlEngine_attachDSPUnitToEvent(kwlEngine* engine, kwlEventHandle eventHandle, kwlDSPUnit* dspUnit);
kwlError kwlEngine_attachDSPUnitToMixBus(kwlEngine* engine, kwlMixBusHandle mixBusHandle, kwlDSPUnit* dspUnit);
kwlError kwlEngine_attachDSPUnitToInput(kwlEngine* engine, kwlDSPUnit* dspUnit);
kwlError kwlEngine_attachDSPUnitToOutput(kwlEngine* engine, kwlDSPUnit* dspUnit);
/***********************************************************************
 * Mix buses/presets
 ***********************************************************************/    
/** */
kwlError kwlEngine_mixBusGetHandle(kwlEngine* engine, const char* const busId, kwlMixBusHandle* handle);

/** Returns the mix bus corresponding to a given handle or NULL if the handle is invalid.*/
struct kwlMixBus* kwlEngine_getMixBusFromHandle(kwlEngine* engine, kwlMixBusHandle handle);
    
/** */
kwlError kwlEngine_mixBusSetGain(kwlEngine* engine, kwlMixBusHandle handle, float gain, int isLinearGain);
    
/** */
kwlError kwlEngine_mixBusSetPitch(kwlEngine* engine, kwlMixBusHandle handle, float pitch);

/** */
kwlError kwlEngine_mixPresetGetHandle(kwlEngine* engine, const char* const busId, kwlMixBusHandle* handle);
    
/** */
kwlError kwlEngine_mixPresetSetActive(kwlEngine* engine, kwlMixPresetHandle handle, int doFade);
    
/** */
void kwlEngine_updateMixPresets(kwlEngine* engine, float timeStepSec);

/***********************************************************************
 * Positional audio
 ***********************************************************************/
/** */
kwlError kwlEngine_setListenerPosition(kwlEngine* engine, float posX, float posY, float posZ);
    
/** */
kwlError kwlEngine_setListenerVelocity(kwlEngine* engine, float velX, float velY, float velZ);
    
/** */
kwlError kwlEngine_setListenerOrientation(kwlEngine* engine, float dirX, float dirY, float dirZ,
                                               float upX, float upY, float upZ);

/** */
kwlError kwlEngine_setDistanceAttenuationModel(kwlEngine* engine,
                                                    kwlDistanceAttenuationModel type, 
                                                    int clamp,
                                                    float maxDistance,
                                                    float rolloffFactor,
                                                    float referenceDistance);

/** */
kwlError kwlEngine_setDopplerShiftParameters(kwlEngine* engine, float speedOfSound, float dopplerScale);

/** */
kwlError kwlEngine_setConeAttenuationEnabled(kwlEngine* engine, int listenerCone, int eventCones);
    
/** */
kwlError kwlEngine_setListenerConeParameters(kwlEngine* engine, 
                                                  float innerAngle, 
                                                  float outerAngle, 
                                                  float outerGain);
    
/** Returns the positional audio distance gain using the currently set distance attenuation model.*/
float kwlEngine_getDistanceGain(kwlEngine* engine, float distanceInv);
/***********************************************************************
 * Basic engine functionality
 ***********************************************************************/
/** */
kwlError kwlEngine_resume(kwlEngine* engine);
    
/** */
kwlError kwlEngine_pause(kwlEngine* engine);
    
/** */
kwlError kwlEngine_update(kwlEngine* engine, float timeStep);
    
/** */
kwlError kwlEngine_initialize(kwlEngine* engine, int sampleRate, int numOutChannels, int numInChannels, int bufferSize);
    
/** */
kwlError kwlEngine_isLoaded(kwlEngine* engine, int* ret);
    
/** */
kwlError kwlEngine_engineDataLoad(kwlEngine* engine, const char* const dataFile);
    
/** */
kwlError kwlEngine_engineDataUnload(kwlEngine* engine);

/** */
kwlError kwlEngine_unloadEngineDataBlocking(kwlEngine* engine);
    
/** Releases all loaded audio and engine data and shuts down the underlying sound system. */
void kwlEngine_deinitialize(kwlEngine* engine);

/***********************************************************************
 * Data loading/unloading methods
 ***********************************************************************/
/** Loads engine data (ie non-audio data) from a given stream. */
kwlError kwlEngine_loadEngineData(kwlEngine* engine, kwlInputStream* stream);
    
/** Loads the audio data entries in Kowalski wave bank binary file. */
kwlError kwlEngine_loadWaveBank(kwlEngine* engine, 
                                     const char* const waveBankFile, 
                                     kwlWaveBankHandle* handle,
                                     int threaded);

/** */
kwlError kwlEngine_waveBankIsLoaded(kwlEngine* engine, kwlWaveBankHandle handle, int* isLoaded);

/** */
kwlError kwlEngine_waveBankIsReferencedByPlayingEvent(kwlEngine* engine, kwlWaveBankHandle handle, int* isReferenced);

/** */
kwlError kwlEngine_requestUnloadWaveBank(kwlEngine* engine, kwlWaveBankHandle waveBankHandle, int blockUntilUnloaded);

/** */
kwlWaveBankHandle kwlEngine_getHandleFromWaveBank(kwlEngine* engine, kwlWaveBank* waveBank);

/** */
kwlError kwlEngine_getNumFramesMixed(kwlEngine* engine, unsigned int* numFrames);
    
/** */
kwlError kwlEngine_getOutLevels(kwlEngine* engine, float* leftLevel, float* rightLevel);
    
/** */
kwlError kwlEngine_hasClipped(kwlEngine* engine, int* hasClipped);
    
/***********************************************************************
 * Engine methods to be implemented per target host.
 ***********************************************************************/
/** 
 * Carries out host specific initialization of the underlying sound system 
 * and hooks it up to the Kowalski engine callbacks.
 */
kwlError kwlEngine_hostSpecificInitialize(kwlEngine* engine, int sampleRate, int numOutChannels, int numInChannels, int bufferSize);

/** 
 * Performs host specific deinitialization of the underlying sound system.
 */
kwlError kwlEngine_hostSpecificDeinitialize(kwlEngine* engine);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__SOUND_ENGINE_H*/
