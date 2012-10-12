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

#import <Cocoa/Cocoa.h>

#include "fileutil.h"

/**
 * Returns the full path to a file in the application's main bundle.
 * @param The path to the file relative to the main bundle's resource folder.
 * @return The full path to the file.
 */
const char* getResourcePath(const char* relativePath)
{
    NSString* path = [[NSBundle mainBundle] pathForResource:[[NSString alloc] initWithUTF8String:relativePath] ofType:nil];
    const char* pChar = [path UTF8String];
    return pChar;
}
