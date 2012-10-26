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

#ifndef KWL_EVENT_DEFINITION_H
#define KWL_EVENT_DEFINITION_H

/*! \file */ 

#include "kwl_decoder.h"
#include "kwl_sounddefinition.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
/** 
 * An enumeration of event retrigger behaviours
 */
typedef enum
{
    /** */
    KWL_RETRIGGER = 0,
    /** */
    KWL_DONT_RETRIGGER
} kwlEventRetriggerMode;

/** 
 * An enumeration of event instance stealing behaviours. Used for one-shot playback.
 */
typedef enum
{
    /** */
    KWL_STEAL_QUIETEST = 0,
    /** */
    KWL_STEAL_RANDOM,
    /** */
    KWL_DONT_STEAL
} kwlEventInstanceStealingMode;

/**
 * An event definition describes the behavior of the event instances associated with it.
 * Any variable state information, like user gain, position etc is stored in the event instances. 
 */
typedef struct kwlEventDefinition
{
    /** The id of the event definition */
    char* id;
    /** The number of event instances created for this definition.*/
    int instanceCount;
    /** Non-zero if this is a positional event, zero otherwise.*/
    int isPositional;
    /** */
    int loopIfStreaming;
    /** The gain associated with the event definition. */
    float gain;
    /** The pitch associated with the event definition. */
    float pitch;
    /** The cosine of the inner cone angle.*/
    float innerConeCosAngle;
    /** The cosine of the outer cone angle.*/
    float outerConeCosAngle;
    /** The gain to apply for listener-event angles greater than the outer cone angle.*/
    float outerConeGain;
    /** */
    kwlEventRetriggerMode retriggerMode;
    /** */
    kwlEventInstanceStealingMode stealingMode;
    /** The encoded audio data for streaming events, NULL for non-streaming events.*/
    kwlAudioData* streamAudioData;
    /** The mix bus this event is fed through. */
    struct kwlMixBus* mixBus;
    /** The sound determining the playback behaviour of the event. Used for non-streaming PCM events only.*/
    struct kwlSoundDefinition* sound;
    /** The number of wave banks indirectly referenced by this event definition. Accessed from the mixer thread.*/
    int numReferencedWaveBanks;
    /** 
     * The wave banks indirectly referenced by this event definition. Used to determine if an event should 
     * stop before unloading a given wavebank. Accessed from the mixer thread.
     */
    kwlWaveBank** referencedWaveBanks;
} kwlEventDefinition;

void kwlEventDefinition_init(kwlEventDefinition* eventDefinition);
#ifdef __cplusplus
}
#endif /* __cplusplus */    

#endif /*KWL_EVENT_DEFINITION_H*/
