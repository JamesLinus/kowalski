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

#import "SampleClockDemo.h"

@implementation SampleClockDemo

@synthesize beatLabel1;
@synthesize beatLabel2;
@synthesize beatLabel3;
@synthesize beatLabel4;

@synthesize pauseSwitch;

-(NSString*)getName
{
    return @"Sample Clock";
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
    pauseSwitch.selectedSegmentIndex = 0;
    
    m_frameCount = 0;
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"music.kwb"]);
    m_eventHandle = kwlEventGetHandle("music/la_romance_quoi_120bpm_loop");
    kwlEventStartFade(m_eventHandle, 3.0f);
    m_currentBeatIndex = 0;
    m_mixerPaused = false;
}

-(void)deinitialize
{
    kwlMixerResume();
    kwlWaveBankUnload(m_waveBankHandle);
    kwlEventRelease(m_eventHandle);
}

-(void)update:(float)timeStep
{
    m_frameCount += kwlGetNumFramesMixed();
    //printf("m_frameCount %d\n", m_frameCount);
    while (m_frameCount > 44100 / 2)
    {
        m_frameCount -= 44100 / 2;
        m_currentBeatIndex = (m_currentBeatIndex + 1) % 4;
    }
    
    UILabel* labels[4] = 
    {
        beatLabel1,
        beatLabel2,
        beatLabel3,
        beatLabel4
    };
    
    for (int i = 0; i < 4; i++)
    {
        
        UILabel* l = labels[i];
        float alpha = i == m_currentBeatIndex ? 1.0 : 0.6;
        
        if (m_mixerPaused)
        {
            alpha *= 0.3f;
        }
        
        l.backgroundColor = [UIColor colorWithRed:0.0f
                                            green:0.3f
                                             blue:0.6f
                                            alpha:alpha];
    }
}

-(IBAction)onPauseSwitchChanged:(id)sender
{
    UISegmentedControl* s = (UISegmentedControl*)sender;
    
    m_mixerPaused = s.selectedSegmentIndex == 1;
    
    if (m_mixerPaused)
    {   
        kwlMixerPause();
    }
    else
    {
        kwlMixerResume();
    }
}
@end
