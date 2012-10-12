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

#ifndef KOWALSKI_CAMERA_H
#define KOWALSKI_CAMERA_H

class Camera
{
public:
    Camera();
    void update(float timeStepSec);
    
    float getPos(float* x, float* y, float *z);
    float getFacing(float* x, float* y, float *z);
    float getUp(float* x, float* y, float *z);
    float getVelocity(float* x, float* y, float *z);
    
    void setPosition(float x, float y, float z);
    void setTransform();
    void moveForward(float d);
    void moveUp(float d);
    void steer(float d);
private:
    float m_rollAngle;
    float m_steeringAngle;
    float m_up[3];
    float m_facing[3];
    float m_velocity[3];
    float m_position[3];
    float m_prevPosition[3];
};

#endif //KOWALSKI_CAMERA_H
