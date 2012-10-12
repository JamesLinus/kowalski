
#import "SoundEmitter.h"


@implementation SoundEmitter

@synthesize xPosition;
@synthesize yPosition;
@synthesize zPosition;

-(id)initWithEventID:(const char*)eventId:(BOOL)oneShot:(BOOL)ninetyDegCone
{
    self = [super initWithImage:[UIImage imageNamed:ninetyDegCone ? @"emittericon90degcone.png" : @"emittericon.png"]];
    if (oneShot)
    {
        eventDefinitionHandle = kwlEventDefinitionGetHandle(eventId);
    }
    else
    {
        eventHandle = kwlEventGetHandle(eventId);
    }
    
    isOneShot = oneShot;
    
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR);
    
    return self;
}

-(void)dealloc
{
    
    kwlEventRelease(eventHandle);
    
    [super dealloc];
}

-(void)update:(float)timeStep
{
    if (!isOneShot)
    {
        //compute a velocity for this update
        if (timeStep != 0)
        {
            xVelocity = (xPosition - xPositionPrev) / timeStep;
            yVelocity = (yPosition - yPositionPrev) / timeStep;
            zVelocity = (zPosition - zPositionPrev) / timeStep;
        }
        
        xPositionPrev = xPosition;
        yPositionPrev = yPosition;
        zPositionPrev = zPosition;
        
        //set event position and velocity
        kwlEventSetPosition(eventHandle, xPosition, yPosition, zPosition);
        kwlEventSetVelocity(eventHandle, xVelocity, yVelocity, zVelocity);
    }
    
    if (isOneShot)
    {
        self.alpha = lastTriggered ? 1.0f : 0.5f;
    }
    else
    {
        self.alpha = [self isPlaying] ? 1.0f : 0.3f;
    }
}

-(void)start
{
    if (isOneShot)
    {
        kwlEventStartOneShotAt(eventDefinitionHandle, xPosition, yPosition, zPosition);
    }
    else
    {
        kwlEventStart(eventHandle);
    }
}

-(float)getFacingAngle
{
    return facingAngle;
}

-(void)setFacingAngle:(float)rad
{
    facingAngle = rad;
    if (!isOneShot)
    {
        kwlEventSetOrientation(eventHandle, 
                               cosf(rad), 
                               sinf(rad), 
                               0);
    }
}

-(BOOL)isPlaying
{
    return isOneShot ? NO : kwlEventIsPlaying(eventHandle) != 0;
}

-(void)setLastTriggered:(BOOL)val
{
    lastTriggered = val;
}

@end
