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

#include "kwl_logging.h"
#include "kwl_enginedata.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
    /**
     *
     */
    typedef struct kwlMixBusChunk
    {
        char* id;
        int numSubBuses;
        int* subBusIndices;
    } kwlMixBusChunk;
    
    /**
     *
     */
    typedef struct kwlMixBusDataChunk
    {
        int chunkId;
        int chunkSize;
        int numMixBuses;
        kwlMixBusChunk* mixBuses;
    } kwlMixBusDataChunk;
    
    /**
     *
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
     *
     */
    typedef struct kwlMixPresetDataChunk
    {
        int chunkId;
        int chunkSize;
        int numMixPresets;
        kwlMixPresetChunk* mixPresets;
    } kwlMixPresetDataChunk;
    
    /**
     *
     */
    typedef struct kwlWaveBankChunk
    {
        char* id;
        int numAudioDataEntries;
        char** audioDataEntries;
    } kwlWaveBankChunk;
    
    /**
     *
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
     *
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
     *
     */
    typedef struct kwlSoundDataChunk
    {
        int chunkId;
        int chunkSize;
        int numSoundDefinitions;
        kwlSoundChunk* soundDefinitions;
    } kwlSoundDataChunk;
    
    /**
     *
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
     *
     */
    typedef struct kwlEventDataChunk
    {
        int chunkId;
        int chunkSize;
        int numEventDefinitions;
        kwlEventChunk* eventDefinitions;
    } kwlEventDataChunk;
    
    /**
     * 
     */
    typedef struct kwlProjectDataBinary
    {
        char fileIdentifier[KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER_LENGTH];
        kwlMixPresetDataChunk mixPresetChunk;
        kwlMixBusDataChunk mixBusChunk;
        kwlWaveBankDataChunk waveBankChunk;
        kwlSoundDataChunk soundChunk;
        kwlEventDataChunk eventChunk;
        
    } kwlProjectDataBinary;
    
    /**
     *
     */
    void kwlProjectDataBinary_writeToBinary(kwlProjectDataBinary* bin,
                                            const char* binPath);
    
    /**
     *
     */
    void kwlProjectDataBinary_loadFromXML(kwlProjectDataBinary* bin,
                                                        const char* xmlPath,
                                                        const char* xsdPath,
                                                        kwlLogCallback errorLogCallback);
    
    /**
     *
     */
    void kwlProjectDataBinary_loadFromBinary(kwlProjectDataBinary* bin,
                                                           const char* binPath,
                                                           kwlLogCallback errorLogCallback);
    
    /**
     *
     */
    void kwlProjectDataBinary_free(kwlProjectDataBinary* bin);
    
    /**
     *
     */
    void kwlProjectDataBinary_dump(kwlProjectDataBinary* bin,
                                                 kwlLogCallback logCallback);
    
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_PROJECT_DATA_BINARY_REPRESENTATION_H*/
