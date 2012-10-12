
#import "PositionalAudioDemoBase.h"


@interface ConeAttenuationDemo : PositionalAudioDemoBase 
{
    float m_eventFacingAngle;
    float m_listenerFacingAngle;
    
    kwlWaveBankHandle m_waveBankHandle;
    
    UISlider* eventAngularVelocitySlider;
    UISlider* listenerAngularVelocitySlider;
}

@property (nonatomic, retain) IBOutlet UISlider* eventAngularVelocitySlider;
@property (nonatomic, retain) IBOutlet UISlider* listenerAngularVelocitySlider;

@end
