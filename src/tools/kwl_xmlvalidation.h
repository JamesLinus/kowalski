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

#ifndef KWL_XML_VALIDATION_H
#define KWL_XML_VALIDATION_H

#include "kwl_toolsutil.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    
    /**
     *
     */
    void kwlValidateProjectData(const char* xmlPath, kwlLogCallback messageCallback);
    
    /**
     *
     */
    void kwlValidateWaveBank(const char* waveBankPath);
    
    /**
     *
     */
    void kwlValidateEngineData(const char* engineDataPath);

    /**
     *
     */
    void kwlBuildEngineData(const char* xmlPath, const char* targetFile);
    
    /**
     *
     */
    void kwlBuildWaveBanks(const char* xmlPath, const char* targetDir);
    
    /**
     *
     */
    void kwlBuildWaveBank(const char* xmlPath, const char* waveBankId, const char* targetDir);

    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_XML_VALIDATION_H*/
