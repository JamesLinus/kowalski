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

#include "AudioTaperDemo.h"
#include "BitmapFont.h"
#include "KowalskiDemoManager.h"
#include "fileutil.h"
#include "assert.h"
#include "math.h"
#include <SDL/SDL_opengl.h>

AudioTaperDemo::AudioTaperDemo() : 
    m_eventHandle(KWL_INVALID_HANDLE),
    m_waveBankHandle(KWL_INVALID_HANDLE),
    m_currentGain(0),
    m_useLinearGain(false),
    m_gainBar(0, 1, false)
{
}

AudioTaperDemo::~AudioTaperDemo()
{
}

const char* AudioTaperDemo::getName()
{
    return "Audio taper";
}

const char* AudioTaperDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Toggle between linear and logarithmic taper: t"; 
        case 1:
            return "Change gain: up/down arrow keys"; 
    }
    return "";
    
}

void AudioTaperDemo::update(float timeStep)
{
    float delta = timeStep * 0.6f;
    Uint8* kstates = SDL_GetKeyState(NULL);
    
    if (kstates[SDLK_UP])
    {
        m_currentGain += delta;
    }
    else if (kstates[SDLK_DOWN])
    {
        m_currentGain -= delta;
    }
    
    if (m_currentGain > 1.0f)
    {
        m_currentGain = 1.0f;
    }
    else if (m_currentGain < 0.0f)
    {
        m_currentGain = 0.0f;
    }
    
    if (m_useLinearGain)
    {
        kwlEventSetLinearGain(m_eventHandle, m_currentGain);
    }
    else
    {
        kwlEventSetGain(m_eventHandle, m_currentGain);
    }
    
    if (!kwlEventIsPlaying(m_eventHandle))
    {
        kwlEventStart(m_eventHandle);
    }
    
}

void AudioTaperDemo::onKeyDown(SDLKey key)
{
    if (key == SDLK_t)
    {
        m_useLinearGain = !m_useLinearGain;
    }
}

void AudioTaperDemo::render2D()
{
    float x = KowalskiDemoManager::VIEWPORT_WIDTH / 2;
    float y = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    float w = 30;
    float h = 400;

    m_gainBar.setValue(m_currentGain);
    m_gainBar.render(x - w / 2, y - h / 2, w, h);
    
    
    glColor3f(0, 0, 0);
    BitmapFont::drawString(x, y + h / 2 + 10, 
                           m_useLinearGain ? "Linear gain" : "Logarithmic gain", BitmapFont::HCENTER);
}

void AudioTaperDemo::initialize()
{
    m_useLinearGain = false;
    m_currentGain = 0.9f;
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("music.kwb"));
    m_eventHandle = kwlEventGetHandle("music/oneone");
    kwlEventStart(m_eventHandle);
}

void AudioTaperDemo::deinitialize()
{
    kwlEventStopFade(m_eventHandle, 4.0f);
    kwlEventRelease(m_eventHandle);
    m_eventHandle = KWL_INVALID_HANDLE;
    kwlWaveBankUnload(m_waveBankHandle);
    m_waveBankHandle = KWL_INVALID_HANDLE;
}

