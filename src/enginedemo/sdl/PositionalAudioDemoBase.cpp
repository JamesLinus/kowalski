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
#include "PositionalAudioDemoBase.h"
#include <SDL/SDL_opengl.h>

PositionalAudioDemoBase::PositionalAudioDemoBase() : 
    DemoBase(),
    m_camera(),
    m_numEmitters(0),
    m_emitters(NULL)
{

}

PositionalAudioDemoBase::~PositionalAudioDemoBase()
{

}

const char* PositionalAudioDemoBase::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "Move listener: arrow keys, A, Z"; 
    }
    return "";
}

void PositionalAudioDemoBase::update(float timeStep)
{
    //update camera movement depending on keyboard input
    Uint8* kstates = SDL_GetKeyState(NULL);
    float speed = 4;
    float steeringSpeed = 0.73f;
    
    if (kstates[SDLK_UP])
    {
        m_camera.moveForward(speed * timeStep);
    }
    if (kstates[SDLK_DOWN])
    {
        m_camera.moveForward(-speed * timeStep);
    }
    if (kstates[SDLK_LEFT])
    {
        m_camera.steer(steeringSpeed * timeStep);
    }
    if (kstates[SDLK_RIGHT])
    {
        m_camera.steer(-steeringSpeed * timeStep);
    }
    if (kstates[SDLK_a])
    {
        m_camera.moveUp(speed * timeStep);
    }
    if (kstates[SDLK_z])
    {
        m_camera.moveUp(-speed * timeStep);
    }
    
    //set listener velocity, position and orientation to match the camera
    m_camera.update(timeStep);
    
    //update sound emitters
    for (int i = 0; i < m_numEmitters; i++)
    {
        m_emitters[i].update(timeStep);
    }
}

void PositionalAudioDemoBase::render3D()
{
    m_camera.setTransform();
    
    for (int i = 0; i < m_numEmitters; i++)
    {
        m_emitters[i].render();
    }
    
    //coord axes
    float l = 1;
    glBegin(GL_LINES);
    glColor4f(1, 0, 0, 1);
    glVertex3f(l, 0, 0);
    glVertex3f(0, 0, 0);
    glColor4f(0, 1, 0, 1);
    glVertex3f(0, l, 0);
    glVertex3f(0, 0, 0);
    glColor4f(0, 0, 1, 1);
    glVertex3f(0, 0, l);
    glVertex3f(0, 0, 0);
    glEnd();
    
    //ground plane
    int numTicks = 20;
    glColor4f(0, 0, 0, 0.15);
    glBegin(GL_LINES);
    for (int i = -numTicks; i <= numTicks; i++)
    {
        glVertex3f(-numTicks, 0, i);
        glVertex3f(numTicks, 0, i);
        glVertex3f(i, 0, -numTicks);
        glVertex3f(i, 0, numTicks);
    }
    glEnd();
        
    //emitter and axis labels
    float spacing = 1.1f;
    glColor4f(1, 0, 0, 1);
    BitmapFont::drawString(l * spacing, 0, 0, "x");
    glColor4f(0, 1, 0, 1);
    BitmapFont::drawString(0, l * spacing, 0, "y");
    glColor4f(0, 0, 1, 1);
    BitmapFont::drawString(0, 0, l * spacing, "z");
    glDisable(GL_DEPTH_TEST); //draw labels as overlay
    glColor3f(0, 0, 0);
    for (int i = 0; i < m_numEmitters; i++)
    {
        BitmapFont::drawString(m_emitters[i].getPosX(), 
                               m_emitters[i].getPosY(), 
                               m_emitters[i].getPosZ(), 
                               m_emitters[i].getEventId(), 
                               BitmapFont::HCENTER | BitmapFont::VCENTER);
    }
}

void PositionalAudioDemoBase::initialize()
{
    m_camera = Camera();
}

void PositionalAudioDemoBase::deinitialize()
{
    if (m_emitters != NULL)
    {
        delete[] m_emitters;
        m_emitters = NULL;
    }
}
