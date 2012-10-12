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

#ifndef KOWALSKI_PIANO_DEMO_H
#define KOWALSKI_PIANO_DEMO_H

#include "DemoBase.h"
#include "MeterBar.h"
#include "kowalski.h"

class PianoDemo : public DemoBase
{
public:
    PianoDemo();
    ~PianoDemo();
    
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    virtual void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "A one octave keyboard with one event per note.";}
    virtual const char* getInstructionLine(int index);
private:
    kwlEventHandle m_keyEventHandles[13];
    kwlWaveBankHandle m_waveBankHandle;
    kwlMixBusHandle m_notesMixBusHandle;
    float m_currentPitch;
    MeterBar m_pitchMeter;
};

#endif //KOWALSKI_PIANO_DEMO_H
