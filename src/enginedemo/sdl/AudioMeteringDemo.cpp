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
#include "AudioMeteringDemo.h"
#include "BitmapFont.h"
#include "KowalskiDemoManager.h"

#include "SDL_opengl.h"

AudioMeteringDemo::AudioMeteringDemo() :
    m_timeSinceLastClip(0),
    m_eventHandle(KWL_INVALID_HANDLE),
    m_waveBankHandle(KWL_INVALID_HANDLE),
    m_meterBarLeft(0, 1, true),
    m_meterBarRight(0, 1, true)
{
}

AudioMeteringDemo::~AudioMeteringDemo()
{
}

void AudioMeteringDemo::update(float timeStep)
{
    m_timeSinceLastClip += timeStep;
    
    if (!kwlEventIsPlaying(m_eventHandle))
    {
        kwlEventStart(m_eventHandle);
    }
}

const char* AudioMeteringDemo::getName()
{
    return "Audio metering";
}

const char* AudioMeteringDemo::getInstructionLine(int index)
{
    return "";
}

void AudioMeteringDemo::onKeyDown(SDLKey key)
{
    
}

void AudioMeteringDemo::render2D()
{
    const float width = 40;
    const float height = 400;
    const float space = 10;
    const float y = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    const float xLeft = (KowalskiDemoManager::VIEWPORT_WIDTH - space) / 2 - width;
    const float xRight = (KowalskiDemoManager::VIEWPORT_WIDTH + space) / 2;
    
    float lvlLeft = kwlGetLevelLeft();
    float lvlRight = kwlGetLevelRight();
    if (kwlHasClipped())
    {
        m_timeSinceLastClip = 0;
    }
    
    const float clipBoxHeight = 20;
    const float clipBoxSpacing = 5;

    const float clipIndicatorFadeTime = 2.0f;
    float alpha = (clipIndicatorFadeTime - m_timeSinceLastClip) / clipIndicatorFadeTime;
    if (alpha < 0)
    {
        alpha = 0;
    }

    float x[2] = {xLeft, xRight};
    for (int i = 0; i < 2; i++)
    {    
        glColor4f(1, 0, 0, alpha);
        glBegin(GL_QUADS);
        glVertex2f(x[i], clipBoxSpacing + y + height / 2);
        glVertex2f(x[i] + width, clipBoxSpacing + y + height / 2);
        glVertex2f(x[i] + width, clipBoxSpacing + y + height / 2 + clipBoxHeight);
        glVertex2f(x[i], clipBoxSpacing + y + height / 2 + clipBoxHeight);
        glEnd();

        glColor4f(1, 1, 1, alpha);
        BitmapFont::drawString(x[i] + width / 2, 
                               clipBoxSpacing + y + height / 2 + clipBoxHeight / 2, 
                               "clip", BitmapFont::HCENTER | BitmapFont::VCENTER);
    }
    
    m_meterBarLeft.setValue(lvlLeft);
    m_meterBarLeft.render(xLeft, y - height / 2, width, height);
    
    m_meterBarRight.setValue(lvlRight);
    m_meterBarRight.render(xRight, y - height / 2, width, height);
}

void AudioMeteringDemo::initialize()
{
    m_timeSinceLastClip = 0;
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("music.kwb"));
    m_eventHandle = kwlEventGetHandle("music/music_stereo_ogg");
    m_timeSinceLastClip = 100000;
    kwlEventStart(m_eventHandle);
}

void AudioMeteringDemo::deinitialize()
{
    kwlWaveBankUnload(m_waveBankHandle);
    kwlEventRelease(m_eventHandle);
}

