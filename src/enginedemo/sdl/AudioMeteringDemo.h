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

#ifndef KOWALSKI_AUDIO_METERING_DEMO_H
#define KOWALSKI_AUDIO_METERING_DEMO_H

#include "Camera.h"
#include "MeterBar.h"
#include "SoundEmitter.h"
#include "DemoBase.h"

class AudioMeteringDemo : public DemoBase
{
public:
    AudioMeteringDemo();
    ~AudioMeteringDemo();

    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    virtual void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Shows audio meters for the left and right output channels and indicates if clipping occured.";}
    virtual const char* getInstructionLine(int index);
private:
    float m_timeSinceLastClip;
    kwlEventHandle m_eventHandle;
    kwlWaveBankHandle m_waveBankHandle;
    MeterBar m_meterBarLeft;
    MeterBar m_meterBarRight;
};

#endif //KOWALSKI_POSITIONAL_AUDIO_DEMO_H

