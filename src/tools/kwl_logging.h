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

#ifndef KWL_LOGGING_H
#define KWL_LOGGING_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
    /**
     * A callback for logging various kinds of messages.
     * Accepts printf style formatting.
     * @param message The message to print.
     */
    typedef void (*kwlLogCallback)(const char* message, ...);
    
    /**
     * The default log callback. Prints to stdout.
     * @param format The message to print.
     */
    void kwlDefaultLogCallback(const char* format, ...);
    
    /**
     * The same as kwlDefaultLogCallback but adds an extra new line
     * after each call.
     * @param format The message to print.
     */
    void kwlLogCallbackWithExtraNewline(const char* format, ...);
    
    /**
     * This log callback does nothing. Used for supressing output.
     * @param format Whatever. Gets ignored.
     */
    void kwlSilentLogCallback(const char* format, ...);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_LOGGING_H*/
