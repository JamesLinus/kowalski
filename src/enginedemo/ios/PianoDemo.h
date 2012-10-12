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


@interface PianoDemo : DemoBase {
    UIButton* key0;
    UIButton* key1;
    UIButton* key2;
    UIButton* key3;
    UIButton* key4;
    UIButton* key5;
    UIButton* key6;
    UIButton* key7;
    UIButton* key8;
    UIButton* key9;
    UIButton* key10;
    UIButton* key11;
    UIButton* key12;
    
    UISlider* pitchSlider;
    
    kwlEventHandle m_keyEventHandles[13];
    kwlWaveBankHandle m_waveBankHandle;
    kwlMixBusHandle m_notesMixBusHandle;
    float m_currentPitch;
}

- (IBAction)keyPressed:(id)sender;
- (IBAction)pitchSliderMoved:(id)sender;

@property (nonatomic, retain) IBOutlet UIButton *key0;
@property (nonatomic, retain) IBOutlet UIButton *key1;
@property (nonatomic, retain) IBOutlet UIButton *key2;
@property (nonatomic, retain) IBOutlet UIButton *key3;
@property (nonatomic, retain) IBOutlet UIButton *key4;
@property (nonatomic, retain) IBOutlet UIButton *key5;
@property (nonatomic, retain) IBOutlet UIButton *key6;
@property (nonatomic, retain) IBOutlet UIButton *key7;
@property (nonatomic, retain) IBOutlet UIButton *key8;
@property (nonatomic, retain) IBOutlet UIButton *key9;
@property (nonatomic, retain) IBOutlet UIButton *key10;
@property (nonatomic, retain) IBOutlet UIButton *key11;
@property (nonatomic, retain) IBOutlet UIButton *key12;

@property (nonatomic, retain) IBOutlet UISlider *pitchSlider;

@end
