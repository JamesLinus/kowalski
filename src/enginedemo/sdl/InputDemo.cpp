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

#include "fileutil.h"
#include "assert.h"
#include "math.h"
#include "BitmapFont.h"
#include "InputDemo.h"
#include "kwl_asm.h"
#include "KowalskiDemoManager.h"

#include "SDL_opengl.h"

void processInputBuffer(float* buffer, int numChannels, 
                        int numFrames, void* data)
{
    InputDSPUnitData* d = (InputDSPUnitData*)data;
    
    float maxAbs = 0.0f;
    for (int ch = 0; ch < numChannels; ch++)
    {
        int idx = ch;
        for (int i = 0; i < numFrames; i++)
        {
            float abs = buffer[idx] > 0.0f ? buffer[idx] : -buffer[idx];
            if (abs > maxAbs)
            {
                maxAbs = abs;
            }
            idx += numChannels;
        }
    }
    
    d->m_currentLevelMixer = maxAbs;
}

void updateParamsEngine(void* data)
{
    InputDSPUnitData* d = (InputDSPUnitData*)data;
    d->m_currentLevelEngine = d->m_currentLevelShared;
}

void updateParamsMixer(void* data)
{
    InputDSPUnitData* d = (InputDSPUnitData*)data;    
    d->m_currentLevelShared = d->m_currentLevelMixer;
}

InputDemo::InputDemo() :
    DemoBase(),
    m_levelBar(0, 1, true),
    m_dspUnitData()
{
    //
}

InputDemo::~InputDemo()
{
    
}

const char* InputDemo::getName()
{
    return "Audio Input";
}

void InputDemo::update(float timeStep)
{

}

void InputDemo::onKeyDown(SDLKey key)
{
    
}

void InputDemo::render2D()
{
    const float width = 40;
    const float height = 400;
    const float y = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    const float x = (KowalskiDemoManager::VIEWPORT_WIDTH - width) / 2;
    
    m_levelBar.setValue(m_dspUnitData.m_currentLevelEngine);
    
    m_levelBar.render(x, y - height / 2, width, height);
}

void InputDemo::initialize()
{
    m_dspUnitData.m_currentLevelEngine = 0.0f;
    m_dspUnitData.m_currentLevelShared = 0.0f;
    m_dspUnitData.m_currentLevelMixer = 0.0f;
    
    //set up the dsp unit;
    m_dspUnit = kwlDSPUnitCreateCustom(&m_dspUnitData, 
                                       processInputBuffer, 
                                       updateParamsEngine, 
                                       updateParamsMixer, 
                                       NULL);
    kwlDSPUnitAttachToInput(m_dspUnit);
}

void InputDemo::deinitialize()
{    
    //remove dsp unit 
    kwlDSPUnitAttachToInput(NULL);
}

const char* InputDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "A custom DSP unit measures the input audio peak level."; 
            
    }
    return "";
}
