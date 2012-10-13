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
#include "assert.h"
#include "BitmapFont.h"
#include "KowalskiDemoManager.h"
#include "ComplexSoundsDemo.h"
#include <SDL/SDL_opengl.h>

const char* ComplexSoundsDemo::m_eventIDs[COMPLEX_SOUNDS_DEMO_NUM_EVENTS] = 
{
    "complexevents/in_random_no_repeat_out",
    "complexevents/in_random_out",
    "complexevents/in_sequential_out",
    "complexevents/random",
    "complexevents/random_no_repeat",
    "complexevents/sequential",
    "complexevents/sequential_no_reset",
    
    "complexevents/in_random_no_repeat_out_2",
    "complexevents/in_random_out_2",
    "complexevents/in_sequential_out_2",
    "complexevents/random_2",
    "complexevents/random_no_repeat_2",
    "complexevents/sequential_2",
    "complexevents/sequential_no_reset_2",
};

const char* ComplexSoundsDemo::m_eventLabels[COMPLEX_SOUNDS_DEMO_NUM_EVENTS] = 
{
    "1. in, random no repeat, out",
    "2. in, random, out",
    "3. in, sequential, out",
    "4. random",
    "5. random, no, repeat",
    "6. sequential",
    "7. sequential, no reset",
    
    "q. in, random no repeat, out (defer stop)",
    "w. in, random, out (defer stop)",
    "e. in, sequential, out (defer stop)",
    "r. random (defer stop)",
    "t. random, no, repeat (defer stop)",
    "y. sequential (defer stop)",
    "u. sequential, no reset (defer stop)"
};


ComplexSoundsDemo::ComplexSoundsDemo()
{

}

ComplexSoundsDemo::~ComplexSoundsDemo()
{

}

void ComplexSoundsDemo::update(float timeStep)
{

}

const char* ComplexSoundsDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Start an event: press the indicated key"; 
        case 1:
            return "Stop an event: release the indicated key";
    }
    return "";
}

int ComplexSoundsDemo::getEventIndexFromKey(SDLKey key)
{
    switch (key)
    {
        case SDLK_1:
            return 0;
        case SDLK_2:
            return 1;
        case SDLK_3:
            return 2;
        case SDLK_4:
            return 3;
        case SDLK_5:
            return 4;
        case SDLK_6:
            return 5;
        case SDLK_7:
            return 6;
        case SDLK_q:
            return 7;
        case SDLK_w:
            return 8;
        case SDLK_e:
            return 9;
        case SDLK_r:
            return 10;
        case SDLK_t:
            return 11;
        case SDLK_y:
            return 12;
        case SDLK_u:
            return 13;
    }
    return -1;
}

void ComplexSoundsDemo::onKeyDown(SDLKey key)
{
    int idx = getEventIndexFromKey(key);
    
    if (idx >= 0)
    {
        kwlEventStart(m_eventHandles[idx]);
    }
}

void ComplexSoundsDemo::onKeyUp(SDLKey key)
{
    int idx = getEventIndexFromKey(key);
    
    if (idx >= 0)
    {
        kwlEventStop(m_eventHandles[idx]);
    }
}

void ComplexSoundsDemo::render2D()
{
    float yTop = KowalskiDemoManager::VIEWPORT_HEIGHT - 100;
    float y = yTop;
    float x = KowalskiDemoManager::VIEWPORT_WIDTH / 2;
    
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        if (kwlEventIsPlaying(m_eventHandles[i]))
        {
            glColor3f(0, 1, 0);
        }
        else
        {
            glColor3f(1, 0, 0);
        }
        
        BitmapFont::drawString(x, y, m_eventLabels[i]);
        y -= 15;
    }
}

void ComplexSoundsDemo::initialize()
{
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        m_eventHandles[i] = kwlEventGetHandle(m_eventIDs[i]);
        kwlError e = kwlGetError();
        printf("ev id %s\n", m_eventIDs[i]);
        assert(e == KWL_NO_ERROR);
    }
    
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("numbers.kwb"));
}

void ComplexSoundsDemo::deinitialize()
{
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        kwlEventRelease(m_eventHandles[i]);
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
    
    kwlWaveBankUnload(m_waveBankHandle);
}

