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

#include <math.h>
#include <string.h>

#include "kwl_assert.h"
#include "kwl_enginedata.h"
#include "kwl_memory.h"
#include "kwl_sounddefinition.h"

kwlError kwlEngineData_load(kwlEngineData* data, kwlInputStream* stream)
{
    if (data->isLoaded)
    {
        KWL_ASSERT(0 && "TODO: free current data");
    }
    
    /*check file identifier*/
    int i;
    for (i = 0; i < KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER_LENGTH; i++)
    {
        const char identifierChari = kwlInputStream_readChar(stream);
        if (identifierChari != KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER[i])
        {
            return KWL_UNKNOWN_FILE_FORMAT;
        }
    }
    
    /*Load chunks*/
    
    kwlEngineData_loadMixBusData(data, stream);
    kwlEngineData_loadMixPresetData(data, stream);
    kwlEngineData_loadWaveBankData(data, stream);
    
    /*must happen after wave bank loading*/
    kwlEngineData_loadSoundData(data, stream);
    
    /*must happen after sound, wave bank and mix bus loading.*/
    kwlEngineData_loadEventData(data, stream);
    
    data->isLoaded = 1;
    
    return KWL_NO_ERROR;
}

void kwlEngineData_unload(kwlEngineData* data)
{
    /* Unload any wave banks*/
    int i;
    for (i = 0; i < data->numWaveBanks; i++)
    {
        kwlWaveBank_unload(&data->waveBanks[i]);
    }
    
    /* Free non-audio data*/
    kwlEngineData_freeEventData(data);
    kwlEngineData_freeSoundData(data);
    kwlEngineData_freeMixPresetData(data);
    kwlEngineData_freeMixBusData(data);
    kwlEngineData_freeWaveBankData(data);
    
    data->isLoaded = 0;
}

kwlError kwlEngineData_loadMixBusData(kwlEngineData* data, kwlInputStream* stream)
{
    kwlInputStream_seekToEngineDataChunk(stream, KWL_MIX_BUSES_CHUNK_ID);
    KWL_ASSERT(data->mixBuses == NULL);
    
    /*allocate memory for the mix bus data*/
    const int numMixBuses = kwlInputStream_readIntBE(stream);
    KWL_ASSERT(numMixBuses > 0);
    data->numMixBuses = numMixBuses;
    data->mixBuses = 
    (kwlMixBus*)KWL_MALLOC(numMixBuses * sizeof(kwlMixBus), 
                           "kwlEngineData_loadMixBusData: mixer bus array");
    kwlMemset(data->mixBuses, 0, numMixBuses * sizeof(kwlMixBus));
    
    /*read mix bus data*/
    int i;
    for (i = 0; i < numMixBuses; i++)
    {
        kwlMixBus* const mixBusi = &data->mixBuses[i];
        kwlMixBus_init(mixBusi);
        
        mixBusi->id = kwlInputStream_readASCIIString(stream);
        if (strcmp(mixBusi->id, "master") == 0)
        {
            KWL_ASSERT(data->masterBus == NULL && "multiple master buses found");
            data->masterBus = mixBusi;
            data->masterBus->isMaster = 1;
        }
        
        const int numSubBuses = kwlInputStream_readIntBE(stream);
        KWL_ASSERT(numSubBuses >= 0);
        mixBusi->numSubBuses = numSubBuses;
        mixBusi->subBuses = NULL;
        if (numSubBuses > 0)
        {
            mixBusi->subBuses = (kwlMixBus**)KWL_MALLOC(numSubBuses * sizeof(kwlMixBus*), 
                                                        "kwlEngineData_loadMixBusData: sub buses");
            int j;
            for (j = 0; j < numSubBuses; j++)
            {
                const int subBusIndexj = kwlInputStream_readIntBE(stream);
                KWL_ASSERT(subBusIndexj >= 0 && subBusIndexj < numMixBuses);
                mixBusi->subBuses[j] = &data->mixBuses[subBusIndexj];
            }
        }
    }
    
    KWL_ASSERT(data->masterBus != NULL);
    
    return KWL_NO_ERROR;
}

