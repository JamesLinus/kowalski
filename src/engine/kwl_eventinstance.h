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

#ifndef KWL__EVENT_H
#define KWL__EVENT_H

/*! \file */ 

#include "kwl_decoder.h"
#include "kwl_eventdefinition.h"
#include "kwl_synchronization.h"
#include "kwl_sound.h"
#include "kwl_engine.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
/** An enumeration of valid event playback states.*/
typedef enum
{
    /** The event is playing. */
    KWL_PLAYING,
    /** */
    KWL_PLAYING_LAST_BUFFER,
    /** The event is not playing. */
    KWL_STOPPED,
    /** The event is playing and stop has been requested. */
    KWL_STOP_REQUESTED,
    /** 
     * The event is playing and has been requested to stop and then get unloaded 
     * (freeform events only)
     */
    KWL_STOP_AND_UNLOAD_REQUESTED,
    /** 
     * The event has been requested to play the last buffer
     * in its sound's buffer list and then stop. 
     */
    KWL_PLAY_LAST_BUFFER_AND_STOP_REQUESTED,
} kwlEventPlaybackState;
    
/** 
 * An event instance.
 */
typedef struct kwlEventInstance
{
    
    //engine->mixer
    /** The effective left gain value. */
    kwlSharedFloat gainLeft;
    /** The effective right gain value. */
    kwlSharedFloat gainRight;
    /** The effective pitch value. */
    kwlSharedFloat pitch;

    
    
    
    
    
    //mixer->engine
    /** The DSP unit that the output of this event is fed through. Ignored if NULL.*/
    kwlSharedVoidPointer dspUnit;

    
    
    
    
    
    /** The event definition associated with the event.*/
    struct kwlEventDefinition* definition_engine;
    /** The event definition associated with the event.*/
    struct kwlEventDefinition* definition_mixer;
    /** 
     * The decoder providing the event with audio data. Used for streaming events.
     * Is NULL when the event is not playing.
     */
    struct kwlDecoder* decoder;

    /** The x component of the event's position (only used for positional events).*/
    float positionX;
    /** The y component of the event's position (only used for positional events).*/
    float positionY;
    /** The z component of the event's position (only used for positional events).*/
    float positionZ;
    
    /** The x component of the event's velocity (only used for positional events).*/
    float velocityX;
    /** The y component of the event's velocity (only used for positional events).*/
    float velocityY;
    /** The z component of the event's velocity (only used for positional events).*/
    float velocityZ;
    
    /** The x component of the event's direction (only used for positional events).*/
    float directionX;
    /** The y component of the event's direction (only used for positional events).*/
    float directionY;
    /** The z component of the event's direction (only used for positional events).*/
    float directionZ;
    
    /** The user gain.*/
    float userGain;
    /** The user pitch.*/
    float userPitch;
    /** The event balance. (Only used for 2D events)*/
    float balance;
    /** The current pitch contribution from this event's sound (if any) */
    float soundPitch;
    
    /** Pitch accumulator used for linear interpolation pitch shifting*/
    float pitchAccumulator;
    
    /** Non-zero if the event is paused, zero otherwise. Accessed only from the mixer thread.*/
    char isPaused;
    /** Non-zero if this instance is associated with an event handle*/
    char isAssociatedWithHandle;
    /** The current playback state of the event. Accessed only from the mixer thread.*/
    kwlEventPlaybackState playbackState;
    /** Non-zero if the event is currently playing, zero otherwise. Accessed only from the engine thread.*/
    char isPlaying;
        
    /** The buffer that the event is currently getting its audio from.*/
    short* currentPCMBuffer;
    /** */
    char currentNumChannels;
    /** The number of frames in the current audio buffer.*/
    int currentPCMBufferSize;
    /** The read position (ie current frame) in the current audio buffer.*/
    int currentPCMFrameIndex;
    /** Sound based events only.*/
    short currentAudioDataIndex;
    /** */
    int numBuffersPlayed;
    
    /** Used for the linked list of playing events in the buses of the mixer. Only accessed from the mixer thread. */
    struct kwlEventInstance* nextEvent_mixer;
    /** Used for the linked list of playing events in the engine. Only accessed from the engine thread. */
    struct kwlEventInstance* nextEvent_engine;
    /** The current fade gain. Used for fading events in and out.*/
    float fadeGain;
    /** The fade gain increment per frame. Depends on the sample rate and the requested fade time. */
    float fadeGainIncrPerFrame;
    /** Used for per buffer gain ramps.*/
    float prevEffectiveGain[2];
    /** A callback to invoke when the event stops.*/
    kwlEventStoppedCallack stoppedCallback;
    /** A pointer to pass to the event stopped callback.*/
    void* stoppedCallbackUserData;
    
} kwlEventInstance;

/** 
 * 
 */
void kwlEventInstance_init(kwlEventInstance* event);

/**
 *
 */
void kwlEventInstance_start(kwlEventInstance* event);

/** */
kwlError kwlEventInstance_createFreeformEventFromBuffer(kwlEventInstance** event, 
                                                kwlPCMBuffer* buffer, 
                                                kwlEventType type);
/** */
kwlError kwlEventInstance_createFreeformEventFromFile(kwlEventInstance** event, 
                                              const char* const audioFilePath, 
                                              kwlEventType type, 
                                              int streamFromDisk);
    
/** */
kwlError kwlEventInstance_createFreeformEventFromAudioData(kwlEventInstance** event, 
                                                   kwlAudioData* data, 
                                                   kwlEventType type, 
                                                   const char* eventId);
    
/** */
void kwlEventInstance_releaseFreeformEvent(kwlEventInstance* event);
    
/**
 * Returns the number of remaining output frames the current buffer of this event
 * will produce, taking the current effective pitch into account. 
 * For an event with unit pitch, this method just returns the number of 
 * source buffer frames.
 */
int kwlEventInstance_getNumRemainingOutFrames(kwlEventInstance* event, float pitch);    

/** 
 * 
 */
int kwlEventInstance_render(kwlEventInstance* event, 
                    float* outBuffer,
                    const int numOutChannels,
                    const int numFrames,
                    float accumulatedBusPitch);

#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__EVENT_H*/
