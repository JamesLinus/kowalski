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
#include "DistanceAttenuationDemo.h"

#include "KowalskiDemoManager.h"
#include "BitmapFont.h"
#include "assert.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>


DistanceAttenuationDemo::DistanceAttenuationDemo() :
    PositionalAudioDemoBase(),
    m_waveBankHandle(KWL_INVALID_HANDLE)
{
    
}

DistanceAttenuationDemo::~DistanceAttenuationDemo()
{
    
}

const char* DistanceAttenuationDemo::getInstructionLine(int index)
{
    if (index < 1)
    {
        return PositionalAudioDemoBase::getInstructionLine(index);
    }
    switch (index)
    {
        case 1:
            return "Trigger events: 1, 2, 3, 4, 5, 6"; 
    }
    return "";
}

void DistanceAttenuationDemo::initialize()
{
    PositionalAudioDemoBase::initialize();
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("sfx.kwb"));
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR);
    
    m_numEmitters = 6;
    float d = 10;
    m_emitters = new SoundEmitter[m_numEmitters];
    m_emitters[0] = SoundEmitter("distanceattenuationdemo/100hzsine", d, 0, 0);
    m_emitters[1] = SoundEmitter("distanceattenuationdemo/1ksine", -d, 0, 0);
    m_emitters[2] = SoundEmitter("distanceattenuationdemo/noise_burst", 0, d, 0);
    m_emitters[3] = SoundEmitter("distanceattenuationdemo/noise_fade_out", 0, -d, 0);
    m_emitters[4] = SoundEmitter("distanceattenuationdemo/sine_fade_out", 0, 0, d);
    m_emitters[5] = SoundEmitter("distanceattenuationdemo/square_fade_out", 0, 0, -d);
}

void DistanceAttenuationDemo::deinitialize()
{
    kwlWaveBankUnload(m_waveBankHandle);
    PositionalAudioDemoBase::deinitialize();
}    

void DistanceAttenuationDemo::render3D()
{
    PositionalAudioDemoBase::render3D();
}

void DistanceAttenuationDemo::render2D()
{

}

void DistanceAttenuationDemo::onKeyDown(SDLKey key)
{
    if (key == SDLK_1)
    {
        m_emitters[0].start();
    }
    if (key == SDLK_2)
    {
        m_emitters[1].start();
    }
    if (key == SDLK_3)
    {
        m_emitters[2].start();
    }
    if (key == SDLK_4)
    {
        m_emitters[3].start();
    }
    if (key == SDLK_5)
    {
        m_emitters[4].start();
    }
    if (key == SDLK_6)
    {
        m_emitters[5].start();
    }
}

const char* DistanceAttenuationDemo::getName()
{
    return "Positional audio: Distance attenuation";
}

void DistanceAttenuationDemo::update(float timeStep)
{
    PositionalAudioDemoBase::update(timeStep);
}

