#include "BalanceAndPanDemo.h"
#include "KowalskiDemoManager.h"
#include "assert.h"
#include "fileutil.h"

BalanceAndPanDemo::BalanceAndPanDemo() :
    m_eventHandle(KWL_INVALID_HANDLE),
    m_waveBankHandle(KWL_INVALID_HANDLE),
    m_currentBalance(0.0f),
    m_panBar(-1, 1, false)
{
    
}

BalanceAndPanDemo::~BalanceAndPanDemo()
{

}

void BalanceAndPanDemo::update(float timeStep)
{
    float delta = timeStep * 0.6f;
    Uint8* kstates = SDL_GetKeyState(NULL);
    
    if (kstates[SDLK_RIGHT])
    {
        m_currentBalance += delta;
    }
    else if (kstates[SDLK_LEFT])
    {
        m_currentBalance -= delta;
    }
    
    if (m_currentBalance > 1.0f)
    {
        m_currentBalance = 1.0f;
    }
    else if (m_currentBalance < -1.0f)
    {
        m_currentBalance = -1.0f;
    }
    
    kwlEventSetBalance(m_eventHandle, m_currentBalance);
}

void BalanceAndPanDemo::render2D()
{
    float x = KowalskiDemoManager::VIEWPORT_WIDTH / 2;
    float y = KowalskiDemoManager::VIEWPORT_HEIGHT / 2;
    float h = 30;
    float w = 400;
    
    m_panBar.setValue(m_currentBalance);
    m_panBar.render(x - w / 2, y - h / 2, w, h);
}

void BalanceAndPanDemo::initialize()
{
    m_currentBalance = 0.0f;
    m_waveBankHandle = kwlWaveBankLoad(getResourcePath("sfx.kwb"));
    m_eventHandle = kwlEventGetHandle("balancedemo/mono_and_stereo");
    kwlEventStartFade(m_eventHandle, 1.0f);
}

void BalanceAndPanDemo::deinitialize()
{
    kwlEventStop(m_eventHandle);
    kwlEventRelease(m_eventHandle);
    kwlWaveBankUnload(m_waveBankHandle);
    m_eventHandle = KWL_INVALID_HANDLE;
    m_waveBankHandle = KWL_INVALID_HANDLE;
}

void BalanceAndPanDemo::onKeyDown(SDLKey key)
{
    
}

const char* BalanceAndPanDemo::getInstructionLine(int index)
{
    switch (index)
    {
        case 0:
            return "The example event uses a sound with both mono and stereo data."; 
        case 1:
            return "Change pan/balance: left/right arrow keys";
    }
    return "";
}
