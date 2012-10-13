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
#include "DSPDemo.h"
#include "KowalskiDemoManager.h"
#include "AudioMeteringDemo.h"
#include "AudioTaperDemo.h"
#include "BalanceAndPanDemo.h"
#include "ComplexSoundsDemo.h"
#include "ConeAttenuationDemo.h"
#include "DistanceAttenuationDemo.h"
#include "DopplerShiftDemo.h"
#include "EmitterSwarmDemo.h"
#include "InputDemo.h"
#include "FreeformEventsDemo.h"
#include "MixPresetDemo.h"
#include "SeamlessPitchDemo.h"
#include "PianoDemo.h"
#include "SafeWaveBankUnloadingDemo.h"
#include "SampleClockDemo.h"
#include "kowalski.h"
#include "assert.h"
#include "fileutil.h"
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "kwl_memory.h"

KowalskiDemoManager::KowalskiDemoManager() :
    m_currentDemo(NULL),
    m_demos(NULL),
    m_numDemos(0),
    m_quitRequested(false),
    m_selectedDemoIndex(0),
    m_leftMeterBar(0, 1, true),
    m_rightMeterBar(0, 1, true)
{
    //set up an open gl drawing surface
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        assert(false && "Unable to initialize SDL");
    }
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    /*SDL_Surface* screen = */SDL_SetVideoMode(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 16, SDL_OPENGL);
    
    initOpenGL();
   
    //fire up the kowalski engine
    kwlInitialize(44100, 2, 1, 256);
    kwlLevelMeteringSetEnabled(1);
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR && "kowalski error on initializing engine");
        
    //create the demos
    m_numDemos = 16;
    m_demos = new DemoBase*[m_numDemos];
    
    m_demos[0] = new DistanceAttenuationDemo();
    m_demos[1] = new DopplerShiftDemo();
    m_demos[2] = new EmitterSwarmDemo();
    m_demos[3] = new ConeAttenuationDemo();
    m_demos[4] = new SampleClockDemo();
    m_demos[5] = new AudioMeteringDemo();
    m_demos[6] = new FreeformEventsDemo();
    m_demos[7] = new AudioTaperDemo();
    m_demos[8] = new PianoDemo();
    m_demos[9] = new MixPresetDemo();
    m_demos[10] = new SafeWaveBankUnloadingDemo();
    m_demos[11] = new ComplexSoundsDemo();
    m_demos[12] = new DSPDemo();
    m_demos[13] = new SeamlessPitchDemo();
    m_demos[14] = new BalanceAndPanDemo();
    m_demos[15] = new InputDemo();
}

KowalskiDemoManager::~KowalskiDemoManager()
{   
    for (int i = 0; i < m_numDemos; i++)
    {
        delete m_demos[i];
    }
    
    delete[] m_demos;
    
    kwlDeinitialize();
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR && "kowalski error on deinitializing engine");
    
    SDL_Quit();
}

void KowalskiDemoManager::setCurrentDemo(int index)
{
    assert(m_currentDemo == NULL);
    assert(index >= 0 && index < m_numDemos);
    
    //load and unload the engine data for each demo for testing purposes
    //(the engine data should really only be loaded once)
    kwlEngineDataLoad(getResourcePath("demoproject.kwl"));
    kwlError error = kwlGetError();
    assert(error == KWL_NO_ERROR);
    
    m_currentDemo = m_demos[index];
    m_currentDemo->initialize();
    error = kwlGetError();
    assert(error == KWL_NO_ERROR && "kowalski error on initializing demo");
}

void KowalskiDemoManager::endCurrentDemo()
{
    assert(m_currentDemo != NULL);
    m_currentDemo->deinitialize();
    
    kwlError e = kwlGetError();
    assert(e == KWL_NO_ERROR && "kowalski error on deinitializing demo");
    
    m_currentDemo = NULL;
    
    //load and unload the engine data for each demo for testing purposes
    //(the engine data should really only be loaded once)
    kwlEngineDataUnload();
}

