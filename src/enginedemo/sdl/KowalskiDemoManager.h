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
#ifndef KOWALSKI_KOWALSKI_DEMO_MANAGER_H
#define KOWALSKI_KOWALSKI_DEMO_MANAGER_H

#include "DemoBase.h"
#include "MeterBar.h"

/** 
 * This class manages a collection of demos, each represented as an instance
 * of a class deriving from DemoBase. The class is responsible for switching
 * between different demos and presenting the collection of demos in a menu.
 */
class KowalskiDemoManager
{
public:
    /** Creates a new demo engine. */
    KowalskiDemoManager();
    /** Destructor. */
    ~KowalskiDemoManager();
    /** The demo engine main loop.*/
    void mainLoop();
    /** The width in pixels of the demo window. */
    static const int VIEWPORT_WIDTH = 800;
    /** The height in pixels of the demo window. */
    static const int VIEWPORT_HEIGHT = 600;
private:
    /** The update interval in milliseconds.*/
    static const int UPDATE_INTERVAL = 20;//50 fps
    /** Sets a given demo as active.*/
    void setCurrentDemo(int index);
    /** Stops and deinitializes the current demo.*/
    void endCurrentDemo();
    /** Does any necessary OpenGL setup.*/
    void initOpenGL();
    /** Renders the current demo, if any, or the demo menu.*/
    void render();
    /** Renders the black top bar.*/
    void renderTopBar();
    /** Called every frame.*/
    void update(float timeStep);
    /** Handles incoming SDL events.*/
    void processEvents();
    /** Draws the demo menu. This method is called if the current demo is NULL */
    void renderMenu();
    /** The currently active demo, NULL if no demo is active.*/
    DemoBase* m_currentDemo;
    /** The index of the currently selected demo.*/
    int m_selectedDemoIndex;
    /** The collection of demos.*/
    DemoBase** m_demos;
    /** The number of demos.*/
    int m_numDemos;
    /** Used to indicate that the user has requested to quit and that the main loop should be terminated.*/
    bool m_quitRequested;
    /*Output level meters in the top bar.*/
    MeterBar m_leftMeterBar;
    MeterBar m_rightMeterBar;
};

#endif //KOWALSKI_KOWALSKI_DEMO_MANAGER_H
