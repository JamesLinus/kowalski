#include "assert.h"
#include "MeterBar.h"
#include "SDL_opengl.h"

MeterBar::MeterBar(float minValue, float maxValue, bool gradientFill) :
    m_doGradientFill(gradientFill),
    m_minValue(minValue),
    m_maxValue(maxValue),
    m_currentValue(minValue)
{
    assert(m_minValue < m_maxValue);
}

void MeterBar::setValue(float value)
{
    if (value > m_maxValue)
    {
        value = m_maxValue;
    }
    if (value < m_minValue)
    {
        value = m_minValue;
    }
    m_currentValue = value;
}

float MeterBar::getRelativeValue()
{
    return (m_currentValue - m_minValue) / (m_maxValue - m_minValue);
}

void MeterBar::render(float x, float y, float width, float height)
{
    float val = getRelativeValue();
    
    glShadeModel(GL_SMOOTH);
    glBegin(GL_QUADS);
    if (m_doGradientFill)
    {
        glColor3f(0, 1, 0);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glColor3f(1, 1, 0);
        glVertex2f(x + width, y + height / 2);
        glVertex2f(x, y + height / 2);
        
        glVertex2f(x, y + height / 2);
        glVertex2f(x + width, y + height / 2);
        glColor3f(1, 0, 0);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    }
    else
    {
        glColor3f(0.6f, 0.6f, 0.6f);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
    }
    
    if (m_doGradientFill)
    {
        glColor3f(0.2f, 0.2f, 0.2f);
    }
    else
    {
        glColor3f(0.8f, 0.8f, 0.8f);
    }
    if (width > height)
    {
        glVertex2f(x + width * val, y);
        glVertex2f(x + width * val, y + height);
        glVertex2f(x + width, y + height);
        glVertex2f(x + width, y);    
    }
    else
    {
        glVertex2f(x, y + height * val);
        glVertex2f(x + width, y + height * val);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);    
    }
    
    glEnd();
}
