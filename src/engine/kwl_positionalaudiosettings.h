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

#ifndef KWL_POSITIONAL_AUDIO_SETTINGS_H
#define KWL_POSITIONAL_AUDIO_SETTINGS_H

/*! \file */ 

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "kowalski.h"
    
/** A collection of positional audio related parameters. */
typedef struct kwlPositionalAudioSettings
{
    /** A scaling factor applied to the doppler pitch shift. A value of 0 disables doppler shift altogether.*/
    float dopplerScale;
    /** The speed of sound in m/s. */
    float speedOfSound;
    /** The reference distance. Model specific interpretation */
    float referenceDistance;
    /** The rolloff factor. Model specific interpretation */
    float rolloffFactor;
    /** Events further away than this from the listener will have a gain of 0*/
    float maxDistance;
    /** The model used to compute distance based attenuation. */
    kwlDistanceAttenuationModel distanceModel;
    /** If this is non-zero, distance models never generate gain values above 1. */
    int clamp;
    /** 
     * Indicates if listener cone attenuation is enabled, i.e if the gain of positional events
     * is modulated by TODO
     */
    int isListenerConeAttenuationEnabled;
    /** 
     * Indicates if event cone attenuation is enabled, i.e if the gain of positional events is modulated
     * by its orientation relative to the listener.
     */
    int isEventConeAttenuationEnabled;
} kwlPositionalAudioSettings;

void kwlPositionalAudioSettings_setDefaults(kwlPositionalAudioSettings* settings);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    

#endif /*KWL_POSITIONAL_AUDIO_SETTINGS_H*/