void KowalskiDemoManager::update(float timeStep)
{
    if (m_currentDemo != NULL)
    {
        m_currentDemo->update(timeStep);
        
        kwlError e = kwlGetError();
        assert(e == KWL_NO_ERROR && "kowalski error on updating demo");
    }
    kwlUpdate(timeStep);
    m_leftMeterBar.setValue(kwlGetLevelLeft());
    m_rightMeterBar.setValue(kwlGetLevelRight());
}

void KowalskiDemoManager::render()
{
    //clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //set up 3D drawing with a perspective camera.
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, VIEWPORT_WIDTH / ((float)VIEWPORT_HEIGHT), 0.1, 90);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4f(1, 1, 1, 1);
    
    if (m_currentDemo != NULL)
    {
        m_currentDemo->render3D();
    }
    
    //set up 2D drawing in pixel coordinates
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, VIEWPORT_WIDTH, 0, VIEWPORT_HEIGHT);
    glColor4f(1, 1, 1, 1);
    
    if (m_currentDemo != NULL)
    {
        m_currentDemo->render2D();
        renderTopBar();
        glColor4f(1, 1, 1, 1);
        float y =  VIEWPORT_HEIGHT - 12;
        for (int i = 0; i < 3; i++)
        {
            
            BitmapFont::drawString(VIEWPORT_WIDTH / 2, y - i * 10, 
                                   m_currentDemo->getInstructionLine(i), BitmapFont::HCENTER);
        }
        
        glColor4f(0.6f, 0.6f, 0.6f, 1);
        BitmapFont::drawString(VIEWPORT_WIDTH / 2, y - 3 * 10,
                               "Return to menu: ESC", BitmapFont::HCENTER);
    }
    else
    {
        //render the menu if there is no current demo
        renderMenu();
    }
    
#ifdef KWL_DEBUG_MEMORY
    char freeMemStr[256];
    sprintf(freeMemStr, "live bytes: %d", kwlDebugGetLiveBytes());
    glColor3f(1, 0, 0);
    BitmapFont::drawString(10, 10, freeMemStr);
#endif //KWL_DEBUG_MEMORY
    
    //make sure drawing went well, then swap buffers
    assert(glGetError() == GL_NO_ERROR);
    SDL_GL_SwapBuffers();
}

