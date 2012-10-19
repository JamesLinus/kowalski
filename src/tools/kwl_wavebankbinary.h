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
     * A wave bank binary entry, corresponding to a pice of audio data.
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
     * A wave bank binary, i.e a struct representation of a wave bank binary file.
     */
    typedef struct kwlWaveBankBinary
    {
        char fileIdentifier[KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH];
        char* id;
        int numEntries;
        kwlWaveBankEntryChunk* entries;
        
    } kwlWaveBankBinary;
    
    /**
     * Writes a given wave bank binary to a file.
     * @param bin The wave bank binary to write.
     * @param path The path of the target file.
     */
    void kwlWaveBankBinary_writeToFile(kwlWaveBankBinary* bin,
                                       const char* path);
    
    /**
     * Creates a wave bank binary corresponding to a wave bank binary file.
     * @param bin The wave bank binary to populate with data.
     * @param path The path of the wave bank binary file.
     * @param errorCallback A callback used for logging any error messages.
     * @return An error code.
     */
    kwlDataValidationResult kwlWaveBankBinary_loadFromBinaryFile(kwlWaveBankBinary* bin,
                                                             const char* path,
                                                             kwlLogCallback errorLogCallback);
    
    /**
     * Creates a wave bank binary corresponding to a wavebank with a given id in
     * a given project data XML file.
     * @param bin The wave bank binary to populate with data.
     * @param xmlPath The path to the project data XML file.
     * @param xsdPath The path to the XML schema describing the structure of project XML data.
     * @param waveBankId The id of the wave bank to load.
     * @param errorLogCallback A callback used for logging any error messages.
     * @return An error code.
     */
    kwlDataValidationResult kwlWaveBankBinary_loadFromXML(kwlWaveBankBinary* bin,
                                                          const char* xmlPath,
                                                          const char* xsdPath,
                                                          const char* waveBankId,
                                                          kwlLogCallback errorLogCallback);
    
    /**
     * Prints the contents of a given wave bank binary.
     * @param bin The wave bank binary to print.
     * @param logCallback The logging callback to print to.
     */
    void kwlWaveBankBinary_dump(kwlWaveBankBinary* bin,
                                kwlLogCallback logCallback);
    
    /**
     * Releases all memory, if any, associated with a given wave bank binary.
     * @param bin The wave bank binary to free.
     */
    void kwlWaveBankBinary_free(kwlWaveBankBinary* bin);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_WAVE_BANK_BINARY_REPRESENTATION_H*/
