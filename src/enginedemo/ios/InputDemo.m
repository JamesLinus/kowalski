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

#import "InputDemo.h"

void processInputBuffer(float* buffer, int numChannels, 
                        int numFrames, void* data)
{
    InputDSPUnitData* d = (InputDSPUnitData*)data;
    
    float maxAbs = 0.0f;
    for (int ch = 0; ch < numChannels; ch++)
    {
        int idx = ch;
        for (int i = 0; i < numFrames; i++)
        {
            float abs = buffer[idx] > 0.0f ? buffer[idx] : -buffer[idx];
            if (abs > maxAbs)
            {
                maxAbs = abs;
            }
            idx += numChannels;
        }
    }
    
    d->m_currentLevelMixer = maxAbs;
}

void updateParamsEngine(void* data)
{
    InputDSPUnitData* d = (InputDSPUnitData*)data;

    //a simple envelope follower
    if (d->m_currentLevelShared > d->m_currentLevelEngine)
    {
        d->m_currentLevelEngine = d->m_currentLevelShared;
    }
    else
    {
        d->m_currentLevelEngine *= 0.95;
    }
}

void updateParamsMixer(void* data)
{
    InputDSPUnitData* d = (InputDSPUnitData*)data;    
    d->m_currentLevelShared = d->m_currentLevelMixer;
}

@implementation InputDemo

@synthesize levelMeter;

-(NSString*)getName
{
    return @"Audio Input";
}

-(NSString*)getDescription
{
    return @"";
}

-(NSString*)getInstructions
{
    return @"";
}

-(void)initialize
{
    m_dspUnitData.m_currentLevelEngine = 0.0f;
    m_dspUnitData.m_currentLevelShared = 0.0f;
    m_dspUnitData.m_currentLevelMixer = 0.0f;
    
    //set up the dsp unit;
    m_dspUnit = kwlDSPUnitCreateCustom(&m_dspUnitData, 
                                       processInputBuffer, 
                                       updateParamsEngine, 
                                       updateParamsMixer, 
                                       NULL);
    kwlDSPUnitAttachToInput(m_dspUnit);
    
    
}

-(void)deinitialize
{
    //remove dsp unit 
    kwlDSPUnitAttachToInput(NULL);
}

-(void)update:(float)timeStep
{
    levelMeter.progress = m_dspUnitData.m_currentLevelEngine;
}
@end
