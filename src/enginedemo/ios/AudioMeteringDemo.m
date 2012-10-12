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

#import "AudioMeteringDemo.h"

@implementation AudioMeteringDemo

@synthesize m_meterBarLeft;
@synthesize m_meterBarRight;
@synthesize m_clipIndicator;

-(void)update:(float)timeStep
{
    m_timeSinceLastClip += timeStep;
    
    if (!kwlEventIsPlaying(m_eventHandle))
    {
        kwlEventStart(m_eventHandle);
    }
    
    m_clipIndicatorAlpha -= timeStep;
    if (m_clipIndicatorAlpha < 0.0f)
    {
        m_clipIndicatorAlpha = 0.0f;
    }
    
    if (kwlHasClipped())
    {
        m_clipIndicatorAlpha = 1.0f;
    }
    
    
    m_clipIndicator.alpha = m_clipIndicatorAlpha;
    
    m_meterBarLeft.progress = kwlGetLevelLeft();
    m_meterBarRight.progress = kwlGetLevelRight();
}

-(NSString*)getName
{
    return @"Audio metering";
}

-(void)initialize
{
    m_clipIndicatorAlpha = 0.0f;
    m_timeSinceLastClip = 0;
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"music.kwb"]);
    m_eventHandle = kwlEventGetHandle("music/music_stereo_ogg");
    m_timeSinceLastClip = 100000;
    kwlEventStart(m_eventHandle);
    kwlLevelMeteringSetEnabled(1);
}

-(void)deinitialize
{
    kwlWaveBankUnload(m_waveBankHandle);
    kwlEventRelease(m_eventHandle);
    kwlLevelMeteringSetEnabled(0);
}


@end
