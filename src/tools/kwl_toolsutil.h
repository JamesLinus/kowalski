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

#ifndef KWL_TOOLS_H
#define KWL_TOOLS_H

#define KWL_XML_MIX_BUS_NAME "MixBus"
#define KWL_XML_WAVE_BANK_GROUP_NAME "WaveBankGroup"
#define KWL_XML_MIX_PRESET_GROUP_NAME "MixPresetGroup"
#define KWL_XML_SOUND_GROUP_NAME "SoundGroup"
#define KWL_XML_EVENT_GROUP_NAME "EventGroup"
#define KWL_XML_PROJECT_NODE_NAME "KowalskiProjectData"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    
    /**
     *
     */
    typedef void (*kwlLogCallback)(const char* message, ...);

    void kwlDefaultLogCallback(const char* format, ...);
    
    void kwlSilentLogCallback(const char* format, ...);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_XML_VALIDATION_H*/
