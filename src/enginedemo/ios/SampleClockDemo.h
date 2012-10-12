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

@interface SampleClockDemo : DemoBase {
    UILabel* beatLabel1;
    UILabel* beatLabel2;
    UILabel* beatLabel3;
    UILabel* beatLabel4;
    
    UISegmentedControl* pauseSwitch;
    
    kwlEventHandle m_eventHandle;
    kwlWaveBankHandle m_waveBankHandle;
    const float m_bpm;
    int m_currentBeatIndex;
    unsigned int m_frameCount;
    bool m_mixerPaused;
}

-(IBAction)onPauseSwitchChanged:(id)sender;

@property (nonatomic, retain) IBOutlet UILabel *beatLabel1;
@property (nonatomic, retain) IBOutlet UILabel *beatLabel2;
@property (nonatomic, retain) IBOutlet UILabel *beatLabel3;
@property (nonatomic, retain) IBOutlet UILabel *beatLabel4;

@property (nonatomic, retain) IBOutlet UISegmentedControl *pauseSwitch;

@end
