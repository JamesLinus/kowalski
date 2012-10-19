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

#ifndef KWL__ENGINE_DATA_H
#define KWL__ENGINE_DATA_H

/*! \file */ 

#include "kwl_audiodata.h"
#include "kwl_mixbus.h"
#include "kwl_mixpreset.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

struct kwlWaveBank;
    
/** The number of bytes in the engine data file identifier.*/
#define KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER_LENGTH 9
    
/** The ID of the event data chunk in an engine data binary file. */
#define KWL_EVENTS_CHUNK_ID 0x73747665
    
/** The ID of the sound data chunk in an engine data binary file. */
#define KWL_SOUNDS_CHUNK_ID 0x73646e73
    
/** The ID of the mix bus data chunk in an engine data binary file. */
#define KWL_MIX_BUSES_CHUNK_ID 0x7362786d
    
/** The ID of the mix preset data chunk in an engine data binary file. */
#define KWL_MIX_PRESETS_CHUNK_ID 0x7270786d
    
/** The ID of the wave bank data chunk in an engine data binary file. */
#define KWL_WAVE_BANKS_CHUNK_ID 0x736b6277
    
/** 
 * The file identifier for engine binaries, ie the sequence of bytes
 * that all engine data binary files start with.
 */
static const char KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER[KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER_LENGTH] =
{
    0xAB, 'K', 'W', 'L', 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
};
    
    
/**
 * A struct containing engine data loaded from a binary file.
 */
typedef struct kwlEngineData
{
    /** Zero if no engine data is loaded, non-zero otherwise. */
    int isLoaded;
    /** The number of mix buses.*/
    int numMixBuses;
    /** */
    kwlMixBus* mixBuses;
    /** */
    kwlMixBus* masterBus;
    /** The number of wave banks */
    int numWaveBanks;
    /** An array of wave banks */
    struct kwlWaveBank* waveBanks;
    
    /** The number of mix presets. */
    int numMixPresets;
    /** An array of mix presets. */
    kwlMixPreset* mixPresets;
    /** The number of seconds it takes to fade between mix presets.*/
    float mixPresetFadeTime;
    
    /** The total number of audio data entries. */
    int totalNumAudioDataEntries;
    /** An array of audio data entries that may or may not contain loaded audio data. */
    kwlAudioData* audioDataEntries;
    
    /** The current number of event definitions read from engine data.*/
    int numEventDefinitions;
    /** An array of event definitions read from data. */
    struct kwlEventDefinition* eventDefinitions;
    /** An array of arrays of event instances read from data. Instance i of event definition j is at [j][i]. */
    struct kwlEventInstance** events;
    
    /** The number of sound definitions currently loaded from engine data.*/
    int numSoundDefinitions;
    /** An array of sound definitions. */
    struct kwlSoundDefinition* sounds;
    
} kwlEngineData;

/** */
kwlError kwlEngineData_load(kwlEngineData* data, kwlInputStream* stream);

/** */
void kwlEngineData_unload(kwlEngineData* data);
    
/** */
kwlError kwlEngineData_loadMixBusData(kwlEngineData* data, kwlInputStream* stream);

/** */
void kwlEngineData_freeMixBusData(kwlEngineData* data);

/** */
kwlError kwlEngineData_loadMixPresetData(kwlEngineData* data, kwlInputStream* stream);

/** */
void kwlEngineData_freeMixPresetData(kwlEngineData* data);

/** */
kwlError kwlEngineData_loadWaveBankData(kwlEngineData* data, kwlInputStream* stream);

/** */
void kwlEngineData_freeWaveBankData(kwlEngineData* data);

/** */
kwlError kwlEngineData_loadSoundData(kwlEngineData* data, kwlInputStream* stream);

/** */
void kwlEngineData_freeSoundData(kwlEngineData* data);

/** */
kwlError kwlEngineData_loadEventData(kwlEngineData* data, kwlInputStream* stream);

/** */
void kwlEngineData_freeEventData(kwlEngineData* data);


#ifdef __cplusplus
}
#endif /* __cplusplus */    

#endif /*KWL__ENGINE_DATA_H*/
