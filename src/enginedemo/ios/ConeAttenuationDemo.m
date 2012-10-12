
#import "ConeAttenuationDemo.h"
#import "SoundEmitter.h"

@implementation ConeAttenuationDemo

@synthesize eventAngularVelocitySlider, listenerAngularVelocitySlider;

-(NSString*)getName
{
    return @"Cone Attenuation";
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
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"sfx.kwb"]);
    
    //the coneattenuationdemo/tone event has inner/outer cone angles of 60/140.
    [self addEmitter:"coneattenuationdemo/tone"
                    :0
                    :0
                    :0
                    :NO
                    :YES];
     
    for (int i = 0; i < [emitters count]; i++)
    {
        SoundEmitter* e = [emitters objectAtIndex:i];
        [e start];
    }
    
    [self setListenerPosition:0 
                             :8];
    
    //set listener cone angles to match the icon image.
    kwlListenerSetConeParameters(90.0f, 180.0f, 0.1f);
    kwlSetConeAttenuationEnabled(1, 1);
    eventAngularVelocitySlider.value = 0.3f;    
    listenerAngularVelocitySlider.value = 0.0f;
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
    m_eventFacingAngle += eventAngularVelocitySlider.value * timeStep;
    m_listenerFacingAngle += listenerAngularVelocitySlider.value * timeStep;
    
    //assume that we only have one emitter
    SoundEmitter* e = [emitters objectAtIndex:0];
    e.facingAngle = m_eventFacingAngle;
    
    [self setListenerFacingAngle:m_listenerFacingAngle];
}

@end
