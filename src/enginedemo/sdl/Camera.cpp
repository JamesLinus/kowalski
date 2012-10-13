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

#include "Camera.h"
#include "kowalski.h"
#include "math.h"
#include "SDL/SDL_opengl.h"

Camera::Camera() :
    m_rollAngle(0),
    m_steeringAngle(3.14f)
{
    m_up[0] = 0;
    m_up[1] = 1;
    m_up[2] = 0;
    
    m_velocity[0] = 0.0f;
    m_velocity[1] = 0.0f;
    m_velocity[2] = 0.0f;
    
    m_position[0] = m_prevPosition[0] = 0;
    m_position[1] = m_prevPosition[1] = 2;
    m_position[2] = m_prevPosition[2] = 25;
    
    m_facing[0] = sinf(m_steeringAngle);
    m_facing[1] = 0.0f;
    m_facing[2] = cosf(m_steeringAngle);
}

void Camera::update(float timeStep)
{
    if (timeStep != 0)
    {
        m_velocity[0] = (m_position[0] - m_prevPosition[0]) / timeStep;
        m_velocity[1] = (m_position[1] - m_prevPosition[1]) / timeStep;
        m_velocity[2] = (m_position[2] - m_prevPosition[2]) / timeStep;
        
        kwlListenerSetVelocity(m_velocity[0], m_velocity[1], m_velocity[2]);
    } 
    
    kwlListenerSetPosition(m_position[0], m_position[1], m_position[2]);
    kwlListenerSetOrientation(m_facing[0], m_facing[1], m_facing[2],
                              m_up[0], m_up[1], m_up[2]);
    
    m_prevPosition[0] = m_position[0];
    m_prevPosition[1] = m_position[1];
    m_prevPosition[2] = m_position[2];
}

void Camera::moveUp(float d)
{
    m_position[1] += d;
}

void Camera::setPosition(float x, float y, float z)
{
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void Camera::moveForward(float d)
{
    m_position[0] += d * m_facing[0];
    m_position[2] += d * m_facing[2];
}

void Camera::steer(float d)
{
    m_steeringAngle += d;
    m_facing[0] = sinf(m_steeringAngle);
    m_facing[2] =  cosf(m_steeringAngle);
}

void Camera::setTransform()
{
    gluLookAt(m_position[0], m_position[1], m_position[2], 
              m_position[0] + m_facing[0], m_position[1] + m_facing[1], m_position[2] + m_facing[2],
              m_up[0], m_up[1], m_up[2]);
}
