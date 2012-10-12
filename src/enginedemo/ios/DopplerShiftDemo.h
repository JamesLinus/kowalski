
#import "PositionalAudioDemoBase.h"


@interface DopplerShiftDemo : PositionalAudioDemoBase 
{
    float m_angularVelocity;
    float m_rotationAngle;
    float m_radius;
    float m_dopplerFactor;
    kwlWaveBankHandle m_waveBankHandle;
    
    UISlider* angularVelocitySlider;
    UISlider* dopplerFactorSlider;
}

@property (nonatomic, retain) IBOutlet UISlider* angularVelocitySlider;
@property (nonatomic, retain) IBOutlet UISlider* dopplerFactorSlider;

-(IBAction)angularVelocityChanged:(id)sender;
-(IBAction)dopplerFactorChanged:(id)sender;

@end
