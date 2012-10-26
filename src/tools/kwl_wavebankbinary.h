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

#ifndef KWL_WAVE_BANK_BINARY_H
#define KWL_WAVE_BANK_BINARY_H

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
     * A struct representation of a wave bank binary file.
     */
    typedef struct kwlWaveBankBinary
    {
        /** The wave bank binary file identifier.*/
        char fileIdentifier[KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH];
        /** The id of the wave bank.*/
        char* id;
        /** The number of audio data entries*/
        int numEntries;
        /** The audio data entries.*/
        kwlWaveBankEntryChunk* entries;
        
    } kwlWaveBankBinary;
    
    /**
     * Determines if a given file is a wave bank binary file.
     * @param path The path to the file to check.
     * @return Zero if the file is not a wave bank binary, non-zero otherwise.
     */
    int kwlFileIsWaveBankBinary(const char* path);
    
    /**
     * Writes a given wave bank binary to a file.
     * @param bin The wave bank binary to write.
     * @param path The path of the target file.
     */
    void kwlWaveBankBinary_writeToFile(kwlWaveBankBinary* bin,
                                       const char* path);
    
    /**
     * Loads a wave bank binary file (including audio data) into into a \kwlWaveBankBinary struct.
     * @param bin The wave bank binary to load data into.
     * @param path The path of the wave bank binary file.
     * @param errorCallback A callback used for logging any error messages.
     * @return An error code.
     */
    kwlResultCode kwlWaveBankBinary_loadFromBinaryFile(kwlWaveBankBinary* bin,
                                                       const char* path,
                                                       kwlLogCallback errorLogCallback);
    
    /**
     * Creates a wave bank binary corresponding to a specific wave bank entry
     * in an engine data binary.
     * @param wbBin The wave bank binary to load data into.
     * @param edBin The engine data binary.
     * @param projNode A pointer to the KowalskiProject XML node of a valid project data document.
     * @param xmlPath The path to the project data XML document
     * @param audioFileRoot The path to the audio file root directory.
     * @param rootIsRelative Non-zero if \c audioFileRoot is a relative path, zero if it's absolute.
     * @param waveBankId The node path of the wave bank to build.
     * @param errorLogCallback Any errors are printed using this callback.
     * @return A result code.
     */
    kwlResultCode kwlWaveBankBinary_create(kwlWaveBankBinary* wbBin,
                                           kwlEngineDataBinary* edBin,
                                           xmlNode* projNode,
                                           const char* xmlPath,
                                           const char* audioFileRoot,
                                           int rootIsRelative,
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


#endif /*KWL_WAVE_BANK_BINARY_H*/