void KowalskiDemoManager::renderTopBar()
{
    int topBarHeight = 60;
    int shadowHeight = 10;
    glColor4f(0, 0, 0, 1);
    glShadeModel(GL_SMOOTH);
    glBegin(GL_QUADS);
    glVertex2f(0, VIEWPORT_HEIGHT);
    glVertex2f(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    glVertex2f(VIEWPORT_WIDTH, VIEWPORT_HEIGHT - topBarHeight);
    glVertex2f(0, VIEWPORT_HEIGHT - topBarHeight);
    glColor4f(1, 1, 1, 1);
    glVertex2f(0, VIEWPORT_HEIGHT - topBarHeight);
    glVertex2f(VIEWPORT_WIDTH, VIEWPORT_HEIGHT - topBarHeight);
    glColor4f(0.8f, 0.8f, 0.8f, 1);

    glVertex2f(VIEWPORT_WIDTH, VIEWPORT_HEIGHT - topBarHeight + shadowHeight);
    glVertex2f(0, VIEWPORT_HEIGHT - topBarHeight + shadowHeight);
    glEnd();
    
    //meter bars
    m_leftMeterBar.render(VIEWPORT_WIDTH - 20, VIEWPORT_HEIGHT - topBarHeight + 12, 8, topBarHeight - 14);
    m_rightMeterBar.render(VIEWPORT_WIDTH - 10, VIEWPORT_HEIGHT - topBarHeight + 12, 8, topBarHeight - 14);
    
}

void KowalskiDemoManager::renderMenu()
{
    renderTopBar();
    
    const int yTop = VIEWPORT_HEIGHT - 60 - 80;
    const int menuItemHeight = 20;
    const int menuItemSpacing = 5;
    const int menuItemWidth = 300;
    
    glColor3f(1, 1, 1);
    BitmapFont::drawString(VIEWPORT_WIDTH / 2, 
                           VIEWPORT_HEIGHT - 23, "kowalski engine demo", BitmapFont::HCENTER);
    glColor3f(0.6f, 0.6f, 0.6f);
    BitmapFont::drawString(VIEWPORT_WIDTH / 2, 
                           VIEWPORT_HEIGHT - 33, "http://kowalski.sourceforge.net", BitmapFont::HCENTER);
    glColor3f(0.5f, 0.5f, 0.5f);
    BitmapFont::drawString(VIEWPORT_WIDTH / 2, 
                           30, "Navigate the menu using the arrow keys", BitmapFont::HCENTER);
    BitmapFont::drawString(VIEWPORT_WIDTH / 2,
                           20, "and space bar. Press escape to quit.", BitmapFont::HCENTER);
    
    float yDesc = VIEWPORT_HEIGHT - 60 - 35;

    glColor3f(0, 0, 0);
    BitmapFont::drawString(VIEWPORT_WIDTH / 2, yDesc
                           , m_demos[m_selectedDemoIndex]->getDescription(), BitmapFont::HCENTER);
    
    float y = yTop;
    for (int i = 0; i < m_numDemos; i++)
    {
        DemoBase* demo = m_demos[i];
        
        if (i == m_selectedDemoIndex)
        {
            glColor4f(0, 0, 0, 1);
            glBegin(GL_QUADS);
        }
        else
        {
            glColor4f(0, 0, 0, 0.2f);
            glBegin(GL_LINE_LOOP);
        }
        glVertex2f(VIEWPORT_WIDTH / 2 - menuItemWidth / 2, y);
        glVertex2f(VIEWPORT_WIDTH / 2 + menuItemWidth / 2, y);
        glVertex2f(VIEWPORT_WIDTH / 2 + menuItemWidth / 2, y + menuItemHeight);
        glVertex2f(VIEWPORT_WIDTH / 2 - menuItemWidth / 2, y + menuItemHeight);
        glEnd();
        
        if (i == m_selectedDemoIndex)
        {
            glColor3f(1, 1, 1);
        }
        else
        {
            glColor3f(0.5f, 0.5f, 0.5f);
        }
        
        BitmapFont::drawString(VIEWPORT_WIDTH / 2, y + menuItemHeight / 2, 
                               demo->getName(), BitmapFont::HCENTER | BitmapFont::VCENTER);
        
        y -= menuItemHeight + menuItemSpacing;
    }
}

void KowalskiDemoManager::initOpenGL()
{
    glClearColor(0.97f, 0.97f, 0.97f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
    
    BitmapFont::init();
    assert(glGetError() == GL_NO_ERROR);
}

void KowalskiDemoManager::processEvents()
{
    SDL_Event event;

    if (m_currentDemo != NULL)
    {
        //if there is a current demo, delegate all key down and up events
        //except escape, which stops the demo and returns to the main menu
        while(SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_KEYDOWN) 
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    endCurrentDemo();
                }
                else if (m_currentDemo)
                {
                    m_currentDemo->onKeyDown(event.key.keysym.sym);
                }
            }
            else if (event.type == SDL_KEYUP) 
            {
                if (m_currentDemo)
                {
                    m_currentDemo->onKeyUp(event.key.keysym.sym);
                }
            }
        }
    }
    else
    {
        while(SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_KEYDOWN) 
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    m_selectedDemoIndex--;
                    if (m_selectedDemoIndex < 0)
                    {
                        m_selectedDemoIndex = m_numDemos - 1;
                    }
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    m_selectedDemoIndex++;
                    if (m_selectedDemoIndex >= m_numDemos)
                    {
                        m_selectedDemoIndex = 0;
                    }
                }
                else if (event.key.keysym.sym == SDLK_SPACE ||
                         event.key.keysym.sym == SDLK_RETURN)
                {
                    setCurrentDemo(m_selectedDemoIndex);
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    m_quitRequested = true;
                }
            }
        }
    }
}

void KowalskiDemoManager::mainLoop()
{
    while (!m_quitRequested)
    {
        float timeStep = 0.001f * UPDATE_INTERVAL;
        processEvents();
        update(timeStep);
        render();
        SDL_Delay(UPDATE_INTERVAL);
    }    
}

