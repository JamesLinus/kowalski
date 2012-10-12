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

#ifndef KOWALSKI_SOUNDEMITTER_H
#define KOWALSKI_SOUNDEMITTER_H

#include "kowalski.h"

class SoundEmitter
{
public:
    SoundEmitter();
    /** */
    SoundEmitter(const char* eventId, float x, float y, float z, bool oneShot = false);
    ~SoundEmitter();
    /** Draws a visualization of the sound emitter. */
    void render();
    void renderCone();
    /** Updates the sound emitter.*/
    void update(float timeStep);
    void start();
    const char* getEventId();
    float getPosX();
    float getPosY();
    float getPosZ();
    void setPos(float x, float y, float z); 
    float getVelX();
    float getVel();
    float getVelZ();
    void setVel(float x, float y, float z); 
    float getDirectionX();
    float getDirection();
    float getDirectionZ();
    void setDirection(float x, float y, float z); 
    void setLastTriggered(bool lastTriggered) { m_lastTriggeredOneShot = lastTriggered; }
    void setAngularVelocity(float vel) { m_angularVelocity = vel; }
    void setDrawCone(bool draw) { m_drawCone = draw; }
private:
    /** A handle to the event associated with this emitter.*/
    kwlEventHandle m_eventHandle;
    kwlEventDefinitionHandle m_eventDefinitionHandle;
    float m_position[3];
    float m_prevPosition[3];
    float m_velocity[3];
    float m_orientation[3];
    const char* m_eventId;
    bool m_oneShot;
    bool m_lastTriggeredOneShot;
    float m_angularVelocity;
    float m_orientationAngle;
    bool m_drawCone;
};

#endif //KOWALSKI_SOUNDEMITTER_H