void kwlEngineData_freeMixBusData(kwlEngineData* data)
{
    if (data->mixBuses == NULL)
    {
        return;
    }
    
    /*free the mix bus IDs*/
    const int numMixBuses = data->numMixBuses;
    int i;
    for (i = 0; i < numMixBuses; i++)
    {
        if (data->mixBuses[i].subBuses != NULL)
        {
            KWL_FREE(data->mixBuses[i].subBuses);
        }
        KWL_FREE(data->mixBuses[i].id);
    }
    
    /*free the mix bus array*/
    KWL_FREE(data->mixBuses);
    data->mixBuses = NULL;
    data->masterBus = NULL;
    data->mixBuses = NULL;;
    data->numMixBuses = 0;;
    data->masterBus = NULL;
}

kwlError kwlEngineData_loadMixPresetData(kwlEngineData* data, kwlInputStream* stream)
{
    kwlInputStream_seekToEngineDataChunk(stream, KWL_MIX_PRESETS_CHUNK_ID);
    KWL_ASSERT(data->mixBuses != 0); /*needed for mix bus lookup per param set*/
    
    /*allocate memory for the mix preset data*/
    const int numMixPresets = kwlInputStream_readIntBE(stream);
    KWL_ASSERT(numMixPresets > 0);
    data->numMixPresets = numMixPresets;
    const int numParameterSets = data->numMixBuses;
    int defaultPresetIndex = -1;
    data->mixPresets = (kwlMixPreset*)KWL_MALLOC(sizeof(kwlMixPreset) * numMixPresets,
                                                              "kwlEngineData_loadMixPresetData");
    
    /*read data*/
    int i;
    for (i = 0; i < numMixPresets; i++)
    {
        data->mixPresets[i].id = kwlInputStream_readASCIIString(stream);
        const int isDefault = kwlInputStream_readIntBE(stream);
        if (isDefault != 0)
        {
            KWL_ASSERT(defaultPresetIndex == -1 && "multiple default presets found");
            defaultPresetIndex = i;
            data->mixPresets[i].weight = 1.0f;
            data->mixPresets[i].targetWeight = 1.0f;
        }
        else
        {
            data->mixPresets[i].weight = 0.0f;
            data->mixPresets[i].targetWeight = 0.0f;
        }
        
        data->mixPresets[i].numParameterSets = numParameterSets;
        data->mixPresets[i].parameterSets = 
        (kwlMixBusParameters*)KWL_MALLOC(sizeof(kwlMixBusParameters) * numParameterSets, 
                                         "kwlEngineData_loadMixPresetData");
        int j;
        for (j = 0; j < numParameterSets; j++)
        {
            const int mixBusIndex = kwlInputStream_readIntBE(stream);
            KWL_ASSERT(mixBusIndex >= 0 &&  mixBusIndex < numParameterSets);
            data->mixPresets[i].parameterSets[j].mixBusIndex = mixBusIndex;
            data->mixPresets[i].parameterSets[j].logGainLeft = kwlInputStream_readFloatBE(stream);
            data->mixPresets[i].parameterSets[j].logGainRight = kwlInputStream_readFloatBE(stream);
            data->mixPresets[i].parameterSets[j].pitch = kwlInputStream_readFloatBE(stream);
        }
    }
    KWL_ASSERT(defaultPresetIndex >= 0);
    
    return KWL_NO_ERROR;
}

void kwlEngineData_freeMixPresetData(kwlEngineData* data)
{
    if (data->mixPresets == NULL)
    {
        return;
    }
    
    const int numMixPresets = data->numMixPresets;
    
    /*free any memory allocated per mix preset*/
    int i;
    for (i = 0; i < numMixPresets; i++)
    {
        KWL_FREE(data->mixPresets[i].id);
        KWL_FREE(data->mixPresets[i].parameterSets);
    }
    
    /*free the mix preset array*/
    KWL_FREE(data->mixPresets);
    data->mixPresets = NULL;
    data->numMixPresets = 0;
}

