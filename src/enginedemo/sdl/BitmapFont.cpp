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

#include <SDL/SDL_opengl.h>
#include "assert.h"
#include <cstring>

#include "BitmapFont.h"

int BitmapFont::m_listOffset;

int BitmapFont::m_spacing = 1;

bool BitmapFont::m_isInitialized = false;

int BitmapFont::GLYPH_WIDTHS[BITMAP_FONT_NUM_GLYPHS] = {
    3,     /*    , ASCII 32   */
    2,     /*   !, ASCII 33   */
    4,     /*   ", ASCII 34   */
    6,     /*   #, ASCII 35   */
    5,     /*   $, ASCII 36   */
    6,     /*   %, ASCII 37   */
    8,     /*   &, ASCII 38   */
    2,     /*   ', ASCII 39   */
    3,     /*   (, ASCII 40   */
    3,     /*   ), ASCII 41   */
    6,     /*   *, ASCII 42   */
    6,     /*   +, ASCII 43   */
    3,     /*   ,, ASCII 44   */
    4,     /*   -, ASCII 45   */
    2,     /*   ., ASCII 46   */
    4,     /*   /, ASCII 47   */
    5,     /*   0, ASCII 48   */
    4,     /*   1, ASCII 49   */
    5,     /*   2, ASCII 50   */
    5,     /*   3, ASCII 51   */
    5,     /*   4, ASCII 52   */
    5,     /*   5, ASCII 53   */
    5,     /*   6, ASCII 54   */
    5,     /*   7, ASCII 55   */
    5,     /*   8, ASCII 56   */
    5,     /*   9, ASCII 57   */
    2,     /*   :, ASCII 58   */
    3,     /*   ;, ASCII 59   */
    4,     /*   <, ASCII 60   */
    4,     /*   =, ASCII 61   */
    4,     /*   >, ASCII 62   */
    5,     /*   ?, ASCII 63   */
    6,     /*   @, ASCII 64   */
    5,     /*   A, ASCII 65   */
    5,     /*   B, ASCII 66   */
    5,     /*   C, ASCII 67   */
    5,     /*   D, ASCII 68   */
    4,     /*   E, ASCII 69   */
    4,     /*   F, ASCII 70   */
    5,     /*   G, ASCII 71   */
    5,     /*   H, ASCII 72   */
    2,     /*   I, ASCII 73   */
    5,     /*   J, ASCII 74   */
    5,     /*   K, ASCII 75   */
    4,     /*   L, ASCII 76   */
    6,     /*   M, ASCII 77   */
    6,     /*   N, ASCII 78   */
    5,     /*   O, ASCII 79   */
    5,     /*   P, ASCII 80   */
    5,     /*   Q, ASCII 81   */
    5,     /*   R, ASCII 82   */
    5,     /*   S, ASCII 83   */
    4,     /*   T, ASCII 84   */
    5,     /*   U, ASCII 85   */
    6,     /*   V, ASCII 86   */
    6,     /*   W, ASCII 87   */
    6,     /*   X, ASCII 88   */
    6,     /*   Y, ASCII 89   */
    4,     /*   Z, ASCII 90   */
    3,     /*   [, ASCII 91   */
    4,     /*   \, ASCII 92   */
    3,     /*   ], ASCII 93   */
    8,     /*   ^, ASCII 94   */
    5,     /*   _, ASCII 95   */
    3,     /*   `, ASCII 96   */
    5,     /*   a, ASCII 97   */
    5,     /*   b, ASCII 98   */
    5,     /*   c, ASCII 99   */
    5,     /*   d, ASCII 100   */
    4,     /*   e, ASCII 101   */
    4,     /*   f, ASCII 102   */
    5,     /*   g, ASCII 103   */
    5,     /*   h, ASCII 104   */
    2,     /*   i, ASCII 105   */
    5,     /*   j, ASCII 106   */
    5,     /*   k, ASCII 107   */
    4,     /*   l, ASCII 108   */
    6,     /*   m, ASCII 109   */
    6,     /*   n, ASCII 110   */
    5,     /*   o, ASCII 111   */
    5,     /*   p, ASCII 112   */
    5,     /*   q, ASCII 113   */
    5,     /*   r, ASCII 114   */
    5,     /*   s, ASCII 115   */
    4,     /*   t, ASCII 116   */
    5,     /*   u, ASCII 117   */
    6,     /*   v, ASCII 118   */
    6,     /*   w, ASCII 119   */
    6,     /*   x, ASCII 120   */
    6,     /*   y, ASCII 121   */
    4,     /*   z, ASCII 122   */
    4,     /*   {, ASCII 123   */
    2,     /*   |, ASCII 124   */
    4,     /*   }, ASCII 125   */
    8,     /*   ~, ASCII 126   */
};

