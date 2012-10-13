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

#include "kwl_positionalaudiolistener.h"
#include "kwl_memory.h"

void kwlPositionalAudioListener_setDefaults(kwlPositionalAudioListener* listener)
{
    kwlMemset(listener, 0, sizeof(kwlPositionalAudioListener));
    listener->innerConeCosAngle = 1.0f;
    listener->outerConeCosAngle = -1.0f;
    listener->outerConeGain = 1.0f;
    
    listener->upX = 0.0f;
    listener->upY = 1.0f;
    listener->upZ = 0.0f;
    
    listener->directionX = 0.0f;
    listener->directionY = 0.0f;
    listener->directionZ = -1.0f;
    
    listener->velocityX = 0.0f;
    listener->velocityY = 0.0f;
    listener->velocityZ = 0.0f;
    
    listener->positionX = 0.0f;
    listener->positionY = 0.0f;
    listener->positionZ = 0.0f;
    
    listener->rightX = 1.0f;
    listener->rightY = 0.0f;
    listener->rightZ = 0.0f;
}