kwlError kwlEngineData_loadWaveBankData(kwlEngineData* data, kwlInputStream* stream)
{
    kwlInputStream_seekToEngineDataChunk(stream, KWL_WAVE_BANKS_CHUNK_ID);
    
    /*deserialize wave bank structures*/
    const int totalnumAudioDataEntries = kwlInputStream_readIntBE(stream);
    KWL_ASSERT(totalnumAudioDataEntries > 0);
    const int numWaveBanks = kwlInputStream_readIntBE(stream);
    KWL_ASSERT(numWaveBanks > 0);
    
    data->totalNumAudioDataEntries = totalnumAudioDataEntries;
    data->audioDataEntries = (kwlAudioData*)KWL_MALLOC(totalnumAudioDataEntries * sizeof(kwlAudioData), "kwlEngineData_loadWaveBankData");
    kwlMemset(data->audioDataEntries, 0, totalnumAudioDataEntries * sizeof(kwlAudioData)); 
    
    data->numWaveBanks = numWaveBanks;
    data->waveBanks = (kwlWaveBank*)KWL_MALLOC(numWaveBanks * sizeof(kwlWaveBank), "kwlEngineData_loadWaveBankData");
    kwlMemset(data->waveBanks, 0, numWaveBanks * sizeof(kwlWaveBank)); 
    
    int i;
    int audioDataItemIdx = 0;
    for (i = 0; i < numWaveBanks; i++)
    {
        kwlWaveBank* waveBanki = &data->waveBanks[i];
        waveBanki->id = kwlInputStream_readASCIIString(stream);
        const int numAudioDataEntries = kwlInputStream_readIntBE(stream);
        KWL_ASSERT(numAudioDataEntries > 0);
        waveBanki->numAudioDataEntries = numAudioDataEntries;
        waveBanki->audioDataItems = &data->audioDataEntries[audioDataItemIdx];
        int j;
        for (j = 0; j < numAudioDataEntries; j++)
        {
            data->audioDataEntries[audioDataItemIdx].filePath = kwlInputStream_readASCIIString(stream);
            data->audioDataEntries[audioDataItemIdx].waveBank = waveBanki;
            audioDataItemIdx++;
        }
    }
    
    return KWL_NO_ERROR;
}

void kwlEngineData_freeWaveBankData(kwlEngineData* data)
{
    if (data->waveBanks != NULL)
    {
        const int numWaveBanks = data->numWaveBanks;
        int i;
        for (i = 0; i < numWaveBanks; i++)
        {
            KWL_FREE((void*)data->waveBanks[i].id);
        }
        KWL_FREE(data->waveBanks);
        data->waveBanks = NULL;
    }
    
    if (data->audioDataEntries != NULL)
    {
        const int numAudioDataEntries = data->totalNumAudioDataEntries;
        int i;
        for (i = 0; i < numAudioDataEntries; i++)
        {
            KWL_FREE((void*)data->audioDataEntries[i].filePath);
        }
        KWL_FREE(data->audioDataEntries);
        data->audioDataEntries = NULL;
    }
}

kwlError kwlEngineData_loadSoundData(kwlEngineData* data, kwlInputStream* stream)
{
    kwlInputStream_seekToEngineDataChunk(stream, KWL_SOUNDS_CHUNK_ID);
    
    /*allocate memory for sound definitions*/
    const int numSoundDefinitions = kwlInputStream_readIntBE(stream);
    KWL_ASSERT(numSoundDefinitions >= 0 && "the number of sound definitions must be non-negative");
    data->numSoundDefinitions = numSoundDefinitions;
    data->sounds = (kwlSoundDefinition*)KWL_MALLOC(numSoundDefinitions * sizeof(kwlSoundDefinition), "sound definitions");
    kwlMemset(data->sounds, 0, numSoundDefinitions * sizeof(kwlSoundDefinition));
    
    /*read sound definitions*/
    int i;
    for (i = 0; i < numSoundDefinitions; i++)
    {
        kwlSoundDefinition_init(&data->sounds[i]);
        data->sounds[i].playbackCount = kwlInputStream_readIntBE(stream);
        data->sounds[i].deferStop = kwlInputStream_readIntBE(stream);
        data->sounds[i].gain = kwlInputStream_readFloatBE(stream);
        data->sounds[i].gainVariation = kwlInputStream_readFloatBE(stream);
        data->sounds[i].pitch = kwlInputStream_readFloatBE(stream);
        data->sounds[i].pitchVariation = kwlInputStream_readFloatBE(stream);
        data->sounds[i].playbackMode = (kwlSoundPlaybackMode)kwlInputStream_readIntBE(stream);
        
        const int numWaveReferences = kwlInputStream_readIntBE(stream);
        KWL_ASSERT(numWaveReferences > 0);
        data->sounds[i].audioDataEntries = (kwlAudioData**)KWL_MALLOC(numWaveReferences * sizeof(kwlAudioData*), 
                                                                                   "kwlEngineData_loadSoundData: wave list");
        data->sounds[i].numAudioDataEntries = numWaveReferences;
        
        int j;
        for (j = 0; j < numWaveReferences; j++)
        {
            const int waveBankIndex = kwlInputStream_readIntBE(stream);
            KWL_ASSERT(waveBankIndex >= 0 && waveBankIndex < data->numWaveBanks);
            kwlWaveBank* waveBank = &data->waveBanks[waveBankIndex];
            
            const int audioDataIndex = kwlInputStream_readIntBE(stream);
            KWL_ASSERT(audioDataIndex >= 0 && audioDataIndex < waveBank->numAudioDataEntries);
            
            KWL_ASSERT(data->audioDataEntries != NULL);
            data->sounds[i].audioDataEntries[j] = &waveBank->audioDataItems[audioDataIndex];
        }
    }
    
    return KWL_NO_ERROR;
}

