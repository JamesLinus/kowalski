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

#ifndef KWL_PROJECT_DATA_BINARY_REPRESENTATION_H
#define KWL_PROJECT_DATA_BINARY_REPRESENTATION_H

#include "kwl_datavalidation.h"
#include "kwl_enginedata.h"
#include "kwl_logging.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
    /**
     * A single mix bus in the mix bus chunk of an engine data binary.
     */
    typedef struct kwlMixBusChunk
    {
        char* id;
        int numSubBuses;
        int* subBusIndices;
    } kwlMixBusChunk;
    
    /**
     * The mix bus chunk of an engine data binary.
     */
    typedef struct kwlMixBusDataChunk
    {
        int chunkId;
        int chunkSize;
        int numMixBuses;
        kwlMixBusChunk* mixBuses;
    } kwlMixBusDataChunk;
    
    /**
     * A single mix preset in the mix   chunk of an engine data binary.
     */
    typedef struct kwlMixPresetChunk
    {
        char* id;
        int isDefault;
        float* gainLeft;
        float* gainRight;
        float* pitch;
        int* mixBusIndices;
    } kwlMixPresetChunk;
    
    /**
     * The mix preset chunk of an engine data binary.
     */
    typedef struct kwlMixPresetDataChunk
    {
        int chunkId;
        int chunkSize;
        int numMixPresets;
        kwlMixPresetChunk* mixPresets;
    } kwlMixPresetDataChunk;
    
    /**
     * A single wave bank in the wave bank chunk of an engine data binary.
     */
    typedef struct kwlWaveBankChunk
    {
        char* id;
        int numAudioDataEntries;
        char** audioDataEntries;
    } kwlWaveBankChunk;
    
    /**
     * The wave bank chunk of an engine data binary.
     */
    typedef struct kwlWaveBankDataChunk
    {
        int chunkId;
        int chunkSize;
        int numAudioDataItemsTotal;
        int numWaveBanks;
        kwlWaveBankChunk* waveBanks;
    } kwlWaveBankDataChunk;
    
    /**
     * A single sound definition in the sound chunk of an engine data binary.
     */
    typedef struct kwlSoundChunk
    {
        int playbackCount;
        int deferStop;
        float gain;
        float gainVariation;
        float pitch;
        float pitchVariation;
        int playbackMode;
        
        int numWaveReferences;
        int* waveBankIndices;
        int* audioDataIndices;
    } kwlSoundChunk;
    
    /**
     * The sound chunk of an engine data binary.
     */
    typedef struct kwlSoundDataChunk
    {
        int chunkId;
        int chunkSize;
        int numSoundDefinitions;
        kwlSoundChunk* soundDefinitions;
    } kwlSoundDataChunk;
    
    /**
     * A single event definition in the event chunk of an engine data binary.
     */
    typedef struct kwlEventChunk
    {
        char* id;
        int instanceCount;
        float gain;
        float pitch;
        float innerConeAngleDeg;
        float innerConeGain;
        float outerConeAngleDeg;
        float outerConeGain;
        int mixBusIndex;
        int isPositional;
        int soundIndex;
        int retriggerMode;
        int waveBankIndex;
        int audioDataIndex;
        int loopIfStreaming;
        int numReferencedWaveBanks;
        int* waveBankIndices;
    } kwlEventChunk;
    
    /**
     * The event chunk of an engine data binary.
     */
    typedef struct kwlEventDataChunk
    {
        int chunkId;
        int chunkSize;
        int numEventDefinitions;
        kwlEventChunk* eventDefinitions;
    } kwlEventDataChunk;
    
    /**
     * An engine data binary, i.e a struct representation of an engine data binary file.
     */
    typedef struct kwlEngineDataBinary
    {
        char fileIdentifier[KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER_LENGTH];
        kwlMixPresetDataChunk mixPresetChunk;
        kwlMixBusDataChunk mixBusChunk;
        kwlWaveBankDataChunk waveBankChunk;
        kwlSoundDataChunk soundChunk;
        kwlEventDataChunk eventChunk;
        
    } kwlEngineDataBinary;
    
    /**
     *
     */
    int kwlFileIsEngineDataBinary(const char* path);
    
    /**
     *
     */
    kwlResultCode kwlEngineDataBinary_writeToFile(kwlEngineDataBinary* bin,
                                                  const char* binPath);
    
    /**
     *
     */
    kwlResultCode kwlEngineDataBinary_loadFromXML(kwlEngineDataBinary* bin,
                                                  const char* xmlPath,
                                                  const char* xsdPath,
                                                  int validateAudioFileReferences,
                                                  kwlLogCallback errorLogCallback);
    
    /**
     *
     */
    kwlResultCode kwlEngineDataBinary_loadFromBinaryFile(kwlEngineDataBinary* bin,
                                                         const char* binPath,
                                                         kwlLogCallback errorLogCallback);
    
    /**
     *
     */
    void kwlEngineDataBinary_free(kwlEngineDataBinary* bin);
    
    /**
     *
     */
    void kwlEngineDataBinary_dump(kwlEngineDataBinary* bin,
                                  kwlLogCallback logCallback);
    
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_PROJECT_DATA_BINARY_REPRESENTATION_H*/
