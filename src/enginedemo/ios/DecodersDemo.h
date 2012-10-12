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


@interface DecodersDemo : DemoBase {
    kwlEventHandle m_eventHandleMP3;
    kwlEventHandle m_eventHandleAAC;
    kwlEventHandle m_eventHandleIMA4;
    kwlEventHandle m_eventHandleVorbis;
    kwlEventHandle m_eventHandlePCM;
    
    kwlWaveBankHandle m_waveBankHandle;
    
    UISlider *pitchSliderMP3;
    UISlider *gainSliderMP3;
    UIView *backgroundMP3;
    
    UISlider *pitchSliderAAC;
    UISlider *gainSliderAAC;
    UIView *backgroundAAC;
    
    UISlider *pitchSliderIMA4;
    UISlider *gainSliderIMA4;
    UIView *backgroundIMA4;
    
    UISlider *pitchSliderVorbis;
    UISlider *gainSliderVorbis;
    UIView *backgroundVorbis;
    
    UISlider *pitchSliderPCM;
    UISlider *gainSliderPCM;
    UIView *backgroundPCM;
    
    float fadeOutTime;
}

- (IBAction)pitchSliderMovedMP3:(id)sender;
- (IBAction)gainSliderMovedMP3:(id)sender;
- (IBAction)startMP3:(id)sender;
- (IBAction)pauseMP3:(id)sender;
- (IBAction)resumeMP3:(id)sender;
- (IBAction)stopMP3:(id)sender;

- (IBAction)pitchSliderMovedAAC:(id)sender;
- (IBAction)gainSliderMovedAAC:(id)sender;
- (IBAction)startAAC:(id)sender;
- (IBAction)pauseAAC:(id)sender;
- (IBAction)resumeAAC:(id)sender;
- (IBAction)stopAAC:(id)sender;

- (IBAction)pitchSliderMovedIMA4:(id)sender;
- (IBAction)gainSliderMovedIMA4:(id)sender;
- (IBAction)startIMA4:(id)sender;
- (IBAction)pauseIMA4:(id)sender;
- (IBAction)resumeIMA4:(id)sender;
- (IBAction)stopIMA4:(id)sender;

- (IBAction)pitchSliderMovedVorbis:(id)sender;
- (IBAction)gainSliderMovedVorbis:(id)sender;
- (IBAction)startVorbis:(id)sender;
- (IBAction)pauseVorbis:(id)sender;
- (IBAction)resumeVorbis:(id)sender;
- (IBAction)stopVorbis:(id)sender;

- (IBAction)pitchSliderMovedPCM:(id)sender;
- (IBAction)gainSliderMovedPCM:(id)sender;
- (IBAction)startPCM:(id)sender;
- (IBAction)pausePCM:(id)sender;
- (IBAction)resumePCM:(id)sender;
- (IBAction)stopPCM:(id)sender;


@property (nonatomic, retain) IBOutlet UISlider *pitchSliderMP3;
@property (nonatomic, retain) IBOutlet UISlider *gainSliderMP3;
@property (nonatomic, retain) IBOutlet UIView *backgroundMP3;

@property (nonatomic, retain) IBOutlet UISlider *pitchSliderAAC;
@property (nonatomic, retain) IBOutlet UISlider *gainSliderAAC;
@property (nonatomic, retain) IBOutlet UIView *backgroundAAC;

@property (nonatomic, retain) IBOutlet UISlider *pitchSliderIMA4;
@property (nonatomic, retain) IBOutlet UISlider *gainSliderIMA4;
@property (nonatomic, retain) IBOutlet UIView *backgroundIMA4;

@property (nonatomic, retain) IBOutlet UISlider *pitchSliderVorbis;
@property (nonatomic, retain) IBOutlet UISlider *gainSliderVorbis;
@property (nonatomic, retain) IBOutlet UIView *backgroundVorbis;

@property (nonatomic, retain) IBOutlet UISlider *pitchSliderPCM;
@property (nonatomic, retain) IBOutlet UISlider *gainSliderPCM;
@property (nonatomic, retain) IBOutlet UIView *backgroundPCM;

@end
