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
#include "kowalski_ext.h"
#include "BitmapFont.h"
#include "FreeformEventsDemo.h"
#include "KowalskiDemoManager.h"
#include <math.h>
#include "SDL_opengl.h"

const char* const FreeformEventsDemo::m_fileNames[NUM_EVENTS] = 
{
    "sine_signed_8bit_mono.au",
    "sine_signed_16bit_mono.au",
    "sine_signed_24bit_mono.au",
    "sine_signed_32bit_mono.au",
    
    "sine_signed_8bit_mono.aiff",
    "sine_signed_16bit_mono.aiff",
    "sine_signed_24bit_mono.aiff",
    "sine_signed_32bit_mono.aiff",
     
    "sine_unsigned_8bit_mono.wav",
    "sine_signed_16bit_mono.wav",
    "sine_signed_24bit_mono.wav",
    "sine_signed_32bit_mono.wav",
    
    "procedurally generated"
};

const char* const FreeformEventsDemo::m_triggerKeyNames[NUM_EVENTS] = 
{
    "1",
    "2",
    "3",
    "4",
    
    "q",
    "w",
    "e",
    "r",
    
    "a",
    "s",
    "d",
    "f",
    
    "z",
};

SDLKey FreeformEventsDemo::m_triggerKeys[NUM_EVENTS] = 
{
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_r,
    
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_f,
    
    SDLK_z
};

FreeformEventsDemo::FreeformEventsDemo() :
    m_eventHandles(),
    m_buffer(NULL)
{
}

FreeformEventsDemo::~FreeformEventsDemo()
{
}

const char* FreeformEventsDemo::getName()
{
    return "Freeform events";
}

const char* FreeformEventsDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Start an event: press the indicated key"; 
        case 1:
            return "Load/unload engine data: L";
    }
    return "";
}

void FreeformEventsDemo::onKeyDown(SDLKey key)
{
    for (int i = 0; i < NUM_EVENTS; i++)
    {
        if (key == m_triggerKeys[i])
        {
            kwlEventStart(m_eventHandles[i]);
        }
    }
    
    if (key == SDLK_l)
    {
        if (kwlEngineDataIsLoaded())
        {
            kwlEngineDataUnload();
        }
        else
        {
            kwlEngineDataLoad(getResourcePath("demoproject.kwl"));
            kwlError e = kwlGetError();
            assert(e == KWL_NO_ERROR);
        }
    }
}

void FreeformEventsDemo::update(float timeStep)
{
    //
}

void FreeformEventsDemo::render2D()
{
    float x = KowalskiDemoManager::VIEWPORT_WIDTH / 2;
    float y = KowalskiDemoManager::VIEWPORT_HEIGHT - 100;
    
    for (int i = 0; i < NUM_EVENTS; i++)
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        BitmapFont::drawString(x - 110, y, m_triggerKeyNames[i], BitmapFont::HCENTER);
        if (kwlEventIsPlaying(m_eventHandles[i]))
        {
            glColor3f(0, 1, 0);
        }
        else
        {
            glColor3f(1, 0, 0);
        }
        BitmapFont::drawString(x, y, m_fileNames[i], BitmapFont::HCENTER);
        y -= 10;
    }
    glColor3f(0.0f, 0.0f, 0.0f);
    BitmapFont::drawString(x, y - 50, kwlEngineDataIsLoaded() ? "engine data is loaded" : "engine data is not loaded" , BitmapFont::HCENTER);
}

void FreeformEventsDemo::initialize()
{
    for (int i = 0; i < NUM_EVENTS; i++)
    {
        if (i < NUM_EVENTS - 1)
        {
            m_eventHandles[i] = 
                kwlEventCreateWithFile(getResourcePath(m_fileNames[i]), KWL_POSITIONAL, 0);
        }
        else
        {
            assert(m_buffer == NULL);
            /*create a procedurally generated mono sine event in the last slot*/
            int numFrames = 44100;
            float f = 50;
            m_buffer = new short[numFrames];
            for (int j = 0; j < numFrames; j++)
            {
                float envelope = (numFrames - j) / (float)numFrames;
                m_buffer[j] = (short)(32767 * envelope * envelope * sinf(f * j));
            }
            
            kwlPCMBuffer buffer;
            buffer.numChannels = 1;
            buffer.numFrames = numFrames;
            buffer.pcmData = m_buffer;
            
            m_eventHandles[i] = 
                kwlEventCreateWithBuffer(&buffer, KWL_POSITIONAL);
        }
    }
}

void FreeformEventsDemo::deinitialize()
{
    for (int i = 0; i < NUM_EVENTS; i++)
    {
        kwlEventRelease(m_eventHandles[i]);
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
    
    delete[] m_buffer;
    m_buffer = NULL;
    
}
