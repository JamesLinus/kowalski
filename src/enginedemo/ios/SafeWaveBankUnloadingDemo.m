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

#import "SafeWaveBankUnloadingDemo.h"

@implementation SafeWaveBankUnloadingDemo

@synthesize eventStatusLabel;
@synthesize waveBankStatusLabel;

-(NSString*)getName
{
    return @"Safe wave bank unloading";
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
    m_eventHandle = kwlEventGetHandle("pianodemo/c-1");
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"notes.kwb"]);
}

-(void)deinitialize
{
    kwlEventRelease(m_eventHandle);
    m_eventHandle = KWL_INVALID_HANDLE;
    kwlWaveBankUnload(m_waveBankHandle);
}

-(void)update:(float)timeStep
{
    if (kwlWaveBankIsLoaded(m_waveBankHandle))
    {
        eventStatusLabel.text = @"Wave bank is loaded";
    }
    else
    {
        eventStatusLabel.text = @"Wave bank is not loaded";
    }
    
    if (kwlEventIsPlaying(m_eventHandle))
    {
        eventStatusLabel.text = @"Event is playing.";
    }
    else
    {
        eventStatusLabel.text = @"Event stopped.";
    }
}

- (IBAction)startEvent:(id)sender
{
    kwlEventStart(m_eventHandle);
}

- (IBAction)loadWaveBank:(id)sender
{
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"notes.kwb"]);
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR);
}

- (IBAction)unloadWaveBank:(id)sender
{
    kwlWaveBankUnload(m_waveBankHandle);
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR);
}

@end
