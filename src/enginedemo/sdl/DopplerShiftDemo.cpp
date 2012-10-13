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
#include "BitmapFont.h"
#include "DopplerShiftDemo.h"
#include "KowalskiDemoManager.h"
#include "math.h"

#include <SDL/SDL_opengl.h>

DopplerShiftDemo::DopplerShiftDemo() :
    //PositionalAudioDemoBase::PositionalAudioDemoBase(),
    m_rotationRadius(10),
    m_rotationAngle(0),
    m_waveBankHandle(KWL_INVALID_HANDLE)
{
    
}

DopplerShiftDemo::~DopplerShiftDemo()
{
    
}

void DopplerShiftDemo::initialize()
{
    PositionalAudioDemoBase::initialize();
    m_numEmitters = 1;
    m_emitters = new SoundEmitter[m_numEmitters];
    m_emitters[0] = SoundEmitter("dopplerdemo/note_loop", 0, 0, 0);
    for (int i = 0; i < m_numEmitters; i++)
    {
        m_emitters[i].start();
    }
    m_rotationAngle = 2;
    m_camera.setPosition(-8.85f, 0.24f, 9.304f);
    
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("notes.kwb"));
}

void DopplerShiftDemo::updateEmitterPositions(float timeStep)
{
    float rotationSpeed = 1;
    float angleOffset = 2.0f * 3.141592f / m_numEmitters;

    for (int i = 0; i < m_numEmitters; i++)
    {
        float x = m_rotationRadius * sinf(i * angleOffset + m_rotationAngle);
        float z = m_rotationRadius * cosf(i * angleOffset + m_rotationAngle);
        m_emitters[i].setPos(x, 0, z);
    }
    
    float dAngle = timeStep * rotationSpeed;
    m_rotationAngle += dAngle;
}

void DopplerShiftDemo::deinitialize()
{
    //cleans up the emitters.
    PositionalAudioDemoBase::deinitialize();
    kwlWaveBankUnload(m_waveBankHandle);
}    

void DopplerShiftDemo::render3D()
{
    PositionalAudioDemoBase::render3D();
}

void DopplerShiftDemo::render2D()
{
    
}

void DopplerShiftDemo::onKeyDown(SDLKey key)
{
    if (key == SDLK_1)
    {
        m_emitters[0].start();
    }
    if (key == SDLK_2)
    {
        m_emitters[2].start();
    }
    if (key == SDLK_3)
    {
        m_emitters[3].start();
    }
    if (key == SDLK_4)
    {
        m_emitters[4].start();
    }
    if (key == SDLK_5)
    {
        m_emitters[5].start();
    }
    if (key == SDLK_6)
    {
        m_emitters[6].start();
    }
}

const char* DopplerShiftDemo::getName()
{
    return "Positional audio: Doppler shift";
}

void DopplerShiftDemo::update(float timeStep)
{
    updateEmitterPositions(timeStep);
    PositionalAudioDemoBase::update(timeStep);
}
