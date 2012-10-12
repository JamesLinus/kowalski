
#import "DopplerShiftDemo.h"
#import "SoundEmitter.h"

@implementation DopplerShiftDemo

@synthesize angularVelocitySlider, dopplerFactorSlider;

-(NSString*)getName
{
    return @"Doppler Shift";
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
    [super initialize];
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"notes.kwb"]);
    
    [self addEmitter:"dopplerdemo/note_loop"
                    :0
                    :0
                    :0];
     
    for (int i = 0; i < [emitters count]; i++)
    {
        SoundEmitter* e = [emitters objectAtIndex:i];
        [e start];
    }
    
    [self setListenerPosition:0 
                             :15];
    
    m_rotationAngle = 2;
    m_radius = 15;
    
    m_angularVelocity = 1;
    angularVelocitySlider.value = m_angularVelocity;
    [self angularVelocityChanged:angularVelocitySlider];
    
    m_dopplerFactor = 1;
    dopplerFactorSlider.value = m_dopplerFactor;
    [self angularVelocityChanged:dopplerFactorSlider];
    
}

-(void)deinitialize
{
    kwlWaveBankUnload(m_waveBankHandle);
    
    [super deinitialize];
}

-(void)update:(float)timeStep
{
    [super update:timeStep];
    
    //move the sound emitter
    
    m_rotationAngle += m_angularVelocity * timeStep;
    
    //elliptic trajectory
    float x = m_radius * cosf(m_rotationAngle);
    float y = 0.3f * m_radius * sinf(m_rotationAngle);
    
    //assume that we only have one emitter
    SoundEmitter* e = [emitters objectAtIndex:0];
    [e setXPosition:x];
    [e setYPosition:y];
}

-(IBAction)angularVelocityChanged:(id)sender
{
    UISlider* slider = (UISlider*)sender;
    
    m_angularVelocity = slider.value;
    
}

-(IBAction)dopplerFactorChanged:(id)sender
{
    UISlider* slider = (UISlider*)sender;
    
    m_dopplerFactor = slider.value;
    float speedOfSound = 340.0f; //m/s
    kwlSetDopplerShiftParameters(speedOfSound, m_dopplerFactor);
}

@end
