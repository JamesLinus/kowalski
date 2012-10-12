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

#import "DecodersDemo.h"

@implementation DecodersDemo

@synthesize pitchSliderMP3, gainSliderMP3, backgroundMP3;
@synthesize pitchSliderAAC, gainSliderAAC, backgroundAAC;
@synthesize pitchSliderIMA4, gainSliderIMA4, backgroundIMA4;
@synthesize pitchSliderVorbis, gainSliderVorbis, backgroundVorbis;
@synthesize pitchSliderPCM, gainSliderPCM, backgroundPCM;


-(NSString*)getName
{
    return @"Decoders";
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
    fadeOutTime = 1.0f;
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"music.kwb"]);

    m_eventHandleAAC = kwlEventGetHandle("music/tonight_thats_allright_loop_aac");
    m_eventHandleMP3 = kwlEventGetHandle("music/tonight_thats_allright_loop_mp3");
    m_eventHandleIMA4 = kwlEventGetHandle("music/tonight_thats_allright_loop_ima4");
    m_eventHandleVorbis = kwlEventGetHandle("music/tonight_thats_allright_loop_ogg");    
    m_eventHandlePCM = kwlEventGetHandle("music/tonight_thats_allright_loop");    
    
    pitchSliderMP3.value = 1.0f;
    gainSliderMP3.value = 1.0f;
    
    pitchSliderAAC.value = 1.0f;
    gainSliderAAC.value = 1.0f;
    
    pitchSliderIMA4.value = 1.0f;
    gainSliderIMA4.value = 1.0f;
    
    pitchSliderVorbis.value = 1.0f;
    gainSliderVorbis.value = 1.0f;
    
    pitchSliderPCM.value = 1.0f;
    gainSliderPCM.value = 1.0f;
}

-(void)deinitialize
{
    kwlEventStop(m_eventHandleMP3);
    kwlEventStop(m_eventHandleAAC);
    kwlEventStop(m_eventHandleIMA4);
    kwlEventStop(m_eventHandleVorbis);
    kwlEventStop(m_eventHandlePCM);
    
    kwlWaveBankUnload(m_waveBankHandle);
    
    m_eventHandleMP3 = KWL_INVALID_HANDLE;
    m_eventHandleAAC = KWL_INVALID_HANDLE;
    m_eventHandleIMA4 = KWL_INVALID_HANDLE;
    m_eventHandleVorbis = KWL_INVALID_HANDLE;
}

-(void)update:(float)timeStep
{
    UIColor* pl = [UIColor colorWithRed:0 
                                  green:0.7f
                                   blue:0 
                                  alpha:0.55f];
    
    UIColor* st = [UIColor colorWithRed:0 
                                  green:0
                                   blue:0 
                                  alpha:0.15f];
    
    backgroundMP3.backgroundColor = kwlEventIsPlaying(m_eventHandleMP3) != 0 ? pl : st;
    backgroundAAC.backgroundColor = kwlEventIsPlaying(m_eventHandleAAC) != 0 ? pl : st;
    backgroundIMA4.backgroundColor = kwlEventIsPlaying(m_eventHandleIMA4) != 0 ? pl : st;
    backgroundVorbis.backgroundColor = kwlEventIsPlaying(m_eventHandleVorbis) != 0 ? pl : st;
    backgroundPCM.backgroundColor = kwlEventIsPlaying(m_eventHandlePCM) != 0 ? pl : st;

}

- (IBAction)pitchSliderMovedMP3:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetPitch(m_eventHandleMP3, s.value);
}

- (IBAction)gainSliderMovedMP3:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetGain(m_eventHandleMP3, s.value);
}

- (IBAction)startMP3:(id)sender
{
    kwlEventStart(m_eventHandleMP3);
}

- (IBAction)pauseMP3:(id)sender
{
    kwlEventPause(m_eventHandleMP3);
}

- (IBAction)resumeMP3:(id)sender
{
    kwlEventResume(m_eventHandleMP3);
}

- (IBAction)stopMP3:(id)sender
{
    kwlEventStopFade(m_eventHandleMP3, fadeOutTime);
}

- (IBAction)pitchSliderMovedAAC:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetPitch(m_eventHandleAAC, s.value);
}

- (IBAction)gainSliderMovedAAC:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetGain(m_eventHandleAAC, s.value);
}

- (IBAction)startAAC:(id)sender
{
    kwlEventStart(m_eventHandleAAC);
}

- (IBAction)pauseAAC:(id)sender
{
    kwlEventPause(m_eventHandleAAC);
}

- (IBAction)resumeAAC:(id)sender
{
    kwlEventResume(m_eventHandleAAC);
}

- (IBAction)stopAAC:(id)sender
{
    kwlEventStopFade(m_eventHandleAAC, fadeOutTime);
}

- (IBAction)pitchSliderMovedIMA4:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetPitch(m_eventHandleIMA4, s.value);
}

- (IBAction)gainSliderMovedIMA4:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetGain(m_eventHandleIMA4, s.value);
}

- (IBAction)startIMA4:(id)sender
{
    kwlEventStart(m_eventHandleIMA4);
}

- (IBAction)pauseIMA4:(id)sender
{
    kwlEventPause(m_eventHandleIMA4);
}

- (IBAction)resumeIMA4:(id)sender
{
    kwlEventResume(m_eventHandleIMA4);
}

- (IBAction)stopIMA4:(id)sender
{
    kwlEventStopFade(m_eventHandleIMA4, fadeOutTime);
}

- (IBAction)pitchSliderMovedVorbis:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetPitch(m_eventHandleVorbis, s.value);
}

- (IBAction)gainSliderMovedVorbis:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetGain(m_eventHandleVorbis, s.value);
}

- (IBAction)startVorbis:(id)sender
{
    kwlEventStart(m_eventHandleVorbis);
}

- (IBAction)pauseVorbis:(id)sender
{
    kwlEventPause(m_eventHandleVorbis);
}

- (IBAction)resumeVorbis:(id)sender
{
    kwlEventResume(m_eventHandleVorbis);
}

- (IBAction)stopVorbis:(id)sender
{
    kwlEventStopFade(m_eventHandleVorbis, fadeOutTime);
}


- (IBAction)pitchSliderMovedPCM:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetPitch(m_eventHandlePCM, s.value);
}

- (IBAction)gainSliderMovedPCM:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetGain(m_eventHandlePCM, s.value);
}

- (IBAction)startPCM:(id)sender
{
    kwlEventStart(m_eventHandlePCM);
}

- (IBAction)pausePCM:(id)sender
{
    kwlEventPause(m_eventHandlePCM);
}

- (IBAction)resumePCM:(id)sender
{
    kwlEventResume(m_eventHandlePCM);
}

- (IBAction)stopPCM:(id)sender
{
    kwlEventStopFade(m_eventHandlePCM, fadeOutTime);
}

@end
