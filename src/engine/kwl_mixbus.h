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
#ifndef KWL_MIX_BUS_H
#define KWL_MIX_BUS_H

/*! \file */ 

#include "kwl_synchronization.h"
#include "kowalski.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
  
struct kwlEvent;
    
/** 
 * A node in a mix bus tree.
 */
typedef struct kwlMixBus
{
    //engine->mixer
    /** The total left channel gain, taking the parent buses into account*/
    kwlSharedFloat totalGainLeft;
    /** The total right channel gain, taking the parent buses into account*/
    kwlSharedFloat totalGainRight;
    /** The total pitch, taking the parent buses into account*/
    kwlSharedFloat totalPitch;
    /** The DSP unit, if any, that the output of this bus is fed through.*/
    kwlSharedVoidPointer dspUnit;
    
    
    
    
    
    /** The unique ID of this mix bus. */
    char* id;
    /** Non-zero if this is the master bus, zero otherwise.*/
    char isMaster;
    
    /** The number of sub buses under this mix bus.*/
    int numSubBuses;
    /** The sub buses of this bus */
    struct kwlMixBus** subBuses;
    /** A linked list of currently playing events in this bus. */
    struct kwlEventInstance* eventList;
    
    /** The left channel user gain */
    float userGainLeft;
    /** The right channel user gain */
    float userGainRight;
    /** The user pitch */
    float userPitch;
    
    /** The left channel computed by blending contributions from mix presets. */
    float mixPresetGainLeft;
    /** The right channel computed by blending contributions from mix presets.*/
    float mixPresetGainRight;
    /** The pitch computed by blending contributions from mix presets. */
    float mixPresetPitch;
    
} kwlMixBus;

/** */
void kwlMixBus_init(kwlMixBus* mixBus);

/** Adds an event to a mix bus. */
void kwlMixBus_addEvent(kwlMixBus* bus, struct kwlEventInstance* event);

/** Removes an event from a mix bus. */
void kwlMixBus_removeEvent(kwlMixBus* bus, struct kwlEventInstance* event);

void kwlMixBus_render(kwlMixBus* mixBus, 
                      void* mixer, //TODO: made this a void* to get things to compile. should be kwlMixer*
                      int numOutChannels,
                      int numFrames, 
                      float* busScratchBuffer,
                      float* eventScratchBuffer,
                      float* outBuffer,
                      float accumulatedPitch,
                      float accumulatedGainLeft,
                      float accumulatedGainRight);
    
#ifdef KOWALSKI_DEBUG_LOADING
void kwlMixBus_print(kwlMixBus* bus, int recursionDepth);
#endif /*KOWALSKI_DEBUG_LOADING*/

#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL_MIX_BUS_H*/
