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
#include "kowalski.h"
#include "KowalskiDemoManager.h"
#include "PianoDemo.h"
#include <SDL/SDL_opengl.h>

PianoDemo::PianoDemo() :
    m_keyEventHandles(),
    m_waveBankHandle(KWL_INVALID_HANDLE),
    m_notesMixBusHandle(KWL_INVALID_HANDLE),
    m_currentPitch(1.0f),
    m_pitchMeter(0.5f, 2.0f, false)
{
}

PianoDemo::~PianoDemo()
{
}

const char* PianoDemo::getName()
{
    return "Piano";
}

const char* PianoDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Trigger notes using the indicated keys."; 
        case 1:
            return "Adjust pitch: up/down arrow keys."; 
    }
    return "";
}

void PianoDemo::update(float timeStep)
{

    Uint8* kstates = SDL_GetKeyState(NULL);
    float delta = 0.6f * timeStep;
    if (kstates[SDLK_UP])
    {
        m_currentPitch += delta;
    }
    if (kstates[SDLK_DOWN])
    {
        m_currentPitch -= delta;
    }
    
    if (m_currentPitch < 0.5f)
    {
        m_currentPitch = 0.5f;
    }
    else if (m_currentPitch > 2.0f)
    {
        m_currentPitch = 2.0f;
    }
    
    kwlMixBusSetPitch(m_notesMixBusHandle, m_currentPitch);
    
}

void PianoDemo::render2D()
{
    float keyboardWidth = 400;
    float keyboardHeight = 200;
    float keyPadding = 3;
    float keyWidth = keyboardWidth / 8;
    float xLeft = KowalskiDemoManager::VIEWPORT_WIDTH / 2 - keyboardWidth / 2;
    const float y = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    
    float relKeyPositions[13] = 
    {
        0.0f, //c
        0.5f, //c#
        1.0f, //d
        1.5f, //d#
        2.0f, //e
        3.0f, //f
        3.5f, //f#
        4.0f, //g
        4.5f, //g#
        5.0f, //a
        5.5f, //b
        6.0f, //h
        7.0f, //c
    };
    
    const char* keyLabels[13] = 
    {
        "q", //c
        "2", //c#
        "w", //d
        "3", //d#
        "e", //e
        "r", //f
        "5", //f#
        "t", //g
        "6", //g#
        "y", //a
        "7", //b
        "u", //h
        "i", //c
    };
    
    glLineWidth(2);
    //white keys
    for (int i = 0; i < 13; i++)
    {
        float relX = relKeyPositions[i];
        if (relX - ((int)relX) > 0.001)
        {
            //black key. draw in the next pass
            continue;
        }
        
        float keyX = xLeft + keyWidth * relX;
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(keyX + keyPadding, y - keyboardHeight / 2);
        glVertex2f(keyX + keyWidth - keyPadding, y - keyboardHeight / 2);
        glVertex2f(keyX + keyWidth - keyPadding, y + keyboardHeight / 2);
        glVertex2f(keyX + keyPadding, y + keyboardHeight / 2);
        glEnd();
        if (kwlEventIsPlaying(m_keyEventHandles[i]))
        {
            glColor3f(0, 1, 0);
        }
        else
        {
            glColor3f(1, 0, 0);
        }
        glBegin(GL_LINE_LOOP);
        glVertex2f(keyX + keyPadding, y - keyboardHeight / 2);
        glVertex2f(keyX + keyWidth - keyPadding, y - keyboardHeight / 2);
        glVertex2f(keyX + keyWidth - keyPadding, y + keyboardHeight / 2);
        glVertex2f(keyX + keyPadding, y + keyboardHeight / 2);
        glEnd();
        glColor3f(0, 0, 0);
        BitmapFont::drawString(keyX + keyWidth / 2, y - keyboardHeight * 0.4, keyLabels[i]);
        keyX += keyWidth;
    }
    //black keys
    for (int i = 0; i < 13; i++)
    {
        float relX = relKeyPositions[i];
        if (relX - ((int)relX) < 0.001)
        {
            //white key. already drawn
            continue;
        }
        
        float keyX = xLeft + keyWidth * relX;
        glColor3f(0, 0, 0);

        glBegin(GL_QUADS);
        glVertex2f(keyX + keyPadding, y);
        glVertex2f(keyX + keyWidth - keyPadding, y);
        glVertex2f(keyX + keyWidth - keyPadding, y + keyboardHeight / 2);
        glVertex2f(keyX + keyPadding, y + keyboardHeight / 2);
        glEnd();
        
        if (kwlEventIsPlaying(m_keyEventHandles[i]))
        {
            glColor3f(0, 1, 0);
        }
        else
        {
            glColor3f(1, 0, 0);
        }
        
        glBegin(GL_LINE_LOOP);
        glVertex2f(keyX + keyPadding, y);
        glVertex2f(keyX + keyWidth - keyPadding, y);
        glVertex2f(keyX + keyWidth - keyPadding, y + keyboardHeight / 2);
        glVertex2f(keyX + keyPadding, y + keyboardHeight / 2);
        glEnd();
        
        glColor3f(1, 1, 1);
        BitmapFont::drawString(keyX + keyWidth / 2, y + keyboardHeight * 0.1, keyLabels[i]);
        
        keyX += keyWidth;
    }
    glLineWidth(1);
    
    float meterWidth = 30;
    float meterHeight = keyboardHeight;
    m_pitchMeter.setValue(m_currentPitch);
    m_pitchMeter.render(xLeft - 2 * meterWidth, y - meterHeight / 2, meterWidth, meterHeight);
}

