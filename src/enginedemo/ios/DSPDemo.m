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

#include "DSPDemo.h"

void processBuffer(float* buffer, int numChannels, 
                   int numFrames, void* data)
{
    DSPUnitData* dspUnitData = (DSPUnitData*)data;
    
    /*apply gain and measure RMS level*/
    float rmsLevel = 0.0f;
    const float gain = dspUnitData->gainMixer;
    const int numSamples = numChannels * numFrames;
    
    for (int i = 0; i < numSamples; i++)
    {
        buffer[i] *= gain;
        rmsLevel += buffer[i] * buffer[i];
    }
    
    rmsLevel /= numFrames;
    
    dspUnitData->rmsLevelMixer = sqrtf(rmsLevel);
}

void updateParametersEngine(void* data)
{
    DSPUnitData* dspUnitData = (DSPUnitData*)data;
    
    /* Copy the shared measured level to an engine-only variable.*/
    dspUnitData->rmsLevelEngine = dspUnitData->rmsLevelShared;
    /* Copy the engine-only gain value to a shared variable 
     that the mixer thread can access.*/
    dspUnitData->gainShared = dspUnitData->gainEngine;
}

void updateParametersMixer(void* data)
{
    DSPUnitData* dspUnitData = (DSPUnitData*)data;
    /* Copy the shared user gain value to a mixer-only variable
     used in the processing callback.*/
    dspUnitData->gainMixer = dspUnitData->gainShared;
    /* Copy the mixer-only level value to a shared variable 
     that the engine thread can access.*/
    dspUnitData->rmsLevelShared = dspUnitData->rmsLevelMixer;
}



@implementation DSPDemo

@synthesize volumeSlider;
@synthesize rmsMeter;


-(NSString*)getName
{
    return @"Custom DSP units";
}

-(void)update:(float)timeStep
{
    rmsMeter.progress = m_dspUnitData.rmsLevelEngine;
}

-(void)initialize
{
    //load the waveforms needed
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"numbers.kwb"]);
    
    //set up the dsp unit
    memset(&m_dspUnitData, 0, sizeof(DSPUnitData));
    m_dspUnitData.gainEngine = 1.0f;
    volumeSlider.value = m_dspUnitData.gainEngine;
    m_dspUnit = kwlDSPUnitCreateCustom(&m_dspUnitData, 
                                       processBuffer, 
                                       updateParametersEngine,
                                       updateParametersMixer, 
                                       NULL);
    
    //attach the dsp unit to an infinitely looping event and start it
    m_eventHandle = kwlEventGetHandle("dspdemo/infinite_loop");
    kwlDSPUnitAttachToEvent(m_dspUnit, m_eventHandle);
    kwlEventStart(m_eventHandle);
}

-(void)deinitialize
{
    //stop and release the event
    kwlEventStop(m_eventHandle);
    kwlEventRelease(m_eventHandle);
    m_eventHandle = KWL_INVALID_HANDLE;
    
    //unload wave bank.
    kwlWaveBankUnload(m_waveBankHandle);
    m_waveBankHandle = KWL_INVALID_HANDLE;
}

- (IBAction)volumeSliderMoved:(id)sender
{
    UISlider* slider = (UISlider*)sender;
    m_dspUnitData.gainEngine = slider.value;
}


@end
