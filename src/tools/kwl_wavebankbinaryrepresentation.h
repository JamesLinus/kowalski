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

#ifndef KWL_WAVE_BANK_BINARY_REPRESENTATION_H
#define KWL_WAVE_BANK_BINARY_REPRESENTATION_H

#include "kwl_logging.h"
#include "kwl_wavebank.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *
     */
    typedef struct kwlWaveBankEntryChunk
    {
        char* fileName;
        int encoding;
        int isStreaming;
        int numChannels;
        int numBytes;
        void* data;
    } kwlWaveBankEntryChunk;
    
    /**
     *
     */
    typedef struct kwlWaveBankBinaryRepresentation
    {
        char fileIdentifier[KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH];
        char* id;
        int numEntries;
        kwlWaveBankEntryChunk* entries;
        
    } kwlWaveBankBinaryRepresentation;
    
    /**
     *
     */
    void kwlWaveBankBinaryRepresentation_writeToBinary(kwlWaveBankBinaryRepresentation* bin,
                                                      const char* path);
    
    /**
     * 
     */
    void kwlWaveBankBinaryRepresentation_loadFromBinary(kwlWaveBankBinaryRepresentation* bin,
                                                        const char* path,
                                                        kwlLogCallback errorLogCallback);
    
    /**
     *
     */
    void kwlWaveBankBinaryRepresentation_loadFromXML(kwlWaveBankBinaryRepresentation* bin,
                                                     const char* xmlPath,
                                                     const char* xsdPath,
                                                     const char* waveBankId,
                                                     kwlLogCallback errorLogCallback);
    
    /**
     *
     */
    void kwlWaveBankBinaryRepresentation_dump(kwlWaveBankBinaryRepresentation* bin,
                                              kwlLogCallback logCallback);
    
    /**
     *
     */
    void kwlWaveBankBinaryRepresentation_free(kwlWaveBankBinaryRepresentation* bin);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_WAVE_BANK_BINARY_REPRESENTATION_H*/
