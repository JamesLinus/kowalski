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

#ifndef KOWALSKI_SAMPLE_CLOCK_DEMO_H
#define KOWALSKI_SAMPLE_CLOCK_DEMO_H

#include "Camera.h"
#include "SoundEmitter.h"
#include "DemoBase.h"

class SampleClockDemo : public DemoBase
{
public:
    SampleClockDemo();
    ~SampleClockDemo();
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Synchronizes a simple animation with a 120 BPM loop using the approximate sample clock API.";}
    virtual const char* getInstructionLine(int index);
private:
    void renderBeat(float xCenter, float yCenter, int idx);
    kwlEventHandle m_eventHandle;
    kwlWaveBankHandle m_waveBankHandle;
    const float m_bpm;
    int m_currentBeatIndex;
    unsigned int m_frameCount;
    bool m_mixerPaused;
};

#endif //KOWALSKI_SAMPLE_CLOCK_DEMO_H
