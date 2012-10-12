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

#ifndef KOWALSKI_DEMO_BASE_H
#define KOWALSKI_DEMO_BASE_H

#include "SDL.h"

/** 
 * An abstract base class for demos managed by a KowalskiDemoManager instance. 
 * Provides a collection of methods that should be overridden to provide 
 * custom demo functionality.
 */
class DemoBase
{
public:
    /** Constructs a new demo.*/
    DemoBase();
    /** Destructor. */
    virtual ~DemoBase();
    /** Returns the name of the demo.*/
    virtual const char* getName() = 0;
    /** A short description of the demo.*/
    virtual const char* getDescription() = 0;
    /** 
     * Returns one of the three lines of demo instructions
     * @param index 0, 1 or 2.
     */
    virtual const char* getInstructionLine(int index) = 0;
    /** 
     * Override this method to perform 3D rendering. When this method is called,
     * a perspective camera has been set up and depth testing is enabled.
     */
    virtual void render3D();
    /**
     * Override this method to perform 2D rendering in window pixel coordinates, 
     * with the origin in the bottom left corner. Depth testing is off by default
     * when this method is called.
     */
    virtual void render2D();
    /** Gets called when the demo is active and a key is pressed. */
    virtual void onKeyDown(SDLKey key);
    /** Gets called when the demo is active and a key is released. */
    virtual void onKeyUp(SDLKey key);
    /** Does any setup required by the demo. Called when the demo is selected from the menu. */
    virtual void initialize() = 0;
    /** Deinitialzes the demo. Called when returning to the menu.*/
    virtual void deinitialize() = 0;
    /** Gets called continually when the demo is active. */
    virtual void update(float timeStep) = 0;
};

#endif //KOWALSKI_DEMO_BASE_H
