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
#ifndef KOWALSKI_FREEFORM_EVENTS_DEMO_H
#define KOWALSKI_FREEFORM_EVENTS_DEMO_H

#include "kowalski.h"
#include "DemoBase.h"

#define NUM_EVENTS 13

class FreeformEventsDemo : public DemoBase
{
public:
    FreeformEventsDemo();
    ~FreeformEventsDemo();
    
    virtual const char* getName();
    virtual void update(float timeStep);
    virtual void render2D();
    virtual void initialize();
    virtual void deinitialize();
    void onKeyDown(SDLKey key);
    virtual const char* getDescription() { return "Demonstrates playback of events created in code and how engine data can be unloaded and loaded independent of playback.";}
    virtual const char* getInstructionLine(int index);
private:
    kwlEventHandle m_eventHandles[NUM_EVENTS];
    static const char* const m_fileNames[NUM_EVENTS];
    static const char* const m_triggerKeyNames[NUM_EVENTS];
    static SDLKey m_triggerKeys[NUM_EVENTS];
    short* m_buffer;
};

#endif //KOWALSKI_FREEFORM_EVENTS_DEMO_H

