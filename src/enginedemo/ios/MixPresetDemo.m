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

#import "MixPresetDemo.h"


@implementation MixPresetDemo

@synthesize presetSwitch;
@synthesize fadeSwitch;

-(void)awakeFromNib
{
    m_presetIDs[0] = "mixpresetdemo/chirps";
    m_presetIDs[1] = "mixpresetdemo/noise";
    m_presetIDs[2] = "mixpresetdemo/tones";
}

-(NSString*)getName
{
    return @"Mix Presets";
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
    for (int i = 0; i < NUM_PRESETS; i++)
    {
        m_presetHandles[i] = kwlMixPresetGetHandle(m_presetIDs[i]);
    }
    
    fadeSwitch.on = YES;
    m_doFade = true;
    
    m_defaultPreset = kwlMixPresetGetHandle("default");
    kwlMixPresetSet(m_presetHandles[0]);
    m_activePresetIndex = 0;
    presetSwitch.selectedSegmentIndex = m_activePresetIndex;
    
    m_noiseEvent = kwlEventGetHandle("mixpresetdemo/noiseloop");
    m_toneEvent = kwlEventGetHandle("mixpresetdemo/tone");
    m_chirpEvent = kwlEventGetHandle("mixpresetdemo/chirploop");
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"sfx.kwb"]);
    
    kwlEventStart(m_noiseEvent);
    kwlEventStart(m_toneEvent);
    kwlEventStart(m_chirpEvent);
    
}

-(void)deinitialize
{
    kwlEventStop(m_noiseEvent);
    kwlEventStop(m_toneEvent);
    kwlEventStop(m_chirpEvent);
    
    kwlEventRelease(m_noiseEvent);
    kwlEventRelease(m_toneEvent);
    kwlEventRelease(m_chirpEvent);
    
    kwlWaveBankUnload(m_waveBankHandle);
    
    kwlMixPresetSet(m_defaultPreset);
}

-(void)update:(float)timeStep
{
    
}

- (IBAction)presetChanged:(id)sender
{
    UISegmentedControl* s = (UISegmentedControl*)sender;
    int i = s.selectedSegmentIndex;
    if (m_doFade)
    {
        kwlMixPresetFadeTo(m_presetHandles[i]);
    }
    else
    {
        kwlMixPresetSet(m_presetHandles[i]);
    }
}

- (IBAction)fadeSwitchChanged:(id)sender
{
    UISwitch* s = (UISwitch*)sender;
    m_doFade = s.on;
}

@end