unsigned char BitmapFont::GLYPH_MASKS[BITMAP_FONT_NUM_GLYPHS][BITMAP_FONT_GLYPH_MASK_HEIGHT] =
{
    /*    , ASCII 32   */
    {(unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, },
    /*   !, ASCII 33   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   ", ASCII 34   */
    {(unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0xa0, (unsigned char)0xa0, (unsigned char)0x0, },
    /*   #, ASCII 35   */
    {(unsigned char)0x0, (unsigned char)0x50, (unsigned char)0xf8, (unsigned char)0x50, (unsigned char)0xf8, (unsigned char)0x50, (unsigned char)0x0, },
    /*   $, ASCII 36   */
    {(unsigned char)0x40, (unsigned char)0xe0, (unsigned char)0x10, (unsigned char)0x60, (unsigned char)0x80, (unsigned char)0x70, (unsigned char)0x20, },
    /*   %, ASCII 37   */
    {(unsigned char)0x0, (unsigned char)0x58, (unsigned char)0x58, (unsigned char)0x20, (unsigned char)0xd0, (unsigned char)0xd0, (unsigned char)0x0, },
    /*   &, ASCII 38   */
    {(unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, },
    /*   ', ASCII 39   */
    {(unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   (, ASCII 40   */
    {(unsigned char)0x0, (unsigned char)0x40, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x0, },
    /*   ), ASCII 41   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x80, (unsigned char)0x0, },
    /*   *, ASCII 42   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0xa8, (unsigned char)0x70, (unsigned char)0xa8, (unsigned char)0x20, (unsigned char)0x0, },
    /*   +, ASCII 43   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0xf8, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0x0, },
    /*   ,, ASCII 44   */
    {(unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, },
    /*   -, ASCII 45   */
    {(unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, },
    /*   ., ASCII 46   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, },
    /*   /, ASCII 47   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0x0, },
    /*   0, ASCII 48   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   1, ASCII 49   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0xc0, (unsigned char)0x0, },
    /*   2, ASCII 50   */
    {(unsigned char)0x0, (unsigned char)0xf0, (unsigned char)0x80, (unsigned char)0x60, (unsigned char)0x10, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   3, ASCII 51   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x10, (unsigned char)0x60, (unsigned char)0x10, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   4, ASCII 52   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0xf0, (unsigned char)0xa0, (unsigned char)0xa0, (unsigned char)0x0, },
    /*   5, ASCII 53   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x10, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0xf0, (unsigned char)0x0, },
    /*   6, ASCII 54   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0x60, (unsigned char)0x0, },
    /*   7, ASCII 55   */
    {(unsigned char)0x0, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x20, (unsigned char)0x10, (unsigned char)0xf0, (unsigned char)0x0, },
    /*   8, ASCII 56   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   9, ASCII 57   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x10, (unsigned char)0x70, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   :, ASCII 58   */
    {(unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x0, (unsigned char)0x0, },
    /*   ;, ASCII 59   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x0, (unsigned char)0x40, (unsigned char)0x0, (unsigned char)0x0, },
    /*   <, ASCII 60   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x40, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x20, (unsigned char)0x0, },
    /*   =, ASCII 61   */
    {(unsigned char)0x0, (unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x0, (unsigned char)0x0, },
    /*   >, ASCII 62   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x20, (unsigned char)0x40, (unsigned char)0x80, (unsigned char)0x0, },
    /*   ?, ASCII 63   */
    {(unsigned char)0x0, (unsigned char)0x40, (unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x10, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   @, ASCII 64   */
    {(unsigned char)0x0, (unsigned char)0x70, (unsigned char)0x80, (unsigned char)0xb0, (unsigned char)0xa8, (unsigned char)0x70, (unsigned char)0x0, },
    /*   A, ASCII 65   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0xf0, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   B, ASCII 66   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0xf0, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   C, ASCII 67   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x80, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   D, ASCII 68   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   E, ASCII 69   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   F, ASCII 70   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   G, ASCII 71   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0xb0, (unsigned char)0x80, (unsigned char)0x70, (unsigned char)0x0, },
    /*   H, ASCII 72   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0xf0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x0, },
    /*   I, ASCII 73   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   J, ASCII 74   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x10, (unsigned char)0x10, (unsigned char)0x10, (unsigned char)0x0, },
    /*   K, ASCII 75   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0xa0, (unsigned char)0xc0, (unsigned char)0xa0, (unsigned char)0x90, (unsigned char)0x0, },
    /*   L, ASCII 76   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   M, ASCII 77   */
    {(unsigned char)0x0, (unsigned char)0x88, (unsigned char)0x88, (unsigned char)0xa8, (unsigned char)0xd8, (unsigned char)0x88, (unsigned char)0x0, },
    /*   N, ASCII 78   */
    {(unsigned char)0x0, (unsigned char)0x88, (unsigned char)0x98, (unsigned char)0xa8, (unsigned char)0xc8, (unsigned char)0x88, (unsigned char)0x0, },
    /*   O, ASCII 79   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   P, ASCII 80   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   Q, ASCII 81   */
    {(unsigned char)0x10, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   R, ASCII 82   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0xa0, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   S, ASCII 83   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x10, (unsigned char)0x60, (unsigned char)0x80, (unsigned char)0x70, (unsigned char)0x0, },
    /*   T, ASCII 84   */
    {(unsigned char)0x0, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   U, ASCII 85   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x0, },
    /*   V, ASCII 86   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x50, (unsigned char)0x50, (unsigned char)0x88, (unsigned char)0x88, (unsigned char)0x0, },
    /*   W, ASCII 87   */
    {(unsigned char)0x0, (unsigned char)0x50, (unsigned char)0xa8, (unsigned char)0xa8, (unsigned char)0xa8, (unsigned char)0x88, (unsigned char)0x0, },
    /*   X, ASCII 88   */
    {(unsigned char)0x0, (unsigned char)0x88, (unsigned char)0x50, (unsigned char)0x20, (unsigned char)0x50, (unsigned char)0x88, (unsigned char)0x0, },
    /*   Y, ASCII 89   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0x50, (unsigned char)0x88, (unsigned char)0x0, },
    /*   Z, ASCII 90   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x20, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   [, ASCII 91   */
    {(unsigned char)0x0, (unsigned char)0xc0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0xc0, (unsigned char)0x0, },
    /*   \, ASCII 92   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0x40, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   ], ASCII 93   */
    {(unsigned char)0x0, (unsigned char)0xc0, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0xc0, (unsigned char)0x0, },
    /*   ^, ASCII 94   */
    {(unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, },
    /*   _, ASCII 95   */
    {(unsigned char)0xf0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, },
    /*   `, ASCII 96   */
    {(unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x0, (unsigned char)0x40, (unsigned char)0x80, (unsigned char)0x0, },
    /*   a, ASCII 97   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0xf0, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   b, ASCII 98   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0xf0, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   c, ASCII 99   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x80, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   d, ASCII 100   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   e, ASCII 101   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   f, ASCII 102   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   g, ASCII 103   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0xb0, (unsigned char)0x80, (unsigned char)0x70, (unsigned char)0x0, },
    /*   h, ASCII 104   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0xf0, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x0, },
    /*   i, ASCII 105   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   j, ASCII 106   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x10, (unsigned char)0x10, (unsigned char)0x10, (unsigned char)0x0, },
    /*   k, ASCII 107   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0xa0, (unsigned char)0xc0, (unsigned char)0xa0, (unsigned char)0x90, (unsigned char)0x0, },
    /*   l, ASCII 108   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   m, ASCII 109   */
    {(unsigned char)0x0, (unsigned char)0x88, (unsigned char)0x88, (unsigned char)0xa8, (unsigned char)0xd8, (unsigned char)0x88, (unsigned char)0x0, },
    /*   n, ASCII 110   */
    {(unsigned char)0x0, (unsigned char)0x88, (unsigned char)0x98, (unsigned char)0xa8, (unsigned char)0xc8, (unsigned char)0x88, (unsigned char)0x0, },
    /*   o, ASCII 111   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   p, ASCII 112   */
    {(unsigned char)0x0, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   q, ASCII 113   */
    {(unsigned char)0x10, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x60, (unsigned char)0x0, },
    /*   r, ASCII 114   */
    {(unsigned char)0x0, (unsigned char)0x90, (unsigned char)0xa0, (unsigned char)0xe0, (unsigned char)0x90, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   s, ASCII 115   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x10, (unsigned char)0x60, (unsigned char)0x80, (unsigned char)0x70, (unsigned char)0x0, },
    /*   t, ASCII 116   */
    {(unsigned char)0x0, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0x40, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   u, ASCII 117   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x90, (unsigned char)0x0, },
    /*   v, ASCII 118   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x50, (unsigned char)0x50, (unsigned char)0x88, (unsigned char)0x88, (unsigned char)0x0, },
    /*   w, ASCII 119   */
    {(unsigned char)0x0, (unsigned char)0x50, (unsigned char)0xa8, (unsigned char)0xa8, (unsigned char)0xa8, (unsigned char)0x88, (unsigned char)0x0, },
    /*   x, ASCII 120   */
    {(unsigned char)0x0, (unsigned char)0x88, (unsigned char)0x50, (unsigned char)0x20, (unsigned char)0x50, (unsigned char)0x88, (unsigned char)0x0, },
    /*   y, ASCII 121   */
    {(unsigned char)0x0, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0x20, (unsigned char)0x50, (unsigned char)0x88, (unsigned char)0x0, },
    /*   z, ASCII 122   */
    {(unsigned char)0x0, (unsigned char)0xe0, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x20, (unsigned char)0xe0, (unsigned char)0x0, },
    /*   {, ASCII 123   */
    {(unsigned char)0x0, (unsigned char)0x60, (unsigned char)0x40, (unsigned char)0x80, (unsigned char)0x40, (unsigned char)0x60, (unsigned char)0x0, },
    /*   |, ASCII 124   */
    {(unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x80, (unsigned char)0x0, },
    /*   }, ASCII 125   */
    {(unsigned char)0x0, (unsigned char)0xc0, (unsigned char)0x40, (unsigned char)0x20, (unsigned char)0x40, (unsigned char)0xc0, (unsigned char)0x0, },
    /*   ~, ASCII 126   */
    {(unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, },
};

void BitmapFont::drawString(float x,
                            float y,
                            const char* string)
{
    drawString(x, y, 0.0f, string, 0);
}

void BitmapFont::drawString(float x,
                            float y,
                            const char* string,
                            int align)
{
    drawString(x, y, 0.0f, string, align);
}

void BitmapFont::drawString(float x,
                            float y,
                            float z,
                            const char* string)
{
    drawString(x, y, z, string, 0);
}

void BitmapFont::drawString(float x,
                            float y,
                            float z,
                            const char* string,
                            int align)
{
    //default alignment is bottom left
    
    glRasterPos3f(x, y, z);
    
    //vertical alignment
    if ((align & TOP) != 0)
    {
        glBitmap(0, 0, 0, 0, 0, -BITMAP_FONT_GLYPH_MASK_HEIGHT, NULL);
    }
    else if ((align & VCENTER) != 0)
    {
        glBitmap(0, 0, 0, 0, 0, -(BITMAP_FONT_GLYPH_MASK_HEIGHT >> 1),  NULL);
    }
    
    //horizontal alignment
    if ( (align & RIGHT) != 0)
    {
        glBitmap(0, 0, 0, 0, -getStringWidth(string), 0,  NULL);
    }
    else if ((align & HCENTER) != 0)
    {
        glBitmap(0, 0, 0, 0, -getStringWidth(string) >> 1, 0, NULL);
    }
    
    glPushAttrib(GL_LIST_BIT);
    glListBase(m_listOffset);
    glCallLists(strlen(string), GL_BYTE, string);
    glPopAttrib();
}

void BitmapFont::init()
{    
    if (m_isInitialized)
    {
        return;
    }
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    m_listOffset = glGenLists(128);
    int error = glGetError();
    assert(error == GL_NO_ERROR);
    
    for (int i = BITMAP_FONT_FIRST_ASCII_INDEX; i < BITMAP_FONT_LAST_ASCII_INDEX; i++)
    {
        int glyphIndex = i - BITMAP_FONT_FIRST_ASCII_INDEX;
        glNewList(i + m_listOffset, GL_COMPILE);

        glBitmap (GLYPH_WIDTHS[glyphIndex], //width
                  BITMAP_FONT_GLYPH_MASK_HEIGHT, //height
                  0.0f, //xorig
                  BASELINE_OFFSET, //yorig
                  GLYPH_WIDTHS[glyphIndex] + m_spacing, //xmove
                  0.0f, //ymove
                  GLYPH_MASKS[glyphIndex]);
        glEndList();
    }
    
    m_isInitialized = true;
}

/**
 * Returns the width in pixels of a given string.
 * @param const char* The const char* to measure the width of.
 * @return The width of the string, in pixels.
 */
int BitmapFont::getStringWidth(const char* string)
{
    int stringWidth = 0;
    int numChars = strlen(string);
    
    if (numChars == 0)
    {
        return 0;
    }
    
    for (int i = 0; i < numChars; i++)
    {
        if (   (string[i] - BITMAP_FONT_FIRST_ASCII_INDEX >= BITMAP_FONT_NUM_GLYPHS)
            || (string[i] - BITMAP_FONT_FIRST_ASCII_INDEX < 0) )
        {
            continue;
        }
        stringWidth += GLYPH_WIDTHS[string[i] - BITMAP_FONT_FIRST_ASCII_INDEX] + m_spacing;
    }
    //subtract the last spacing
    stringWidth -= m_spacing;
    return stringWidth;        
}
