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

#import "ComplexSoundsDemo.h"

@implementation ComplexSoundsDemo

@synthesize button0;
@synthesize button1;
@synthesize button2;
@synthesize button3;
@synthesize button4;
@synthesize button5;
@synthesize button6;
@synthesize button7;
@synthesize button8;
@synthesize button9;
@synthesize button10;
@synthesize button11;
@synthesize button12;
@synthesize button13;

-(void)awakeFromNib
{
    m_eventIDs[0] = "complexevents/in_random_no_repeat_out";
    m_eventIDs[1] = "complexevents/in_random_out";
    m_eventIDs[2] = "complexevents/in_sequential_out";
    m_eventIDs[3] = "complexevents/random";
    m_eventIDs[4] = "complexevents/random_no_repeat";
    m_eventIDs[5] = "complexevents/sequential";
    m_eventIDs[6] = "complexevents/sequential_no_reset";
    
    m_eventIDs[7] = "complexevents/in_random_no_repeat_out_2";
    m_eventIDs[8] = "complexevents/in_random_out_2";
    m_eventIDs[9] = "complexevents/in_sequential_out_2";
    m_eventIDs[10] = "complexevents/random_2";
    m_eventIDs[11] = "complexevents/random_no_repeat_2";
    m_eventIDs[12] = "complexevents/sequential_2";
    m_eventIDs[13] = "complexevents/sequential_no_reset_2";
    
    m_eventLabels[0] = @"in, random no repeat, out";
    m_eventLabels[1] = @"in, random, out";
    m_eventLabels[2] = @"in, sequential, out";
    m_eventLabels[3] = @"random";
    m_eventLabels[4] = @"random, no, repeat";
    m_eventLabels[5] = @"sequential";
    m_eventLabels[6] = @"sequential, no reset";
    
    m_eventLabels[7] = @"in, random no repeat, out";
    m_eventLabels[8] = @"in, random, out";
    m_eventLabels[9] = @"in, sequential, out";
    m_eventLabels[10] = @"random";
    m_eventLabels[11] = @"random, no, repeat";
    m_eventLabels[12] = @"sequential";
    m_eventLabels[13] = @"sequential, no reset";
    
    buttons[0] = button0;
    buttons[1] = button1;
    buttons[2] = button2;
    buttons[3] = button3;
    buttons[4] = button4;
    buttons[5] = button5;
    buttons[6] = button6;
    buttons[7] = button7;
    buttons[8] = button8;
    buttons[9] = button9;
    buttons[10] = button10;
    buttons[11] = button11;
    buttons[12] = button12;
    buttons[13] = button13;
    
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        [buttons[i] setTitle:m_eventLabels[i]
                    forState:UIControlStateNormal];
    }
    
}

-(NSString*)getName
{
    return @"Complex sounds";
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
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        m_eventHandles[i] = kwlEventGetHandle(m_eventIDs[i]);
        kwlError e = kwlGetError();
        //printf("ev id %s\n", m_eventIDs[i]);
        assert(e == KWL_NO_ERROR);
    }
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"numbers.kwb"]);
}

-(void)deinitialize
{
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        kwlEventRelease(m_eventHandles[i]);
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
    
    kwlWaveBankUnload(m_waveBankHandle);
    
}

-(void)update:(float)timeStep
{
    
}

- (IBAction)buttonUp:(id)sender
{
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        if (sender == buttons[i])
        {
            kwlEventStop(m_eventHandles[i]);
            return;
        }
    }
    
    assert(0);
}

- (IBAction)buttonDown:(id)sender
{
    for (int i = 0; i < COMPLEX_SOUNDS_DEMO_NUM_EVENTS; i++)
    {
        if (sender == buttons[i])
        {
            kwlEventStart(m_eventHandles[i]);
            return;
        }
    }
    
    assert(0);
}

@end
