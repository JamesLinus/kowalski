
#import "PositionalAudioDemoBase.h"


@interface EmitterSwarmDemo : PositionalAudioDemoBase 
{
    int m_numRowsAndColumns;
    kwlWaveBankHandle m_waveBankHandle;
}

-(IBAction)onTriggerEvent:(id)sender;

@end
