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
#include "EmitterSwarmDemo.h"

EmitterSwarmDemo::EmitterSwarmDemo() :
    m_numRowsAndColumns(0),
    m_waveBankHandle(KWL_INVALID_HANDLE)
{

}

EmitterSwarmDemo::~EmitterSwarmDemo()
{

}

const char* EmitterSwarmDemo::getName()
{
    return "Positional audio: One-shot playback";
}

const char* EmitterSwarmDemo::getInstructionLine(int index)
{
    if (index < 1)
    {
        return PositionalAudioDemoBase::getInstructionLine(index);
    }
    switch (index)
    {
        case 1:
            return "Trigger playback in a random location: t"; 
        case 2:
            return "Event instance count is 3"; 
    }
    return "";
}

void EmitterSwarmDemo::onKeyDown(SDLKey key)
{
    switch (key)
    {
        case SDLK_t:
            for (int i = 0; i < m_numEmitters; i++)
            {
                m_emitters[i].setLastTriggered(false);
            }
            
            int idx = rand() % m_numEmitters;
            m_emitters[idx].setLastTriggered(true);
            m_emitters[idx].start();
            break;
    }
}

void EmitterSwarmDemo::update(float timeStep)
{
    PositionalAudioDemoBase::update(timeStep);
}

void EmitterSwarmDemo::render3D()
{
    PositionalAudioDemoBase::render3D();
}

void EmitterSwarmDemo::initialize()
{
    PositionalAudioDemoBase::initialize();
    //create emitters using the same event definition and
    //place them on an N by N grid
    m_numRowsAndColumns = 4;
    m_numEmitters = m_numRowsAndColumns * m_numRowsAndColumns;
    m_emitters = new SoundEmitter[m_numEmitters];
    
    float spacing = 10;
    float min = -0.5f * (m_numRowsAndColumns - 1) * spacing;
    int emitterIdx = 0;
    for (int i = 0; i < m_numRowsAndColumns; i++)
    {
        float x = min + i * spacing;
        for (int j = 0; j < m_numRowsAndColumns; j++)
        {
            float z = min + j * spacing;
            m_emitters[emitterIdx++] = 
                SoundEmitter("oneshotdemo/square_fade_out", x, 0, z, true);
        }
    }
    
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("sfx.kwb"));
}

void EmitterSwarmDemo::deinitialize()
{
    PositionalAudioDemoBase::deinitialize();
    kwlWaveBankUnload(m_waveBankHandle);
}
