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

#ifndef KOWALSKI_DOPPLER_SHIFT_DEMO_H
#define KOWALSKI_DOPPLER_SHIFT_DEMO_H

#include "Camera.h"
#include "PositionalAudioDemoBase.h"

class DopplerShiftDemo : public PositionalAudioDemoBase
{
public:
    DopplerShiftDemo();
    ~DopplerShiftDemo();
    virtual const char* getDescription() { return "Demonstrates how events moving relative to the listener undergo a change in pitch.";}
private:
    virtual const char* getName();
    virtual void update(float timeStep);
    void updateEmitterPositions(float timeStep);
    virtual void render2D();
    virtual void render3D();
    virtual void initialize();
    virtual void deinitialize();
    void onKeyDown(SDLKey key);
    void setRotatingEmitterPosition(float angle);
    float m_rotationAngle;
    float m_rotationRadius;
    kwlWaveBankHandle m_waveBankHandle;
};

#endif //KOWALSKI_DOPPLER_SHIFT_DEMO_H

