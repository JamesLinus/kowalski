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
#include "SafeWaveBankUnloadingDemo.h"
#include "SDL_opengl.h"

SafeWaveBankUnloadingDemo::SafeWaveBankUnloadingDemo()
{

}

SafeWaveBankUnloadingDemo::~SafeWaveBankUnloadingDemo()
{

}

const char* SafeWaveBankUnloadingDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Trigger event: P"; 
        case 1:
            return "Unload wave bank: U";
        case 2:
            return "Load wave bank: L";
    }
    return "";
}

const char* SafeWaveBankUnloadingDemo::getName()
{
    return "Safe wave bank unloading";
}

void SafeWaveBankUnloadingDemo::update(float timeStep)
{

}

void SafeWaveBankUnloadingDemo::render2D()
{
    float xMid = KowalskiDemoManager::VIEWPORT_WIDTH / 2;
    float yMid = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    
    if (kwlEventIsPlaying(m_eventHandle))
    {
        glColor3f(0, 1, 0);
        BitmapFont::drawString(xMid, yMid - 20, "Event is playing", BitmapFont::HCENTER);
    }
    else
    {
        glColor3f(1, 0, 0);
        BitmapFont::drawString(xMid, yMid - 20, "Event is not playing", BitmapFont::HCENTER);
    }
    
    if (kwlWaveBankIsLoaded(m_waveBankHandle))
    {
        glColor3f(0, 1, 0);
        BitmapFont::drawString(xMid, yMid + 20, "Wave bank is loaded", BitmapFont::HCENTER);
    }
    else
    {
        glColor3f(1, 0, 0);
        BitmapFont::drawString(xMid, yMid + 20, "Wave bank is not loaded", BitmapFont::HCENTER);
    }
    
}

void SafeWaveBankUnloadingDemo::initialize()
{
    m_eventHandle = kwlEventGetHandle("pianodemo/c-1");
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("notes.kwb"));
}

void SafeWaveBankUnloadingDemo::deinitialize()
{
    kwlEventRelease(m_eventHandle);
    m_eventHandle = KWL_INVALID_HANDLE;
    kwlWaveBankUnload(m_waveBankHandle);
}

void SafeWaveBankUnloadingDemo::onKeyDown(SDLKey key)
{
    if (key == SDLK_u)    
    {
        kwlWaveBankUnload(m_waveBankHandle);
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
    else if (key == SDLK_l)    
    {
        m_waveBankHandle = kwlWaveBankLoad(getResourcePath("notes.kwb"));
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
    else if (key == SDLK_p)    
    {
        kwlEventStart(m_eventHandle);
    }
}
