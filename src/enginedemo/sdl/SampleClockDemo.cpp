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
#include "SampleClockDemo.h"
#include "SDL_opengl.h"
#include "kowalski.h"

SampleClockDemo::SampleClockDemo() :
    m_bpm(120),
    m_currentBeatIndex(0),
    m_frameCount(0),
    m_mixerPaused(false),
    m_eventHandle(KWL_INVALID_HANDLE),
    m_waveBankHandle(KWL_INVALID_HANDLE)
{

}

SampleClockDemo::~SampleClockDemo()
{

}

const char* SampleClockDemo::getName()
{
    return "Sample clock";
}

const char* SampleClockDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Pause/resume mixer: P";
    }
    return "";
}

void SampleClockDemo::update(float timeStep)
{
    m_frameCount += kwlGetNumFramesMixed();
    //printf("m_frameCount %d", m_frameCount);
    while (m_frameCount > 44100 / 2)
    {
        m_frameCount -= 44100 / 2;
        m_currentBeatIndex = (m_currentBeatIndex + 1) % 4;
    }
}

void SampleClockDemo::onKeyDown(SDLKey key)
{
    if (key == SDLK_p)
    {
        if (m_mixerPaused)
        {   
            kwlMixerResume();
        }
        else
        {
            kwlMixerPause();
        }
        m_mixerPaused = !m_mixerPaused;
    }
}
        
void SampleClockDemo::render2D()
{
    int numBeats = 4;
    float beatSpacing = 160.0f;
    for (int i = 0; i < numBeats; i++)
    {
        renderBeat(beatSpacing + i * beatSpacing, KowalskiDemoManager::VIEWPORT_HEIGHT / 2, i);
    }
}

void SampleClockDemo::renderBeat(float xCenter, float yCenter, int idx)
{
    const float boxSize = 100;
    float alphaFactor = m_mixerPaused ? 0.3f : 1.0f;
    
    if (idx == m_currentBeatIndex)
    {
        glColor4f(0.0f, 0.3f, 0.6f, 0.4f * alphaFactor);
    }
    else
    {
        glColor4f(0.0f, 0.3f, 0.6f, 0.2f * alphaFactor);
    }

    glBegin(GL_QUADS);
    glVertex2f(xCenter - boxSize / 2, yCenter - boxSize / 2);
    glVertex2f(xCenter + boxSize / 2, yCenter - boxSize / 2);
    glVertex2f(xCenter + boxSize / 2, yCenter + boxSize / 2);
    glVertex2f(xCenter - boxSize / 2, yCenter + boxSize / 2);
    glEnd();
    
    if (idx == m_currentBeatIndex)
    {
        glColor4f(0.0f, 0.3f, 0.6f, 1 * alphaFactor);
    }
    else
    {
        glColor4f(0.0f, 0.3f, 0.6f, 0.7f * alphaFactor);
    }
    
    glBegin(GL_LINE_LOOP);
    glVertex2f(xCenter - boxSize / 2, yCenter - boxSize / 2);
    glVertex2f(xCenter + boxSize / 2, yCenter - boxSize / 2);
    glVertex2f(xCenter + boxSize / 2, yCenter + boxSize / 2);
    glVertex2f(xCenter - boxSize / 2, yCenter + boxSize / 2);
    glEnd();
    
    glColor3f(0, 0, 0);
    if (idx == 0)
    {
        BitmapFont::drawString(xCenter, yCenter, "1", BitmapFont::HCENTER | BitmapFont::VCENTER);
    }
    else if (idx == 1)
    {
        BitmapFont::drawString(xCenter, yCenter, "2", BitmapFont::HCENTER | BitmapFont::VCENTER);
    }
    else if (idx == 2)
    {
        BitmapFont::drawString(xCenter, yCenter, "3", BitmapFont::HCENTER | BitmapFont::VCENTER);
    }
    else if (idx == 3)
    {
        BitmapFont::drawString(xCenter, yCenter, "4", BitmapFont::HCENTER | BitmapFont::VCENTER);
    }
}

void SampleClockDemo::initialize()
{
    m_frameCount = 0;
    m_waveBankHandle = 
        kwlWaveBankLoad(getResourcePath("music.kwb"));
    m_eventHandle = kwlEventGetHandle("music/la_romance_quoi_120bpm_loop");
    kwlEventStartFade(m_eventHandle, 3.0f);
    m_currentBeatIndex = 0;
    m_mixerPaused = false;
}

void SampleClockDemo::deinitialize()
{
    kwlMixerResume();
    kwlWaveBankUnload(m_waveBankHandle);
    kwlEventRelease(m_eventHandle);
}
