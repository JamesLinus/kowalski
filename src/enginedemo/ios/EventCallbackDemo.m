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

#import "EventCallbackDemo.h"
#import "kowalski_ext.h"

@implementation EventCallbackDemo

@synthesize oneshotStoppedIndicator, instanceStoppedIndicator;

-(NSString*)getName
{
    return @"Event callbacks";
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
    oneshotStoppedIndicator.alpha = 0.0f;
    instanceStoppedIndicator.alpha = 0.0f;
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"sfx.kwb"]);
    m_eventDefinitionHandle = kwlEventDefinitionGetHandle("oneshotdemo/square_fade_out");
    m_eventInstanceHandle = kwlEventGetHandle("distanceattenuationdemo/sine_fade_out");
    kwlEventSetCallback(m_eventInstanceHandle, instanceStopped, self);
}

-(void)deinitialize
{
    kwlEventStop(m_eventInstanceHandle);
    kwlWaveBankUnload(m_waveBankHandle);
    m_eventInstanceHandle = KWL_INVALID_HANDLE;
    m_eventDefinitionHandle = KWL_INVALID_HANDLE;
    m_waveBankHandle = KWL_INVALID_HANDLE;
}

-(void)update:(float)timeStep
{
    float deltaAlpha = timeStep;
    
    oneshotStoppedIndicator.alpha -= deltaAlpha;
    instanceStoppedIndicator.alpha -= deltaAlpha;
}

void oneshotStopped(void* data)
{
    EventCallbackDemo* demo = (EventCallbackDemo*)data;
    [demo onOneshotStopped];
}

void instanceStopped(void* data)
{
    EventCallbackDemo* demo = (EventCallbackDemo*)data;
    [demo onInstanceStopped];
}

-(void)onOneshotStopped
{
    oneshotStoppedIndicator.alpha = 1.0f;
}

-(void)onInstanceStopped
{
    instanceStoppedIndicator.alpha = 1.0f;
}

- (IBAction)playEventInstance:(id)sender
{
    kwlEventStart(m_eventInstanceHandle);
}

- (IBAction)stopEventInstance:(id)sender
{
    kwlEventStop(m_eventInstanceHandle);
}

- (IBAction)playOneShotEvent:(id)sender
{
    kwlEventStartOneShotWithCallbackAt(m_eventDefinitionHandle, 0, 0, 0, oneshotStopped, self);
}

@end
