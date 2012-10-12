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

#ifndef KWL__DSP_UNIT_H
#define KWL__DSP_UNIT_H

/*! \file */ 

#include "kowalski_ext.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * An enumeration of valid DSP unit types.
 */
typedef enum kwlDSPUnitType
{
    /** A user defined DSP unit.*/
    KWL_CUSTOM_DSP_UNIT = 0
} kwlDSPUnitType;

/**
 * A DSP unit that can be attached to a point in the signal chain.
 */
typedef struct kwlDSPUnit
{
    /** The DSP unit type.*/
    kwlDSPUnitType type;
    /**
     * User data associated with the DSP unit. Gets passed to the three callbacks 
     * below.
     */
    void *data;
    /** 
     * A pointer to a function responsible for processing audio buffers. 
     */
    kwlDSPCallback dspCallback;
    /**
     * This callback gets invoked from the mixer thread and is where any DSP unit 
     * parameter updates should be performed. The callback is invoked from within a critical section,
     * so it is safe to access any variables shared between the engine and mixer threads.
     * Perform as little work as possible in this callback. 
     */
    kwlDSPUpdateCallback updateDSPMixerCallback;
    /**
     * This callback gets invoked from the engine thread and is where any DSP unit 
     * parameter updates should be performed. The callback is invoked from within a critical section,
     * so it is safe to access any variables shared between the engine and mixer threads.
     * Perform as little work as possible in this callback. 
     */        
    kwlDSPUpdateCallback updateDSPEngineCallback;
    
} kwlDSPUnit;
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */
        
#endif /*KWL__DSP_UNIT_H*/
