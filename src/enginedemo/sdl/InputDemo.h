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

#ifndef KOWALSKI_INPUT_DEMO_H
#define KOWALSKI_INPUT_DEMO_H

#include "DemoBase.h"
#include "MeterBar.h"
#include "kowalski.h"
#include "kowalski.h"

/**
 * This method gets called when it's time to process 
 * another audio buffer. Gets called from the mixer 
 * thread and should ONLY access variables exclusive 
 * to the mixer thread.
 */
void processInputBuffer(float* buffer, int numChannels, int numFrames, void* dataMixer);

/** 
 * Called from the engine thread to update the state 
 * of the DSP unit.
 */
void updateParamsEngine(void* data);

/** 
 * Called from the mixer thread to update the state 
 * of the DSP unit.
 */
void updateParamsMixer(void* data);

typedef struct InputDSPUnitData
{
    float m_currentLevelEngine;
    float m_currentLevelShared;
    float m_currentLevelMixer;
} InputDSPUnitData;

class InputDemo : public DemoBase
{
public:
    InputDemo();
    ~InputDemo();
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Demonstrates audio input processing.";}
    virtual const char* getInstructionLine(int index);
private:
    kwlDSPUnitHandle m_dspUnit;
    kwlEventHandle m_eventHandle;
    InputDSPUnitData m_dspUnitData;
    MeterBar m_levelBar;
};

#endif //KOWALSKI_INPUT_DEMO_H
