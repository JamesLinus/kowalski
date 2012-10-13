/*
Copyright (c) 2010-2012 Per Gantelius

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#ifndef KWL__LISTENER_H
#define KWL__LISTENER_H

/*! \file */ 

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** A struct containing information about a listener in 3D space.*/
typedef struct kwlPositionalAudioListener
{
    /** The x component of the position of the listener.*/
    float positionX;
    /** The y component of the position of the listener.*/
    float positionY;
    /** The z component of the position of the listener.*/
    float positionZ;
    /** The x component of the velocity of the listener.*/
    float velocityX;
    /** The y component of the velocity of the listener.*/
    float velocityY;
    /** The z component of the velocity of the listener.*/
    float velocityZ;
    /** The x component of the unit vector defining the listener's up direction.*/
    float upX;
    /** The y component of the unit vector defining the listener's up direction.*/
    float upY;
    /** The z component of the unit vector defining the listener's up direction.*/
    float upZ;
    /** The x component of the facing direction of the listener.*/
    float directionX;
    /** The y component of the facing direction of the listener.*/
    float directionY;
    /** The z component of the facing direction of the listener.*/
    float directionZ;
    /** The x component of the unit vector defining the listener's right hand direction.*/
    float rightX;
    /** The x component of the unit vector defining the listener's right hand direction.*/
    float rightY;
    /** The z component of the unit vector defining the listener's right hand direction.*/
    float rightZ;
    /** The cosine of the inner directional cone angle. */
    float innerConeCosAngle;
    /** The cosine of the outer directional cone angle. */
    float outerConeCosAngle;
    /** The gain in directions outside the outer directional cone. */
    float outerConeGain;
} kwlPositionalAudioListener;
    
void kwlPositionalAudioListener_setDefaults(kwlPositionalAudioListener* listener);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__LISTENER_H*/
