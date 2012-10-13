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

#include "BitmapFont.h"
#include "ConeAttenuationDemo.h"
#include <SDL/SDL_opengl.h>
#include "fileutil.h"

ConeAttenuationDemo::ConeAttenuationDemo() :
    PositionalAudioDemoBase(),
    m_waveBankHandle(KWL_INVALID_HANDLE),
    m_paused(false),
    m_eventConeEnabled(true),
    m_listenerConeEnabled(true)
{
    
}

ConeAttenuationDemo::~ConeAttenuationDemo()
{
    
}

void ConeAttenuationDemo::initialize()
{
    PositionalAudioDemoBase::initialize();
    m_eventConeEnabled = true;
    m_listenerConeEnabled = true;
    kwlSetConeAttenuationEnabled(m_listenerConeEnabled ? 1 : 0, m_eventConeEnabled ? 1 : 0);
    kwlListenerSetConeParameters(90, 270, 0.1f);
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("sfx.kwb"));
    
    m_numEmitters = 1;
    m_emitters = new SoundEmitter[m_numEmitters];
    m_emitters[0] = SoundEmitter("coneattenuationdemo/tone", 0, 4, 0);
    m_emitters[0].setAngularVelocity(0.6f);
    m_emitters[0].setDrawCone(true);
    m_emitters[0].start();
    m_camera.setPosition(0, 4, 5);
}

void ConeAttenuationDemo::deinitialize()
{
    PositionalAudioDemoBase::deinitialize();
    kwlWaveBankUnload(m_waveBankHandle);
    kwlSetConeAttenuationEnabled(1, 1);
    kwlListenerSetConeParameters(0, 360, 1);
    m_waveBankHandle = KWL_INVALID_HANDLE;
}    

void ConeAttenuationDemo::render3D()
{
    PositionalAudioDemoBase::render3D();
}

void ConeAttenuationDemo::render2D()
{
    glColor3f(0.0f, 0.0f, 0.0f);
    BitmapFont::drawString(10, 20, m_eventConeEnabled ? "event cone on" : "event cone off");
    BitmapFont::drawString(10, 30, m_listenerConeEnabled ? "listener cone on" : "listener cone off");
}

const char* ConeAttenuationDemo::getInstructionLine(int index)
{
    if (index < 1)
    {
        return PositionalAudioDemoBase::getInstructionLine(index);
    }
    switch (index)
    {
        case 1:
            return "Toggle rotation: r"; 
        case 2:
            return "Toggle event/listener cone attenuation: e/l"; 
    }
    return "";
}

void ConeAttenuationDemo::onKeyDown(SDLKey key)
{
    if (key == SDLK_r)
    {
        m_paused = !m_paused;
        m_emitters[0].setAngularVelocity(m_paused ? 0.0f : 0.6f);
    }
    if (key == SDLK_e)
    {
        m_eventConeEnabled = !m_eventConeEnabled;
    }
    if (key == SDLK_l)
    {
        m_listenerConeEnabled = !m_listenerConeEnabled;
    }
    
    kwlSetConeAttenuationEnabled(m_listenerConeEnabled ? 1 : 0, m_eventConeEnabled ? 1 : 0);
}

const char* ConeAttenuationDemo::getName()
{
    return "Positional audio: Cone attenuation";
}

void ConeAttenuationDemo::update(float timeStep)
{
    PositionalAudioDemoBase::update(timeStep);
}
