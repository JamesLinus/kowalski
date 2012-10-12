
#import "EmitterSwarmDemo.h"
#import "SoundEmitter.h"


@implementation EmitterSwarmDemo

-(NSString*)getName
{
    return @"One-shot Playback";
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
    
    //create emitters using the same event definition and
    //place them on an N by N grid
    m_numRowsAndColumns = 4;
    
    
    float spacing = 8;
    float min = -0.5f * (m_numRowsAndColumns - 1) * spacing;
    for (int i = 0; i < m_numRowsAndColumns; i++)
    {
        float x = min + i * spacing;
        for (int j = 0; j < m_numRowsAndColumns; j++)
        {
            float z = 5 + min + j * spacing;
            [self addEmitter:"oneshotdemo/square_fade_out"
                            :x
                            :z
                            :0
                            :YES];
        }
    }
    
    [self setListenerPosition:0
                             :24];
    
    m_waveBankHandle = kwlWaveBankLoad([DemoBase getResourcePath:@"sfx.kwb"]);
    
}

-(void)deinitialize
{
    [super deinitialize];
    kwlWaveBankUnload(m_waveBankHandle);
}

-(void)update:(float)timeStep
{
    [super update:timeStep];
}

-(IBAction)onTriggerEvent:(id)sender
{
    for (int i = 0; i < [emitters count]; i++)
    {
        [[emitters objectAtIndex:i] setLastTriggered:NO];
    }
    
    int idx = arc4random() % [emitters count];
    SoundEmitter* e = [emitters objectAtIndex:idx];
    [e setLastTriggered:YES];
    [e start];

}

@end