void kwlEngineData_freeSoundData(kwlEngineData* data)
{
    if (data->sounds == NULL)
    {
        return;
    }
    
    int i;
    for (i = 0; i < data->numSoundDefinitions; i++)
    {
        KWL_FREE(data->sounds[i].audioDataEntries);
    }
    KWL_FREE(data->sounds);    
    data->sounds = NULL;
    data->numSoundDefinitions = 0;
    
}

kwlError kwlEngineData_loadEventData(kwlEngineData* data, kwlInputStream* stream)
{
    kwlInputStream_seekToEngineDataChunk(stream, KWL_EVENTS_CHUNK_ID);
    KWL_ASSERT(data->sounds != NULL);
    KWL_ASSERT(data->events == NULL);
    KWL_ASSERT(data->eventDefinitions == NULL);
    
    /*read the total number of event definitions*/
    const int numEventDefinitions = kwlInputStream_readIntBE(stream);
    KWL_ASSERT(numEventDefinitions > 0);
    data->numEventDefinitions = numEventDefinitions;
    data->events = 
    (kwlEventInstance**)KWL_MALLOC(numEventDefinitions * sizeof(kwlEventInstance*), "kwlEngineData_loadEventData");
    kwlMemset(data->events, 0, numEventDefinitions * sizeof(kwlEventInstance*));
    data->eventDefinitions = 
    (kwlEventDefinition*)KWL_MALLOC(numEventDefinitions * sizeof(kwlEventDefinition), "kwlEngineData_loadEventData");
    kwlMemset(data->eventDefinitions, 0, numEventDefinitions * sizeof(kwlEventDefinition));
    
    int i;
    for (i = 0; i < numEventDefinitions; i++)
    {
        kwlEventDefinition* definitioni = &data->eventDefinitions[i];
        /*read the id of this event definition*/
        definitioni->id = kwlInputStream_readASCIIString(stream);
        
        const int instanceCount = kwlInputStream_readIntBE(stream);
        KWL_ASSERT(instanceCount >= -1);
        definitioni->instanceCount = instanceCount;
        const int numInstancesToAllocate = instanceCount < 1 ? 1 : instanceCount;
        data->events[i] = 
        (kwlEventInstance*)KWL_MALLOC(numInstancesToAllocate * sizeof(kwlEventInstance), "kwlEngineData_loadEventData");
        kwlMemset(data->events[i], 0, numInstancesToAllocate * sizeof(kwlEventInstance));
        
        definitioni->gain = kwlInputStream_readFloatBE(stream);
        definitioni->pitch = kwlInputStream_readFloatBE(stream);
        const float degToRad = 0.0174532925199433;
        float innerConeAngleRad = degToRad * kwlInputStream_readFloatBE(stream);
        float outerConeAngleRad = degToRad * kwlInputStream_readFloatBE(stream);
        definitioni->innerConeCosAngle = cosf(innerConeAngleRad / 2.0f);
        definitioni->outerConeCosAngle = cosf(outerConeAngleRad / 2.0f);
        definitioni->outerConeGain = kwlInputStream_readFloatBE(stream);
        
        kwlEventInstance_init(&data->events[i][0]);
        data->events[i][0].definition_engine = definitioni;
        data->events[i][0].definition_mixer = definitioni;
        
        /*read the index of the mix bus that this event belongs to*/
        const int mixBusIndex = kwlInputStream_readIntBE(stream);
        KWL_ASSERT(mixBusIndex >= 0 && mixBusIndex < data->numMixBuses);
        definitioni->mixBus = &data->mixBuses[mixBusIndex];
        definitioni->isPositional = kwlInputStream_readIntBE(stream);
        KWL_ASSERT(definitioni->isPositional == 0 || 
                   definitioni->isPositional == 1);
        
        /*read the index of the sound referenced by this event (ignored for streaming events)*/
        const int soundIndex = kwlInputStream_readIntBE(stream);
        if (soundIndex == -1)
        {
            /*streaming event, no sound.*/
            definitioni->sound = NULL;
        }
        else
        {
            KWL_ASSERT(soundIndex >= 0 && soundIndex < data->numSoundDefinitions);
            definitioni->sound = &data->sounds[soundIndex];
        }
        
        /*read the event retrigger mode (ignored for streaming events)*/
        definitioni->retriggerMode = (kwlEventRetriggerMode)kwlInputStream_readIntBE(stream);
        KWL_ASSERT(definitioni->retriggerMode <= 1 && definitioni->retriggerMode >= 0);
        
        /*read the index of the audio data referenced by this event (only used for streaming events)*/
        const int waveBankIndex = kwlInputStream_readIntBE(stream);
        const int audioDataIndex = kwlInputStream_readIntBE(stream);
        if (audioDataIndex >= 0 && audioDataIndex < data->totalNumAudioDataEntries &&
            waveBankIndex >=0 && waveBankIndex < data->numWaveBanks)
        {
            definitioni->streamAudioData = &data->waveBanks[waveBankIndex].audioDataItems[audioDataIndex];
        }
        else
        {
            definitioni->streamAudioData = NULL;
        }
        
        /*read loop flag (ignored for non-streaming events)*/
        const int loopIfStreaming = kwlInputStream_readIntBE(stream);
        definitioni->loopIfStreaming = loopIfStreaming;
        
        /*read referenced wave banks*/
        definitioni->numReferencedWaveBanks = kwlInputStream_readIntBE(stream);
        KWL_ASSERT(definitioni->numReferencedWaveBanks < 10000 && definitioni->numReferencedWaveBanks >= 0);
        definitioni->referencedWaveBanks = 
        (kwlWaveBank**)KWL_MALLOC(definitioni->numReferencedWaveBanks * sizeof(kwlWaveBank*), "wave bank refs");
        int j;
        for (j = 0; j < definitioni->numReferencedWaveBanks; j++)
        {
            int waveBankIndex = kwlInputStream_readIntBE(stream);
            KWL_ASSERT(waveBankIndex >= 0 && waveBankIndex < data->numWaveBanks);
            definitioni->referencedWaveBanks[j] = &data->waveBanks[waveBankIndex];
        }
        
        /*copy the first event instance to the other slots.*/
        for (j = 1; j < numInstancesToAllocate; j++)
        {
            kwlMemcpy(&data->events[i][j], &data->events[i][0], sizeof(kwlEventInstance));
        }
    }
    
    return KWL_NO_ERROR;
    
}

void kwlEngineData_freeEventData(kwlEngineData* data)
{
    if (data->events == NULL && 
        data->eventDefinitions == NULL)
    {
        return;
    }
    
    const int numEventDefinitions = data->numEventDefinitions;
    
    int i;
    for (i = 0; i < numEventDefinitions; i++)
    {
        kwlEventDefinition* defi = &data->eventDefinitions[i];
        KWL_FREE(data->events[i]);
        KWL_FREE(defi->referencedWaveBanks);
        KWL_FREE(defi->id);
    }
    
    KWL_FREE(data->events);
    data->events = NULL;
    KWL_FREE(data->eventDefinitions);
    data->eventDefinitions = NULL;
    data->numEventDefinitions = 0;
}
