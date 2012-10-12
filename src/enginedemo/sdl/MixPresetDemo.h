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

#ifndef KOWALSKI_MIX_PRESET_DEMO_H
#define KOWALSKI_MIX_PRESET_DEMO_H

#include "DemoBase.h"
#include "kowalski.h"

#define NUM_PRESETS 3

class MixPresetDemo : public DemoBase
{
public:
    MixPresetDemo();
    ~MixPresetDemo();
    
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    virtual void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Demonstrates how mix presets can be used to switch between different mixes.";}
    virtual const char* getInstructionLine(int index);
private:
    kwlMixPresetHandle m_defaultPreset;
    kwlEventHandle m_noiseEvent;
    kwlEventHandle m_toneEvent;
    kwlEventHandle m_chirpEvent;
    int m_activePresetIndex;
    kwlMixPresetHandle m_presetHandles[NUM_PRESETS];
    static const char* const m_presetIDs[NUM_PRESETS];
    static SDLKey m_triggerKeys[NUM_PRESETS];
    bool m_doFade;
    kwlWaveBankHandle m_waveBankHandle;
};

#endif //KOWALSKI_MIX_PRESET_DEMO_H
