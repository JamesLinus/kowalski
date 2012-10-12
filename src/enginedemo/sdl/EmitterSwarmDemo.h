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

#ifndef KOWALSKI_EMITTER_SWARM_DEMO_H
#define KOWALSKI_EMITTER_SWARM_DEMO_H

#include "Camera.h"
#include "SoundEmitter.h"
#include "PositionalAudioDemoBase.h"

class EmitterSwarmDemo : public PositionalAudioDemoBase
{
public:
    EmitterSwarmDemo();
    ~EmitterSwarmDemo();
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render3D();
    virtual void initialize();
    virtual void deinitialize();
    void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Demonstrates positional event one-shot playback and instance stealing.";}
    virtual const char* getInstructionLine(int index);
private:
    int m_numRowsAndColumns;
    kwlWaveBankHandle m_waveBankHandle;
};

#endif //KOWALSKI_EMITTER_SWARM_DEMO_H
