#import "DemoBase.h"


@interface PositionalAudioDemoBase : DemoBase 
{
    NSMutableArray* emitters;
    
    UIImageView* listenerView;
    float listenerPosition[2];
    float listenerFacingAngleRad;
}

-(void)positionAndRotateView:(UIView*)view
                            :(float)xWorld
                            :(float)yWorld
                            :(float)facingAngle;

-(void)worldToScreen:(float)xWorld
                    :(float)yWorld
                    :(float*)xScreen
                    :(float*)yScreen;

-(void)setListenerPosition:(float)x
                          :(float)y;

-(void)setListenerFacingAngle:(float)angleInRadians;

-(void)addEmitter:(const char*)eventId
                 :(float)x
                 :(float)y
                 :(float)z;

-(void)addEmitter:(const char*)eventId
                 :(float)x
                 :(float)y
                 :(float)z
                 :(BOOL)oneShot;

-(void)addEmitter:(const char*)eventId
                 :(float)x
                 :(float)y
                 :(float)z
                 :(BOOL)oneShot
                 :(BOOL)ninetyDegCone;

@end
