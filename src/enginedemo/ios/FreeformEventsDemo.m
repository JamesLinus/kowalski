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

#import "FreeformEventsDemo.h"

#import "kowalski.h"

@implementation FreeformEventsDemo

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

-(void)awakeFromNib
{
    m_fileNames[0] = @"sine_signed_8bit_mono.au";
    m_fileNames[1] = @"sine_signed_16bit_mono.au";
    m_fileNames[2] = @"sine_signed_24bit_mono.au";
    m_fileNames[3] = @"sine_signed_32bit_mono.au";
        
    m_fileNames[4] = @"sine_signed_8bit_mono.aiff";
    m_fileNames[5] = @"sine_signed_16bit_mono.aiff";
    m_fileNames[6] = @"sine_signed_24bit_mono.aiff";
    m_fileNames[7] = @"sine_signed_32bit_mono.aiff";
        
    m_fileNames[8] = @"sine_unsigned_8bit_mono.wav";
    m_fileNames[9] = @"sine_signed_16bit_mono.wav";
    m_fileNames[10] = @"sine_signed_24bit_mono.wav";
    m_fileNames[11] = @"sine_signed_32bit_mono.wav";
        
    m_fileNames[12] = @"procedurally generated sine";
    
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
    
    for (int i = 0; i < NUM_EVENTS; i++)
    {
        [buttons[i] setTitle:m_fileNames[i]
                    forState:UIControlStateNormal];
    }
    
}

-(NSString*)getName
{
    return @"Freeform Events";
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
    for (int i = 0; i < NUM_EVENTS; i++)
    {
        if (i < NUM_EVENTS - 1)
        {
            NSString* fileName = [@"freeformeventaudio" stringByAppendingPathComponent:m_fileNames[i]];
            m_eventHandles[i] = kwlEventCreateWithFile([DemoBase getResourcePath:fileName], KWL_POSITIONAL, 0);
        }
        else
        {
            assert(m_buffer == NULL);
            /*create a procedurally generated mono sine event in the last slot*/
            int numFrames = 44100;
            float f = 50;
            m_buffer = (short*)malloc(numFrames * sizeof(short));
            for (int j = 0; j < numFrames; j++)
            {
                float envelope = (numFrames - j) / (float)numFrames;
                m_buffer[j] = (short)(32767 * envelope * envelope * sinf(f * j));
            }
            
            kwlPCMBuffer buffer;
            buffer.numChannels = 1;
            buffer.numFrames = numFrames;
            buffer.pcmData = m_buffer;
            
            m_eventHandles[i] = 
            kwlEventCreateWithBuffer(&buffer, KWL_POSITIONAL);
        }
    }
}

-(void)deinitialize
{
    for (int i = 0; i < NUM_EVENTS; i++)
    {
        kwlEventRelease(m_eventHandles[i]);
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
    
    free(m_buffer);
    m_buffer = NULL;
}

-(void)update:(float)timeStep
{
    
}

- (IBAction)buttonPressed:(id)sender
{
    for (int i = 0; i < NUM_EVENTS; i++)
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
