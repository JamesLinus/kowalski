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

#ifndef KOWALSKI_POSITIONAL_AUDIO_DEMO_BASE_H
#define KOWALSKI_POSITIONAL_AUDIO_DEMO_BASE_H

#include "Camera.h"
#include "DemoBase.h"
#include "SoundEmitter.h"

/** 
 * Base class for demos with a number of sound emitters 
 * and a camera, also acting as a listener, moving in 3D space.
 */
class PositionalAudioDemoBase : public DemoBase
{
public:
    PositionalAudioDemoBase();
    ~PositionalAudioDemoBase();
    virtual void update(float timeStep);
    virtual void initialize();
    virtual void deinitialize();
    virtual const char* getInstructionLine(int index);
    /** Draws the emitters and the floor plane. */
    void render3D();
protected:
    /** An array of sound emitters. */
    SoundEmitter* m_emitters;
    /** The number of sound emitters. */
    int m_numEmitters;
    /** 
     * The camera that the scene is viewed through and that determines the 
     * position, facing and velocity of the positional audio listener.
     */
    Camera m_camera;
private:
    /** Draws a plane grid in the xz plane at y = 0 */
    void drawFloor();
    /** Draws all emitters*/
    void drawEmitters();
};

#endif //KOWALSKI_POSITIONAL_AUDIO_DEMO_BASE_H

