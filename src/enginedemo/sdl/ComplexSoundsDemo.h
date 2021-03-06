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

#ifndef KOWALSKI_COMPLEX_SOUNDS_DEMO_H
#define KOWALSKI_COMPLEX_SOUNDS_DEMO_H

#include "DemoBase.h"
#include "kowalski.h"

#define COMPLEX_SOUNDS_DEMO_NUM_EVENTS 14

class ComplexSoundsDemo : public DemoBase
{
public:
    ComplexSoundsDemo();
    ~ComplexSoundsDemo();
    
    virtual const char* getName() { return "Complex sounds"; }
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    virtual void onKeyDown(SDLKey key);
    virtual void onKeyUp(SDLKey key);
    virtual const char* getDescription() { return "Demonstrates complex playback behaviors achieved using sounds.";}
    virtual const char* getInstructionLine(int index);
private:
    int getEventIndexFromKey(SDLKey key);
    kwlEventHandle m_eventHandles[COMPLEX_SOUNDS_DEMO_NUM_EVENTS];
    kwlWaveBankHandle m_waveBankHandle;
    static const char* m_eventIDs[COMPLEX_SOUNDS_DEMO_NUM_EVENTS];
    static const char* m_eventLabels[COMPLEX_SOUNDS_DEMO_NUM_EVENTS];
};

#endif //KOWALSKI_COMPLEX_SOUNDS_DEMO_H
