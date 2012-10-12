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

#import "BalanceAndPanDemo.h"

@implementation BalanceAndPanDemo

@synthesize panSlider;
@synthesize m_meterBarLeft;
@synthesize m_meterBarRight;

-(NSString*)getName
{
    return @"Balance and pan";
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
    panSlider.value = 0.0f;
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"sfx.kwb"]);
    m_eventHandle = kwlEventGetHandle("balancedemo/mono_and_stereo");
    kwlEventStartFade(m_eventHandle, 1.0f);
    kwlEventSetBalance(m_eventHandle, 0.0f);
    kwlLevelMeteringSetEnabled(1);
}

-(void)deinitialize
{
    kwlEventStop(m_eventHandle);
    kwlEventRelease(m_eventHandle);
    kwlWaveBankUnload(m_waveBankHandle);
    m_eventHandle = KWL_INVALID_HANDLE;
    m_waveBankHandle = KWL_INVALID_HANDLE;
    kwlLevelMeteringSetEnabled(0);
}

-(void)update:(float)timeStep
{
    m_meterBarLeft.progress = kwlGetLevelLeft();
    m_meterBarRight.progress = kwlGetLevelRight();
}

- (IBAction)panChanged:(id)sender
{
    UISlider* s = (UISlider*)sender;
    kwlEventSetBalance(m_eventHandle, s.value);
}


@end
