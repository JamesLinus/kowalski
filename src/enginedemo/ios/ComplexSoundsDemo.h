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

#import "DemoBase.h"

#define COMPLEX_SOUNDS_DEMO_NUM_EVENTS 14

@interface ComplexSoundsDemo : DemoBase 
{
    kwlEventHandle m_eventHandles[COMPLEX_SOUNDS_DEMO_NUM_EVENTS];
    kwlWaveBankHandle m_waveBankHandle;
    const char* m_eventIDs[COMPLEX_SOUNDS_DEMO_NUM_EVENTS];
    NSString* m_eventLabels[COMPLEX_SOUNDS_DEMO_NUM_EVENTS];
    
    UIButton* buttons[COMPLEX_SOUNDS_DEMO_NUM_EVENTS];
    
    UIButton* button0;
    UIButton* button1;
    UIButton* button2;
    UIButton* button3;
    UIButton* button4;
    UIButton* button5;
    UIButton* button6;
    UIButton* button7;
    UIButton* button8;
    UIButton* button9;
    UIButton* button10;
    UIButton* button11;
    UIButton* button12;
    UIButton* button13;
}

- (IBAction)buttonDown:(id)sender;
- (IBAction)buttonUp:(id)sender;

@property (nonatomic, retain) IBOutlet UIButton *button0;
@property (nonatomic, retain) IBOutlet UIButton *button1;
@property (nonatomic, retain) IBOutlet UIButton *button2;
@property (nonatomic, retain) IBOutlet UIButton *button3;
@property (nonatomic, retain) IBOutlet UIButton *button4;
@property (nonatomic, retain) IBOutlet UIButton *button5;
@property (nonatomic, retain) IBOutlet UIButton *button6;
@property (nonatomic, retain) IBOutlet UIButton *button7;
@property (nonatomic, retain) IBOutlet UIButton *button8;
@property (nonatomic, retain) IBOutlet UIButton *button9;
@property (nonatomic, retain) IBOutlet UIButton *button10;
@property (nonatomic, retain) IBOutlet UIButton *button11;
@property (nonatomic, retain) IBOutlet UIButton *button12;
@property (nonatomic, retain) IBOutlet UIButton *button13;

@end
