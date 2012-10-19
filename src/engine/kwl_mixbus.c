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

#include "kwl_asm.h"
#include "kwl_eventinstance.h"
#include "kwl_memory.h"
#include "kwl_mixbus.h"
#include "kwl_sounddefinition.h"

kwlMixBus* kwlMixBus_alloc()
{
    kwlMixBus* newMixBus = (kwlMixBus*)KWL_MALLOC(sizeof(kwlMixBus), "kwlMixBus_alloc");
    kwlMemset(newMixBus, 0, sizeof(kwlMixBus));
    kwlMixBus_init(newMixBus);
    return newMixBus;
}

void kwlMixBus_init(kwlMixBus* mixBus)
{
    kwlMemset(mixBus, 0, sizeof(kwlMixBus));
    
    mixBus->totalPitch.valueEngine = 0.0f;
    mixBus->totalPitch.valueMixer = 0.0f;
    mixBus->totalPitch.valueShared = 0.0f;

    mixBus->totalGainLeft.valueEngine = 0.0f;
    mixBus->totalGainLeft.valueMixer = 0.0f;
    mixBus->totalGainLeft.valueShared = 0.0f;
    
    mixBus->totalGainRight.valueEngine = 0.0f;
    mixBus->totalGainRight.valueMixer = 0.0f;
    mixBus->totalGainRight.valueShared = 0.0f;

    mixBus->userGainLeft = 1.0f;
    mixBus->userGainRight = 1.0f;
    mixBus->userPitch = 1.0f;
    
    mixBus->mixPresetGainLeft = 1.0f;
    mixBus->mixPresetGainRight = 1.0f;
    mixBus->mixPresetPitch = 1.0f;
    
    mixBus->isMaster = 0;
    
    mixBus->numSubBuses = 0;
    mixBus->subBuses = NULL;
}

void kwlMixBus_dealloc(kwlMixBus* mixBus)
{
    if (mixBus->id != NULL)
    {
        KWL_FREE(mixBus->id);
    }
    KWL_FREE(mixBus);
}

void kwlMixBus_addEvent(kwlMixBus* bus, kwlEventInstance* event)
{
    /*printf("adding event %d to bus %s\n", (int)event, bus->id);
      printf("    event list before:\n");
    kwlEventInstance* tempEvent = bus->eventList;
    while (tempEvent != NULL)
    {
        if (event == tempEvent)
        {
            printf("kwlMixBus_addEvent: event %s is already in mix bus %s\n", event->definition_mixer->id, bus->id);
        }
        tempEvent = tempEvent->nextEvent_mixer;
    }
    */
    
    KWL_ASSERT(event->nextEvent_mixer == NULL && "event to add already has event(s) attached to it");
        
    kwlEventInstance* eventi = bus->eventList;
    if (eventi == NULL)
    {
        /*The list is empty. Make the incoming event the first item.*/
        bus->eventList = event;
    }
    else
    {
        /*Find the last event in the list...*/
        while (eventi->nextEvent_mixer != NULL)
        {
            eventi = eventi->nextEvent_mixer;
        }
        /*...and attach the new event to it*/
        eventi->nextEvent_mixer = event;
    }
    
    /*printf("    event list after:\n");
    tempEvent = bus->eventList;
    while (tempEvent != NULL)
    {
        printf("        %d\n", (int)tempEvent);
        tempEvent = tempEvent->nextEvent_mixer;
    }*/

}

void kwlMixBus_removeEvent(kwlMixBus* bus, kwlEventInstance* event)
{
    kwlEventInstance* prevEvent = NULL;
    kwlEventInstance* eventi = bus->eventList;
    
    while (eventi != event)
    {
        prevEvent = eventi;
        eventi = eventi->nextEvent_mixer;
    }
    
    if (prevEvent == NULL)
    {
        bus->eventList = eventi->nextEvent_mixer;
    }
    else
    {
        prevEvent->nextEvent_mixer = eventi->nextEvent_mixer;
    }
    
    event->nextEvent_mixer = NULL;
    
    /*
    kwlEventInstance* current
    if (previousEvent)
    {
        previousEvent->nextEvent_mixer = event->nextEvent_mixer;
    }
    else
    {
        mixBusi->eventList = event->nextEvent_mixer;
    }*/
}


