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

#import "PianoDemo.h"


@implementation PianoDemo

@synthesize key0;
@synthesize key1;
@synthesize key2;
@synthesize key3;
@synthesize key4;
@synthesize key5;
@synthesize key6;
@synthesize key7;
@synthesize key8;
@synthesize key9;
@synthesize key10;
@synthesize key11;
@synthesize key12;

@synthesize pitchSlider;

-(NSString*)getName
{
    return @"Piano";
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
    m_keyEventHandles[0] = kwlEventGetHandle("pianodemo/c-1");
    m_keyEventHandles[1] = kwlEventGetHandle("pianodemo/c#-1");
    m_keyEventHandles[2] = kwlEventGetHandle("pianodemo/d-1");
    m_keyEventHandles[3] = kwlEventGetHandle("pianodemo/d#-1");
    m_keyEventHandles[4] = kwlEventGetHandle("pianodemo/e-1");
    m_keyEventHandles[5] = kwlEventGetHandle("pianodemo/f-1");
    m_keyEventHandles[6] = kwlEventGetHandle("pianodemo/f#-1");
    m_keyEventHandles[7] = kwlEventGetHandle("pianodemo/g-1");
    m_keyEventHandles[8] = kwlEventGetHandle("pianodemo/g#-1");
    m_keyEventHandles[9] = kwlEventGetHandle("pianodemo/a-1");
    m_keyEventHandles[10] = kwlEventGetHandle("pianodemo/b-1");
    m_keyEventHandles[11] = kwlEventGetHandle("pianodemo/h-1");
    m_keyEventHandles[12] = kwlEventGetHandle("pianodemo/c-2");
    
    m_currentPitch = 1.0f;
    pitchSlider.value = m_currentPitch;
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"notes.kwb"]);
    
    m_notesMixBusHandle = kwlMixBusGetHandle("notes");
}

-(void)deinitialize
{
    for (int i = 0; i < 13; i++)
    {
        //TODO: return engine data not loaded if that is the case
        kwlEventRelease(m_keyEventHandles[i]);
    }
    kwlWaveBankUnload(m_waveBankHandle);
}

-(void)update:(float)timeStep
{
    
}

- (IBAction)keyPressed:(id)sender
{
    int idx = -1;
    
    if (sender == key0)
        idx = 0;
    else if (sender == key1)
        idx = 1;
    else if (sender == key2)
        idx = 2;
    else if (sender == key3)
        idx = 3;
    else if (sender == key4)
        idx = 4;
    else if (sender == key5)
        idx = 5;
    else if (sender == key6)
        idx = 6;
    else if (sender == key7)
        idx = 7;
    else if (sender == key8)
        idx = 8;
    else if (sender == key9)
        idx = 9;
    else if (sender == key10)
        idx = 10;
    else if (sender == key11)
        idx = 11;
    else if (sender == key12)
        idx = 12;
    
    if (idx >= 0)
    {
        kwlEventStart(m_keyEventHandles[idx]);
    }
}

- (IBAction)pitchSliderMoved:(id)sender
{
    m_currentPitch = ((UISlider*)sender).value;
    
    kwlMixBusSetPitch(m_notesMixBusHandle, m_currentPitch);
}

- (IBAction)volumeSliderMoved:(id)sender
{
    m_currentPitch = ((UISlider*)sender).value;
    
    kwlMixBusSetPitch(m_notesMixBusHandle, m_currentPitch);
}


@end
