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
#ifndef KWL__MIX_PRESET_H
#define KWL__MIX_PRESET_H

/*! \file */ 

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** A set of mix bus parameters completely defining the state of a single mixbus. */
typedef struct kwlMixBusParameters
{
    /** 
     * The index into the mix bus array 
     * of the mix bus associated with this parameter set
     */
    int mixBusIndex;
    /** The logarithmic gain of the left channel.*/
    float logGainLeft;
    /** The logarithmic gain of the right channel.*/
    float logGainRight;
    /** The pitch.*/
    float pitch;
} kwlMixBusParameters;

/** A collection of mix bus parameter sets, completely defining the state of the entire mix bus hierarchy. */
typedef struct kwlMixPreset
{
    /** The ID of the mix preset*/
    char* id; 
    /** The number of parameter sets in this preset. Should equal the number of mix buses.*/
    int numParameterSets;
    /** An array of parameter sets, one for each mix bus.*/
    kwlMixBusParameters* parameterSets;
    /** The target blending weight of the preset. 0 - 1*/
    float targetWeight;
    /** The current blending weight of the preset. 0 - 1*/
    float weight;
} kwlMixPreset;

#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__MIX_PRESET_H*/