void kwlMixBus_render(kwlMixBus* mixBus, 
                      void* mixerVoid, //TODO: made this a void* to get things to compile. should be kwlMixer*
                      int numOutChannels,
                      int numFrames, 
                      float* busScratchBuffer,
                      float* eventScratchBuffer,
                      float* outBuffer,
                      float accumulatedPitch,
                      float accumulatedGainLeft,
                      float accumulatedGainRight)
{
    kwlMixer* mixer = (kwlMixer*)mixerVoid;
    const int numSubBuses = mixBus->numSubBuses;
    
    /* Render sub buses recursively. */
    for (int i = 0; i < numSubBuses; i++)
    {
        kwlMixBus* busi = mixBus->subBuses[i];
        kwlMixBus_render(busi, 
                         mixer,
                         numOutChannels, 
                         numFrames,
                         busScratchBuffer,
                         eventScratchBuffer,
                         outBuffer,
                         busi->totalPitch.valueMixer * accumulatedPitch,
                         busi->totalGainLeft.valueMixer * accumulatedGainLeft,
                         busi->totalGainRight.valueMixer *accumulatedGainRight);
    }
    
    /* Mix the events of this bus into the out buffer. */
    kwlClearFloatBuffer(busScratchBuffer, numOutChannels * numFrames);
    kwlEventInstance* event = mixBus->eventList;
    int numEventsInBus = 0;    
    
    while (event != NULL)
    {
        int eventFinishedPlaying = kwlEventInstance_render(event, 
                                                   eventScratchBuffer, 
                                                   numOutChannels,
                                                   numFrames,
                                                   accumulatedPitch);
                
        /*mix event temp buffer into mixbus temp buffer*/
        kwlMixFloatBuffer(eventScratchBuffer, 
                          busScratchBuffer,
                          numOutChannels * numFrames);
        
        numEventsInBus++;
            
        /*Get the next event in the linked list and
          rearrange the list if the current event
          just stopped playing*/
        if (eventFinishedPlaying)
        {
            kwlMixer_sendEventStoppedMessage(mixer, event);
            /* Risky business: we're manipulating the linked list we're iterating over.
               Cache the next event in the bus because nextEvent_mixer gets reset
               when removing the event from the bus.
             */
            kwlEventInstance* nextEvent = event->nextEvent_mixer;
            kwlMixBus_removeEvent(mixBus, event);
            event = nextEvent;
        }
        else
        {
            event = event->nextEvent_mixer;
        }
    }
    
    /*Feed the bus output through the DSP unit if any.*/
    if (mixBus->dspUnit.valueMixer)
    {
        kwlDSPUnit* dspUnit = (kwlDSPUnit*)mixBus->dspUnit.valueMixer;
        /*process and replace mixbus temp buffer*/
        (*dspUnit->dspCallback)(busScratchBuffer,
                                numOutChannels,
                                numFrames, 
                                dspUnit->data);
    }

    /*if we have mixed any events for this bus, 
      mix the result into the output buffer*/
    if (numEventsInBus > 0)
    {
        /*... and then mix the bus buffer into the out buffer, applying
          the mix bus gain.*/
        for (int ch = 0; ch < numOutChannels; ch++)
        {
            kwlMixFloatBufferWithGain(busScratchBuffer, 
                                      outBuffer, 
                                      numOutChannels * numFrames, 
                                      ch, 
                                      numOutChannels, 
                                      //TODO: propagate gain values
                                      ch == 0 ? accumulatedGainLeft :
                                                accumulatedGainRight);
        }
    }
}

#ifdef KOWALSKI_DEBUG_LOADING
void kwlMixBus_print(kwlMixBus* bus, int recursionDepth)
{
    int i;
    for (i = 0; i < recursionDepth; i++)
    {
        printf("    ");
    }
    
    printf("MixBus: %s, gain l=%f (%f), gain r=%f (%f), pitch=%f (%f)\n", 
           bus->id, 
           bus->gainLeft, bus->accumulatedGainLeft,
           bus->gainRight, bus->accumulatedGainRight,
           bus->pitch, bus->accumulatedPitch);
    
    for (int i = 0; i < bus->numSubBuses; i++)
    {
        kwlMixBus_print(bus->subBuses[i], recursionDepth + 1);
    }
}

#endif /*KOWALSKI_DEBUG_LOADING*/
