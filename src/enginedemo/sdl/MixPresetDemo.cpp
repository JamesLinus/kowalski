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

#include "assert.h"
#include "fileutil.h"
#include "BitmapFont.h"
#include "KowalskiDemoManager.h"
#include "MixPresetDemo.h"
#include "SDL_opengl.h"

const char* const MixPresetDemo::m_presetIDs[NUM_PRESETS] = 
{
    "mixpresetdemo/chirps",
    "mixpresetdemo/noise",
    "mixpresetdemo/tones"
};

SDLKey MixPresetDemo::m_triggerKeys[NUM_PRESETS] = 
{
    SDLK_1,
    SDLK_2,
    SDLK_3
};

MixPresetDemo::MixPresetDemo() :
    DemoBase(),
    m_presetHandles(),
    m_doFade(true),
    m_defaultPreset(KWL_INVALID_HANDLE),
    m_noiseEvent(KWL_INVALID_HANDLE),
    m_toneEvent(KWL_INVALID_HANDLE),
    m_chirpEvent(KWL_INVALID_HANDLE),
    m_waveBankHandle(KWL_INVALID_HANDLE),
    m_activePresetIndex(0)
{
    //
}

MixPresetDemo::~MixPresetDemo()
{

}

const char* MixPresetDemo::getName()
{
    return "Mix presets";
}

void MixPresetDemo::update(float timeStep)
{

}

void MixPresetDemo::onKeyDown(SDLKey key)
{
    for (int i = 0; i < NUM_PRESETS; i++)
    {
        if (key == m_triggerKeys[i])
        {
            if (m_doFade)
            {
                kwlMixPresetFadeTo(m_presetHandles[i]);
            }
            else
            {
                kwlMixPresetSet(m_presetHandles[i]);
            }
            m_activePresetIndex = i;
            break;
        }
    }   
    
    if (key == SDLK_c)
    {
        m_doFade = !m_doFade;
    }
    
}

void MixPresetDemo::render2D()
{
    float x = KowalskiDemoManager::VIEWPORT_WIDTH / 2;
    float xOffsets[3] = {-150, 0, 150};
    float y = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    
    for (int i = 0; i < NUM_PRESETS; i++)
    {
        if (i == m_activePresetIndex)
        {
            glColor3f(0, 1, 0);
        }
        else
        {
            glColor3f(1, 0, 0);
        }
        BitmapFont::drawString(x + xOffsets[i], y, m_presetIDs[i], BitmapFont::HCENTER);
    }
    
    glColor3f(0.0f, 0.0f, 0.0f);
    BitmapFont::drawString(x, y - 100, m_doFade ? "crossfade" : "no crossfade", BitmapFont::HCENTER);

}

void MixPresetDemo::initialize()
{
    for (int i = 0; i < NUM_PRESETS; i++)
    {
        m_presetHandles[i] = kwlMixPresetGetHandle(m_presetIDs[i]);
    }
    
    m_doFade = true;
    
    m_defaultPreset = kwlMixPresetGetHandle("default");
    kwlMixPresetSet(m_presetHandles[0]);
    m_activePresetIndex = 0;
    
    m_noiseEvent = kwlEventGetHandle("mixpresetdemo/noiseloop");
    m_toneEvent = kwlEventGetHandle("mixpresetdemo/tone");
    m_chirpEvent = kwlEventGetHandle("mixpresetdemo/chirploop");
    
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("sfx.kwb"));
    
    kwlEventStart(m_noiseEvent);
    kwlEventStart(m_toneEvent);
    kwlEventStart(m_chirpEvent);
}

void MixPresetDemo::deinitialize()
{
    kwlEventStop(m_noiseEvent);
    kwlEventStop(m_toneEvent);
    kwlEventStop(m_chirpEvent);
    
    kwlEventRelease(m_noiseEvent);
    kwlEventRelease(m_toneEvent);
    kwlEventRelease(m_chirpEvent);
    
    kwlWaveBankUnload(m_waveBankHandle);
    
    kwlMixPresetSet(m_defaultPreset);
}

const char* MixPresetDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Select active mix preset: 1, 2, 3"; 
        case 1:
            return "Toggle crossfade: c"; 
    }
    return "";
}
