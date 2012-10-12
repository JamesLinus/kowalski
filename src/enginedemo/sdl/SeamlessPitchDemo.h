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

#ifndef KOWALSKI_SEAMLESS_PITCH_DEMO_H
#define KOWALSKI_SEAMLESS_PITCH_DEMO_H

#include "Camera.h"
#include "DemoBase.h"
#include "MeterBar.h"
#include "SoundEmitter.h"

class SeamlessPitchDemo : public DemoBase
{
public:
    SeamlessPitchDemo();
    ~SeamlessPitchDemo();
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Seamless pitch shifting of looping waveforms.";}
    virtual const char* getInstructionLine(int index);
private:
    kwlEventHandle m_eventHandle;
    kwlWaveBankHandle m_waveBankHandle;
    float m_currentPitch;
    MeterBar m_pitchBar;
};


#endif //KOWALSKI_SEAMLESS_PITCH_DEMO_H
