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
#include "SeamlessPitchDemo.h"
#include <SDL/SDL_opengl.h>
#include "kowalski.h"

SeamlessPitchDemo::SeamlessPitchDemo() :
    m_eventHandle(KWL_INVALID_HANDLE),
    m_waveBankHandle(KWL_INVALID_HANDLE),
    m_currentPitch(0.0f),
    m_pitchBar(0.0f, 2.0f, false)
{

}

SeamlessPitchDemo::~SeamlessPitchDemo()
{
}

const char* SeamlessPitchDemo::getName()
{
    return "Pitch control";
}

const char* SeamlessPitchDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Change pitch: up/down keys";
    }
    return "";
}

void SeamlessPitchDemo::update(float timeStep)
{
    Uint8* kstates = SDL_GetKeyState(NULL);
    float delta = 2.0f * timeStep;
    if (kstates[SDLK_UP])
    {
        m_currentPitch += delta;
    }
    if (kstates[SDLK_DOWN])
    {
        m_currentPitch -= delta;
    }
    
    if (m_currentPitch < 0.0f)
    {
        m_currentPitch = 0.0f;
    }
    if (m_currentPitch > 2.0f)
    {
        m_currentPitch = 2.0f;
    }
    
    kwlEventSetPitch(m_eventHandle, m_currentPitch);
}

void SeamlessPitchDemo::onKeyDown(SDLKey key)
{

}

void SeamlessPitchDemo::render2D()
{
    float x = KowalskiDemoManager::VIEWPORT_WIDTH / 2;
    float y = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    float w = 30;
    float h = 400;
    
    m_pitchBar.setValue(m_currentPitch);
    m_pitchBar.render(x - w / 2, y - h / 2, w, h);
}

void SeamlessPitchDemo::initialize()
{
    m_currentPitch = 0.3465f;
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("sfx.kwb"));
    m_eventHandle = kwlEventGetHandle("pitchdemo/cosineloop");
    kwlEventStart(m_eventHandle);
    kwlError error = kwlGetError();
    assert(error == KWL_NO_ERROR);
}

void SeamlessPitchDemo::deinitialize()
{
    kwlEventStop(m_eventHandle);
    kwlWaveBankUnload(m_waveBankHandle);
    kwlEventRelease(m_eventHandle);
    m_eventHandle = KWL_INVALID_HANDLE;
}
