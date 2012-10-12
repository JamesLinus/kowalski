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

#ifndef KOWALSKI_BITMAPFONT_H
#define KOWALSKI_BITMAPFONT_H

#define BITMAP_FONT_NUM_GLYPHS 95
#define BITMAP_FONT_FIRST_ASCII_INDEX 32
#define BITMAP_FONT_LAST_ASCII_INDEX 126
#define BITMAP_FONT_GLYPH_MASK_HEIGHT 7

/**
 * This class encapsulates minimalistic OpenGL text rendering functionality
 * and does not rely on loading font resources from disk. Only ASCII characters
 * are supported. The font used is Silkscreen.
 */
class BitmapFont
{
public:
    /** Alignment constants. */
    enum
    {
        LEFT = 1 << 0,
        RIGHT = 1 << 1,
        HCENTER = 1 << 2,
        TOP = 1 << 3,
        VCENTER = 1 << 4,
        BASELINE = 1 << 5
    };
    
    /**
     * Initialises the bitmap font. Calling this method on an already
     * initialised bitmap font does nothing.
     */
    static void init();    
    
    /**
     * Draws a left aligned string at a given position.
     * @param x The x coordinate of the drawn string.
     * @param y The y coordinate of the drawn string.
     * @param const char* The const char* to be drawn.
     */
    static void drawString(float x,
                           float y,
                           const char* string);
    
    /**
     * Draws a left aligned string at a given position.
     * @param x The x coordinate of the drawn string.
     * @param y The y coordinate of the drawn string.
     * @param z The z coordinate of the drawn string.
     * @param const char* The const char* to be drawn.
     */
    static void drawString(float x,
                           float y,
                           float z,
                           const char* string);
    
    /**
     * Draws a string at a given position with a given alignment.
     * @param x The x coordinate of the drawn string.
     * @param y The y coordinate of the drawn string.
     * @param const char* The const char* to be drawn.
     * @param align Alignment flags, for example LEFT | TOP.
     */
    static void drawString(float x,
                           float y,
                           const char* string,
                           int align);
    
    /**
     * Draws a string at a given position with a given alignment.
     * @param x The x coordinate of the drawn string.
     * @param y The y coordinate of the drawn string.
     * @param z The z coordinate of the drawn string.
     * @param const char* The const char* to be drawn.
     * @param align Alignment flags, for example LEFT | TOP.
     */
    static void drawString(float x,
                           float y,
                           float z,
                           const char* string,
                           int align);
        
    /**
     * Returns the width in pixels of a given string.
     * @param const char* The string to measure the width of.
     * @return The width of the string, in pixels.
     */
    static int getStringWidth(const char* string);
    
private:
    /** */
    static const int BASELINE_OFFSET = 1;
    
    /** Glyph widths in pixels. */
    static int GLYPH_WIDTHS[BITMAP_FONT_NUM_GLYPHS];
    
    /** Glyph bitmasks.*/
    static unsigned char GLYPH_MASKS[BITMAP_FONT_NUM_GLYPHS][BITMAP_FONT_GLYPH_MASK_HEIGHT];
    
    /** The display list offset*/
    static int m_listOffset;
    
    /** The number of pixels between glyphs.*/
    static int m_spacing;
    
    /** Indicates if the font has been initialized.*/
    static bool m_isInitialized;
    
};

#endif //KOWALSKI_BITMAPFONT_H
