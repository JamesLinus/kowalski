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

#ifndef KOWALSKI_METER_BAR_H
#define KOWALSKI_METER_BAR_H

/**
 * A horizontal or vertical meter bar.
 */
class MeterBar
{
public:   
    MeterBar(float minValue, float MaxValue, bool gradientFill);
    void setValue(float value);
    /** If width < height, the bar is vertical, otherwise horizontal.*/
    void render(float x, float y, float width, float height);
private:
    float getRelativeValue();
    bool m_doGradientFill;
    float m_currentValue;
    float m_minValue;
    float m_maxValue;
};

#endif //METER_BAR
