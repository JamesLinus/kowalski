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
#include "BitmapFont.h"
#include "SoundEmitter.h"

#include "assert.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"

#include <SDL/SDL_opengl.h>

SoundEmitter::SoundEmitter():
    m_position(),
    m_prevPosition(),
    m_velocity(),
    m_orientation(),
    m_eventId(""),
    m_oneShot(false),
    m_eventHandle(KWL_INVALID_HANDLE),
    m_eventDefinitionHandle(KWL_INVALID_HANDLE),
    m_lastTriggeredOneShot(false),
    m_angularVelocity(0.0f),
    m_orientationAngle(0.0f),
    m_drawCone(false)
{
    //
}

SoundEmitter::SoundEmitter(const char* evId, float x, float y, float z, bool oneShot) :
    m_position(),
    m_prevPosition(),
    m_velocity(),
    m_orientation(),
    m_eventId(evId),
    m_oneShot(oneShot),
    m_eventHandle(KWL_INVALID_HANDLE),
    m_eventDefinitionHandle(KWL_INVALID_HANDLE),
    m_lastTriggeredOneShot(false),
    m_angularVelocity(0.0f),
    m_orientationAngle(0.0f),
    m_drawCone(false)
{
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR);
    if (m_oneShot)
    {
        m_eventDefinitionHandle = kwlEventDefinitionGetHandle(m_eventId);
    }
    else
    {
        m_eventHandle = kwlEventGetHandle(m_eventId);
        kwlEventSetPosition(m_eventHandle, m_position[0], m_position[1], m_position[2]);
    }
    
    m_orientation[2] = -1;
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
    e = kwlGetError();
    assert(e == KWL_NO_ERROR);
}

SoundEmitter::~SoundEmitter()
{
    if (m_oneShot)
    {
    }
    else
    {
        kwlEventStop(m_eventHandle);
        kwlEventRelease(m_eventHandle);
    }
    
}

void SoundEmitter::start()
{
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR);
    if (m_oneShot)
    {
        kwlEventStartOneShotAt(m_eventDefinitionHandle, m_position[0], m_position[1], m_position[2]);
    }
    else
    {
        kwlEventStart(m_eventHandle);
    }
    e = kwlGetError();
    assert(e == KWL_NO_ERROR);
}

const char* SoundEmitter::getEventId()
{
    return m_eventId;
}

float SoundEmitter::getPosX()
{
    return m_position[0];
}

float SoundEmitter::getPosY()
{
    return m_position[1];
}

float SoundEmitter::getPosZ()
{
    return m_position[2];
}

void SoundEmitter::setPos(float x, float y, float z)
{
    m_position[0] = x;
    m_position[1] = y;
    m_position[2] = z;
}

void SoundEmitter::update(float timeStep)
{
    if (timeStep > 0)
    {
        m_velocity[0] = (m_position[0] - m_prevPosition[0]) / timeStep;
        m_velocity[1] = (m_position[1] - m_prevPosition[1]) / timeStep;
        m_velocity[2] = (m_position[2] - m_prevPosition[2]) / timeStep;
        //printf("vel %f, %f, %f\n", xVel, yVel, zVel);
    }
    if (!m_oneShot)
    {
        kwlEventSetPosition(m_eventHandle, m_position[0], m_position[1], m_position[2]);
        kwlEventSetVelocity(m_eventHandle, m_velocity[0], m_velocity[1], m_velocity[2]);
        kwlEventSetOrientation(m_eventHandle, m_orientation[0], m_orientation[1], m_orientation[2]);
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR);
    }
    
    m_prevPosition[0] = m_position[0];
    m_prevPosition[1] = m_position[1];
    m_prevPosition[2] = m_position[2];
    
    if (m_angularVelocity != 0.0f)
    {
        m_orientationAngle += timeStep * m_angularVelocity;
        m_orientation[0] = 0.0f;
        m_orientation[1] = cosf(m_orientationAngle);
        m_orientation[2] = sinf(m_orientationAngle);
    }
}

void SoundEmitter::renderCone()
{   
    int numSegments = 16;
    
    //hardcoded to match whatever values are defined in project data
    float innerConeAngle = 1.0471975f / 2.0f;
    float outerConeAngle = 2.4434609f / 2.0f;
    
    float cosConeAngles[2] = {cosf(innerConeAngle), cosf(outerConeAngle)};
    float sinConeAngles[2] = {sinf(innerConeAngle), sinf(outerConeAngle)};
    
    float scale = 5.0f;
    
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2 * scale, 0);
    glEnd();
    
    glColor3f(0, 0, 0);
    
    for (int k = 0; k < 2; k++)
    {
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(0.6f, 0.6f, 0.6f);
        glVertex3f(0, 0, 0);
        glColor3f(0.8f, 0.8f, 0.8f);
        for (int i = 0; i <= numSegments; i++)
        {    
            float angle = i * 2.0f * 3.141592f / numSegments;
            float cos = cosf(angle);
            float sin = sinf(angle);
            
            float y = cosConeAngles[k];
            
            float x = sinConeAngles[k] * sin;
            
            float z = sinConeAngles[k] * cos;
            glVertex3f(scale * x, scale * y, scale * z);
            
        }
        glEnd();
    }    
}

void SoundEmitter::render()
{
    bool isPlaying = m_oneShot ? false : kwlEventIsPlaying(m_eventHandle);
    const float SCALE = isPlaying ? 0.4f : 0.2f;
    
    glPushMatrix();
    glTranslatef(m_position[0], m_position[1], m_position[2]);
    
    //rotate so that the event orientation vector is 
    //pointing in the positive y direction
        
    glRotatef(180 * m_orientationAngle / 3.141592f, 1.0f, 0.0f, 0.0f);
    
    glScalef(SCALE, SCALE, SCALE);
    if (m_lastTriggeredOneShot)
    {
        glColor3f(0.2f, 0.4f, 0.9f);
    }
    else if (isPlaying)
    {
        glColor3f(0.2f, 0.7f, 0.0f);
    }
    else
    {
        glColor3f(0.7f, 0.2f, 0.0f);
    }
    glBegin(GL_QUADS);
    
    //-x
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, 1, -1);
    //+x
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    
    //-y
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    //+y
    glVertex3f(-1, 1, -1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, 1, -1);
    
    //-z
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, -1, -1);
    //+z
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(1, -1, 1);
    glEnd();
    
    if (m_drawCone)
    {
        renderCone();
    }
    glPopMatrix();
}
