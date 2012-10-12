
#import "PositionalAudioDemoBase.h"
#import "SoundEmitter.h"


@implementation PositionalAudioDemoBase

-(void)update:(float)timeStep
{
    //printf("listenerFacingAngleRad %f\n", listenerFacingAngleRad);
    
    if (listenerView == nil)
    {
        listenerView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"listenericon90degcone.png"]];
        [self addSubview:listenerView];
    }
    
    //listener
    [self positionAndRotateView:listenerView 
                               :listenerPosition[0]
                               :listenerPosition[1]
                               :listenerFacingAngleRad];
    
    kwlListenerSetOrientation(cosf(listenerFacingAngleRad), 
                              sinf(listenerFacingAngleRad), 0, 
                              0, 0, 1);
    
    //emitters
    for (int i = 0; i < [emitters count]; i++)
    {
        SoundEmitter* e = [emitters objectAtIndex:i];
        
        //update emitter
        [e update:timeStep];
        
        //position and rotate the emitter view
        [self positionAndRotateView:e 
                                   :e.xPosition
                                   :e.yPosition
                                   :[e getFacingAngle]];
    }
}

-(void)positionAndRotateView:(UIView*)view
                            :(float)xWorld
                            :(float)yWorld
                            :(float)facingAngle
{
    float x, y;
    
    [self worldToScreen:xWorld
                       :yWorld
                       :&x
                       :&y];
    
    CGAffineTransform rot = CGAffineTransformMakeRotation(-facingAngle);
    
    float w = view.bounds.size.width;
    float h = view.bounds.size.height;
    CGAffineTransform trans = CGAffineTransformMakeTranslation(x - w / 2, 
                                                               y - h / 2);
    CGAffineTransform tot = CGAffineTransformConcat(rot, trans);
    view.transform = tot;    
    
}

-(void)worldToScreen:(float)xWorld
                    :(float)yWorld
                    :(float*)xScreen
                    :(float*)yScreen
{
    float xMin = -20;
    float xMax = 20;
    float yMin = -20;
    float yMax = 20;
    
    CGRect f = self.frame;
    
    *xScreen = (xWorld - xMin) / (xMax - xMin) * f.size.width;
    *yScreen = (yWorld - yMin) / (yMax - yMin) * f.size.width;
    
}

-(void)setListenerPosition:(float)x
                          :(float)y
{
    listenerPosition[0] = x;
    listenerPosition[1] = y;
}

-(void)setListenerFacingAngle:(float)angleInRadians
{
    listenerFacingAngleRad = angleInRadians;
    
}

-(void)addEmitter:(const char*)eventId
                 :(float)x
                 :(float)y
                 :(float)z
                 :(BOOL)oneShot
                 :(BOOL)ninetyDegCone
{
    SoundEmitter* e = [[SoundEmitter alloc] initWithEventID:eventId:oneShot:ninetyDegCone];
    e.xPosition = x;
    e.yPosition = y;
    e.zPosition = z;
    //make emitters look down the negative y direction by default
    [e setFacingAngle:-M_PI / 2];
    [emitters addObject:e];
    [self addSubview:e];
    
    [e release];
}

-(void)addEmitter:(const char*)eventId
                 :(float)x
                 :(float)y
                 :(float)z
                 :(BOOL)oneShot
{
    [self addEmitter:eventId
                    :x
                    :y
                    :z
                    :oneShot
                    :NO];
}

-(void)addEmitter:(const char*)eventId
                 :(float)x
                 :(float)y
                 :(float)z
{
    [self addEmitter:eventId
                    :x
                    :y
                    :z
                    :NO
                    :NO];
}

-(void)initialize
{
    emitters = [NSMutableArray arrayWithCapacity:10];
    [emitters retain];
    
    //make the listener look in the positive y direction
    [self setListenerFacingAngle:M_PI / 2];
}

-(void)deinitialize
{
    for (int i = 0; i < [emitters count]; i++)
    {
        SoundEmitter* e = [emitters objectAtIndex:i];
        [e removeFromSuperview];
    }
    
    [emitters removeAllObjects];
    [emitters release];
    
    [self setListenerPosition:0
                             :0];
    
    kwlSetConeAttenuationEnabled(0, 0);
}

@end
