#import <UIKit/UIKit.h>

#import "kowalski.h"

@interface SoundEmitter : UIImageView 
{
    kwlEventHandle eventHandle;
    kwlEventDefinitionHandle eventDefinitionHandle;
    float xPosition;
    float yPosition;
    float zPosition;
    
    float xPositionPrev;
    float yPositionPrev;
    float zPositionPrev;
    
    float xVelocity;
    float yVelocity;
    float zVelocity;
    
    float facingAngle;
    BOOL lastTriggered;
    BOOL isOneShot;
}

@property float xPosition;
@property float yPosition;
@property float zPosition;

-(id)initWithEventID:(const char*)eventId:(BOOL)oneShot:(BOOL)ninetyDegCone;

-(void)update:(float)timeStep;

-(void)start;

-(float)getFacingAngle;

-(void)setFacingAngle:(float)rad;

-(BOOL)isPlaying;

-(void)setLastTriggered:(BOOL)val;

@end
