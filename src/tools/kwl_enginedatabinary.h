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

#include <libxml/tree.h>

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
    typedef struct kwlMixBusesChunk
    {
        int chunkId;
        int chunkSize;
        int numMixBuses;
        kwlMixBusChunk* mixBuses;
    } kwlMixBusesChunk;
    
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
    typedef struct kwlMixPresetsChunk
    {
        int chunkId;
        int chunkSize;
        int numMixPresets;
        kwlMixPresetChunk* mixPresets;
    } kwlMixPresetsChunk;
    
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
    typedef struct kwlWaveBanksChunk
    {
        int chunkId;
        int chunkSize;
        int numAudioDataItemsTotal;
        int numWaveBanks;
        kwlWaveBankChunk* waveBanks;
    } kwlWaveBanksChunk;
    
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
    typedef struct kwlSoundsChunk
    {
        int chunkId;
        int chunkSize;
        int numSoundDefinitions;
        kwlSoundChunk* soundDefinitions;
    } kwlSoundsChunk;
    
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
        float outerConeAngleDeg;
        float outerConeGain;
        int mixBusIndex;
        int isPositional;
        int soundIndex;
        int retriggerMode;
        int instanceStealingMode;
        int waveBankIndex;
        int audioDataIndex;
        int loopIfStreaming;
        int numReferencedWaveBanks;
        int* waveBankIndices;
    } kwlEventChunk;
    
    /**
     * The event chunk of an engine data binary.
     */
    typedef struct kwlEventsChunk
    {
        int chunkId;
        int chunkSize;
        int numEventDefinitions;
        kwlEventChunk* eventDefinitions;
    } kwlEventsChunk;
    
    /**
     * A struct representation of an engine data binary file.
     */
    typedef struct kwlEngineDataBinary
    {
        /** The engine data binary file identifier. */
        char fileIdentifier[KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER_LENGTH];
        /** Wave bank data.*/
        kwlWaveBanksChunk waveBanksChunk;
        /** Mix bus data.*/
        kwlMixBusesChunk mixBusesChunk;
        /** Mix preset data.*/
        kwlMixPresetsChunk mixPresetsChunk;
        /** Sound definition data.*/
        kwlSoundsChunk soundsChunk;
        /** Event definition data.*/
        kwlEventsChunk eventsChunk;
        
    } kwlEngineDataBinary;
    
    /**
     * Checks if a file is an engine data binary.
     * @param path The path of the file to check.
     * @return Non-zero if \c path corresponds to a valid
     * engine data binary file, zero otherwise.
     */
    int kwlFileIsEngineDataBinary(const char* path);
    
    /**
     * Checks if the physical audio files referenced from a given engine data binary exist.
     * @param bin The binary to check.
     * @param xmlPath The path of the project data XML file (needed to resolve paths
     * relative to the this file).
     * @param audioFileRoot The audio file root folder.
     * @param rootIsRelative If non-zero, the \c audioRootFolder path is relative to \c xmlPath,
     * otherwise \c audioRootFolder is an absolute path and \c xmlPath is ignored.
     * @param errorLogCallback A callback used to print any errors.
     * @return A result code.
     */
    kwlResultCode kwlEngineDataBinary_validateFileReferences(kwlEngineDataBinary* bin,
                                                             const char* xmlPath,
                                                             const char* audioFileRoot,
                                                             int rootIsRelative,
                                                             kwlLogCallback errorLogCallback);
    
    /**
     * Writes a given engine data binary to a file.
     * @param bin The binary to write.
     * @param binPath The path to write the file to.
     * @return A result code.
     */
    kwlResultCode kwlEngineDataBinary_writeToFile(kwlEngineDataBinary* bin,
                                                  const char* binPath);
    
    /**
     * Loads an engine data binary from a given project data XML file.
     * @param bin The binary to load data into.
     * @param xmlPath The path to the project XML data file.
     * @param xmlPath The path to the project XML data schema.
     * @param validateAudioFileReferences If non-zero, loading will fail if
     * the project data XML file contains invalid references to physical audio files,
     * otherwise loading will only fail on errors in the XML file.
     * @param errorLogCallback Any errors are printed to this callback.
     * @return A result code.
     */
    kwlResultCode kwlEngineDataBinary_loadFromXMLFile(kwlEngineDataBinary* bin,
                                                      const char* xmlPath,
                                                      const char* xsdPath,
                                                      int validateAudioFileReferences,
                                                      kwlLogCallback errorLogCallback);
    
    /**
     * Loads an engine data binary from a given libxml XML document.
     * This function assumes that \c document has been properly validated.
     * @param bin The binary to load data into.
     * @param xmlPath The path to the project XML data file.
     * @param document A libxml document pointer representing a valid project data XML file.
     * @param errorLogCallback Any errors are printed to this callback.
     * @return A result code.
     */
    kwlResultCode kwlEngineDataBinary_loadFromXMLDocument(kwlEngineDataBinary* bin,
                                                          const char* xmlPath,
                                                          xmlDoc* document,
                                                          kwlLogCallback errorLogCallback);
    
    /**
     * Loads an engine data binary from a given binary file.
     * @param bin The binary to load data into.
     * @param binPath The path to the engine data binary file to load.
     * @param errorLogCallback Any errors are printed to this callback.
     * @return A result code.
     */
    kwlResultCode kwlEngineDataBinary_loadFromBinaryFile(kwlEngineDataBinary* bin,
                                                         const char* binPath,
                                                         kwlLogCallback errorLogCallback);
    
    /**
     * Frees all memory associated with a given engine data binary.
     * @param The binary to free.
     */
    void kwlEngineDataBinary_free(kwlEngineDataBinary* bin);
    
    /**
     * Prints the contents of a given engine data binary.
     * @param The binary to view.
     * @param logCallback A callback that handles the printing.
     */
    void kwlEngineDataBinary_dump(kwlEngineDataBinary* bin,
                                  kwlLogCallback logCallback);
    
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /*KWL_PROJECT_DATA_BINARY_REPRESENTATION_H*/
