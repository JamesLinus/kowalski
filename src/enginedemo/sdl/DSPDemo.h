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

#ifndef KOWALSKI_DSP_DEMO_H
#define KOWALSKI_DSP_DEMO_H

#include "DemoBase.h"
#include "MeterBar.h"
#include "kowalski.h"
#include "kowalski.h"

/** 
 * A struct defining the state of a simple DSP unit 
 * that applies a gain factor to an event and 
 * measures the resulting output RMS level.
 */
struct DSPUnitData
{
    /** 
     * The gain value set by the user. Only accessed from
     * the engine thread (i.e the application thread). 
     */
    float gainEngine;
    
    /** 
     * The gain value applied to the samples. Only accessed
     * from the mixer thread. 
     */
    float gainMixer;
    
    /** 
     * Variable for temporarily storing the gain set in the 
     * engine thread. NOTE: This variable is shared between 
     * the engine and mixer threads and should ONLY be 
     * accessed in the parameter update callbacks.
     */
    float gainShared;
    
    /** 
     * The output level. Only accessed from the engine thread 
     * (i.e the application thread). 
     */
    float rmsLevelEngine;
    
    /** 
     * The output level computed in the processing callback. 
     * Only accessed from the mixer thread. 
     */
    float rmsLevelMixer;
    
    /** 
     * Variable for temporarily storing the level computed 
     * in the mixer thread. NOTE: This variable is shared 
     * between the engine and mixer threads and should ONLY 
     * be accessed in the parameter update callbacks.
     */
    float rmsLevelShared;
};

/**
 * This method gets called when it's time to process 
 * another audio buffer. Gets called from the mixer 
 * thread and should ONLY access variables exclusive 
 * to the mixer thread.
 */
void processBuffer(float* buffer, int numChannels, int numFrames, void* dataMixer);

/** 
 * Called from the engine thread to update the state 
 * of the DSP unit.
 */
void updateParametersEngine(void* data);

/** 
 * Called from the mixer thread to update the state 
 * of the DSP unit.
 */
void updateParametersMixer(void* data);

class DSPDemo : public DemoBase
{
public:
    DSPDemo();
    ~DSPDemo();
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Demonstrates custom audio processing using DSP units.";}
    virtual const char* getInstructionLine(int index);
private:
    DSPUnitData m_dspUnitData;
    kwlWaveBankHandle m_waveBankHandle;
    kwlEventHandle m_eventHandle;
    kwlDSPUnitHandle m_dspUnit;
    MeterBar m_levelBar;
    MeterBar m_gainBar;
};

#endif //KOWALSKI_DSP_DEMO_H
