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

#define NUM_PRESETS 3

@interface MixPresetDemo : DemoBase {
    kwlMixPresetHandle m_defaultPreset;
    kwlEventHandle m_noiseEvent;
    kwlEventHandle m_toneEvent;
    kwlEventHandle m_chirpEvent;
    int m_activePresetIndex;
    kwlMixPresetHandle m_presetHandles[NUM_PRESETS];
    const char* m_presetIDs[NUM_PRESETS];
    bool m_doFade;
    kwlWaveBankHandle m_waveBankHandle;
    
    UISegmentedControl *presetSwitch;
    UISwitch *fadeSwitch;
}

@property (nonatomic, retain) IBOutlet UISegmentedControl *presetSwitch;
@property (nonatomic, retain) IBOutlet UISwitch *fadeSwitch;

- (IBAction)presetChanged:(id)sender;
- (IBAction)fadeSwitchChanged:(id)sender;

@end