void PianoDemo::onKeyDown(SDLKey key)
{
    int idx = -1;
    if (key == SDLK_q)
    {
        idx = 0;
    }
    else if (key == SDLK_2)
    {
        idx = 1;
    }
    else if (key == SDLK_w)
    {
        idx = 2;
    }
    else if (key == SDLK_3)
    {
        idx = 3;
    }
    else if (key == SDLK_e)
    {
        idx = 4;
    }
    else if (key == SDLK_r)
    {
        idx = 5;
    }
    else if (key == SDLK_5)
    {
        idx = 6;
    }
    else if (key == SDLK_t)
    {
        idx = 7;
    }
    else if (key == SDLK_6)
    {
        idx = 8;
    }
    else if (key == SDLK_y)
    {
        idx = 9;
    }
    else if (key == SDLK_7)
    {
        idx = 10;
    }
    else if (key == SDLK_u)
    {
        idx = 11;
    }
    else if (key == SDLK_i)
    {
        idx = 12;
    }
    
    if (idx >= 0)
    {
        kwlEventStart(m_keyEventHandles[idx]);
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
}

void PianoDemo::initialize()
{
    m_keyEventHandles[0] = kwlEventGetHandle("pianodemo/c-1");
    m_keyEventHandles[1] = kwlEventGetHandle("pianodemo/c#-1");
    m_keyEventHandles[2] = kwlEventGetHandle("pianodemo/d-1");
    m_keyEventHandles[3] = kwlEventGetHandle("pianodemo/d#-1");
    m_keyEventHandles[4] = kwlEventGetHandle("pianodemo/e-1");
    m_keyEventHandles[5] = kwlEventGetHandle("pianodemo/f-1");
    m_keyEventHandles[6] = kwlEventGetHandle("pianodemo/f#-1");
    m_keyEventHandles[7] = kwlEventGetHandle("pianodemo/g-1");
    m_keyEventHandles[8] = kwlEventGetHandle("pianodemo/g#-1");
    m_keyEventHandles[9] = kwlEventGetHandle("pianodemo/a-1");
    m_keyEventHandles[10] = kwlEventGetHandle("pianodemo/b-1");
    m_keyEventHandles[11] = kwlEventGetHandle("pianodemo/h-1");
    m_keyEventHandles[12] = kwlEventGetHandle("pianodemo/c-2");
    kwlError error = kwlGetError();
    assert(error == KWL_NO_ERROR);
    m_currentPitch = 1.0f;
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("notes.kwb"));
    error = kwlGetError();
    assert(error == KWL_NO_ERROR);
    m_notesMixBusHandle = kwlMixBusGetHandle("notes");
    error = kwlGetError();
    assert(error == KWL_NO_ERROR);
}

void PianoDemo::deinitialize()
{
    for (int i = 0; i < 13; i++)
    {
        kwlEventRelease(m_keyEventHandles[i]);
    }
    kwlWaveBankUnload(m_waveBankHandle);
}
