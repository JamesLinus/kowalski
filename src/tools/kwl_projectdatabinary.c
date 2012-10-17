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

#include <stdio.h>

#include "kwl_projectdatabinary.h"
#include "kwl_inputstream.h"
#include "kwl_memory.h"
#include "kwl_sound.h"
#include "kwl_toolsutil.h"

void kwlProjectDataBinary_load(kwlProjectDataBinary* bin, const char* engineDataPath)
{
    kwlProjectDataBinary binaryRep;
    kwlMemset(&binaryRep, 0, sizeof(kwlProjectDataBinary));
    
    kwlLogCallback errorLogCallback = kwlDefaultLogCallback;
    
    kwlInputStream is;
    kwlError result = kwlInputStream_initWithFile(&is, engineDataPath);
    if (result != KWL_NO_ERROR)
    {
        //error loading file
        errorLogCallback("Could not open engine data binary %s\n", engineDataPath);
        return;
    }
    
    /*check file identifier*/
    for (int i = 0; i < KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER_LENGTH; i++)
    {
        const char identifierChari = kwlInputStream_readChar(&is);
        binaryRep.fileIdentifier[i] = identifierChari;
        if (identifierChari != KWL_ENGINE_DATA_BINARY_FILE_IDENTIFIER[i])
        {
            //invalid file format
            kwlInputStream_close(&is);
            errorLogCallback("Invalid engine data binary file header\n");
            return;
        }
    }
    
    //mix buses
    {
        binaryRep.mixBusChunk.chunkId = KWL_MIX_BUSES_CHUNK_ID;
        binaryRep.mixBusChunk.chunkSize = kwlInputStream_seekToEngineDataChunk(&is, KWL_MIX_BUSES_CHUNK_ID);
        
        //allocate memory for the mix bus data
        binaryRep.mixBusChunk.numMixBuses = kwlInputStream_readIntBE(&is);
        binaryRep.mixBusChunk.mixBuses = KWL_MALLOCANDZERO(binaryRep.mixBusChunk.numMixBuses * sizeof(kwlMixBusChunk),
                                                           "bin mix buses");
        
        
        //read mix bus data
        for (int i = 0; i < binaryRep.mixBusChunk.numMixBuses; i++)
        {
            kwlMixBusChunk* mi = &binaryRep.mixBusChunk.mixBuses[i];
            mi->id = kwlInputStream_readASCIIString(&is);
            mi->numSubBuses = kwlInputStream_readIntBE(&is);
            mi->subBusIndices = NULL;
            
            if (mi->numSubBuses > 0)
            {
                mi->subBusIndices = KWL_MALLOCANDZERO(mi->numSubBuses * sizeof(int), "bin sub bus list");
                for (int j = 0; j < mi->numSubBuses; j++)
                {
                    const int subBusIndexj = kwlInputStream_readIntBE(&is);
                    if(subBusIndexj < 0 || subBusIndexj >= binaryRep.mixBusChunk.numMixBuses)
                    {
                        errorLogCallback("Mix bus %s sub bus index at %d is %d. Expected value in [0, %d]\n",
                                         mi->id, j, subBusIndexj, binaryRep.mixBusChunk.numMixBuses - 1);
                        goto onDataError;
                    }
                    mi->subBusIndices[j] = subBusIndexj;
                }
            }
        }
    }
    
    
    //mix presets
    {
        binaryRep.mixPresetChunk.chunkId = KWL_MIX_PRESETS_CHUNK_ID;
        binaryRep.mixPresetChunk.chunkSize = kwlInputStream_seekToEngineDataChunk(&is, KWL_MIX_PRESETS_CHUNK_ID);
        
        //allocate memory for the mix preset data
        binaryRep.mixPresetChunk.numMixPresets = kwlInputStream_readIntBE(&is);
        binaryRep.mixPresetChunk.mixPresets = KWL_MALLOCANDZERO(binaryRep.mixPresetChunk.numMixPresets * sizeof(kwlMixPresetChunk), "bin mix presets");
        
        const int numParameterSets = binaryRep.mixBusChunk.numMixBuses;
        int defaultPresetIndex = -1;
        
        //read data
        for (int i = 0; i < binaryRep.mixPresetChunk.numMixPresets; i++)
        {
            kwlMixPresetChunk* mpi = &binaryRep.mixPresetChunk.mixPresets[i];
            
            mpi->id = kwlInputStream_readASCIIString(&is);
            mpi->isDefault = kwlInputStream_readIntBE(&is);
            
            if (mpi->isDefault != 0)
            {
                if(defaultPresetIndex != -1)
                {
                    errorLogCallback("Multiple default mix presets found");
                    goto onDataError;
                }
                defaultPresetIndex = i;
            }
            
            mpi->gainLeft = (float*)KWL_MALLOCANDZERO(sizeof(float) * numParameterSets, "bin mp gains l");
            mpi->gainRight = (float*)KWL_MALLOCANDZERO(sizeof(float) * numParameterSets, "bin mp gains r");
            mpi->pitch = (float*)KWL_MALLOCANDZERO(sizeof(float) * numParameterSets, "bin mp pitches");
            mpi->mixBusIndices = (int*)KWL_MALLOCANDZERO(sizeof(int) * numParameterSets, "bin mp indices");
            
            for (int j = 0; j < numParameterSets; j++)
            {
                mpi->mixBusIndices[j] = kwlInputStream_readIntBE(&is);
                if (mpi->mixBusIndices[j] < 0 ||  mpi->mixBusIndices[j] >= numParameterSets)
                {
                    errorLogCallback("Mix preset %s references out of bounds index %d (mix bus count is %d)\n",
                                     mpi->id, mpi->mixBusIndices[j], binaryRep.mixBusChunk.numMixBuses);
                    goto onDataError;
                }
                mpi->gainLeft[j] = kwlInputStream_readFloatBE(&is);
                mpi->gainRight[j] = kwlInputStream_readFloatBE(&is);
                mpi->pitch[j] = kwlInputStream_readFloatBE(&is);
            }
        }
        
        if (defaultPresetIndex < 0)
        {
            errorLogCallback("No default mix preset found");
            goto onDataError;
        }
    }
    
    //wave bank data
    {
        binaryRep.waveBankChunk.chunkId = KWL_WAVE_BANKS_CHUNK_ID;
        binaryRep.waveBankChunk.chunkSize = kwlInputStream_seekToEngineDataChunk(&is, KWL_WAVE_BANKS_CHUNK_ID);
        
        /*deserialize wave bank structures*/
        binaryRep.waveBankChunk.numAudioDataItemsTotal = kwlInputStream_readIntBE(&is);
        if (binaryRep.waveBankChunk.numAudioDataItemsTotal < 0)
        {
            errorLogCallback("Expected at least one audio data entry, found %d", binaryRep.waveBankChunk.numAudioDataItemsTotal);
            goto onDataError;
        }
        
        binaryRep.waveBankChunk.numWaveBanks = kwlInputStream_readIntBE(&is);
        if (binaryRep.waveBankChunk.numWaveBanks < 0)
        {
            errorLogCallback("Expected at least one wave bank, found %d", binaryRep.waveBankChunk.numWaveBanks);
            goto onDataError;
        }
        
        binaryRep.waveBankChunk.waveBanks = KWL_MALLOCANDZERO(binaryRep.waveBankChunk.numWaveBanks * sizeof(kwlWaveBankChunk),
                                                              "bin wbs");
        
        int audioDataItemIdx = 0;
        for (int i = 0; i < binaryRep.waveBankChunk.numWaveBanks; i++)
        {
            kwlWaveBankChunk* wbi = &binaryRep.waveBankChunk.waveBanks[i];
            wbi->id = kwlInputStream_readASCIIString(&is);
            wbi->numAudioDataEntries = kwlInputStream_readIntBE(&is);
            if (wbi->numAudioDataEntries < 0)
            {
                errorLogCallback("Wave bank %s has %d audio data items. Expected at least 1.", wbi->id, wbi->numAudioDataEntries);
                goto onDataError;
            }
            
            wbi->audioDataEntries = KWL_MALLOCANDZERO(wbi->numAudioDataEntries * sizeof(char*), "bin wb audio data list");
            
            for (int j = 0; j < wbi->numAudioDataEntries; j++)
            {
                wbi->audioDataEntries[j] = kwlInputStream_readASCIIString(&is);
                audioDataItemIdx++;
            }
        }
    }
    
    //sound data
    {
        binaryRep.soundChunk.chunkId = KWL_SOUNDS_CHUNK_ID;
        binaryRep.soundChunk.chunkSize = kwlInputStream_seekToEngineDataChunk(&is, KWL_SOUNDS_CHUNK_ID);
        
        /*allocate memory for sound definitions*/
        binaryRep.soundChunk.numSoundDefinitions = kwlInputStream_readIntBE(&is);
        binaryRep.soundChunk.soundDefinitions = KWL_MALLOCANDZERO(binaryRep.soundChunk.numSoundDefinitions * sizeof(kwlSoundChunk),
                                                                  "bin sound defs");
        
        /*read sound definitions*/
        for (int i = 0; i < binaryRep.soundChunk.numSoundDefinitions; i++)
        {
            kwlSoundChunk* si = & binaryRep.soundChunk.soundDefinitions[i];
            si->playbackCount = kwlInputStream_readIntBE(&is);
            si->deferStop = kwlInputStream_readIntBE(&is);
            si->gain = kwlInputStream_readFloatBE(&is);
            si->gainVariation = kwlInputStream_readFloatBE(&is);
            si->pitch = kwlInputStream_readFloatBE(&is);
            si->pitchVariation = kwlInputStream_readFloatBE(&is);
            si->playbackMode = (kwlSoundPlaybackMode)kwlInputStream_readIntBE(&is);
            
            si->numWaveReferences = kwlInputStream_readIntBE(&is);
            if (si->numWaveReferences < 0)
            {
                errorLogCallback("Sound with index %d has %d audio data items. Expected at least 1.", i, si->numWaveReferences);
                goto onDataError;
            }
            
            si->waveBankIndices = KWL_MALLOCANDZERO(si->numWaveReferences * sizeof(int), "bin wb idcs");
            si->audioDataIndices = KWL_MALLOCANDZERO(si->numWaveReferences * sizeof(int), "bin ad idcs");
            
            for (int j = 0; j < si->numWaveReferences; j++)
            {
                si->waveBankIndices[j] = kwlInputStream_readIntBE(&is);
                si->audioDataIndices[j] = kwlInputStream_readIntBE(&is);
            }
        }
    }

    //event data
    {
        binaryRep.eventChunk.chunkId = KWL_EVENTS_CHUNK_ID;
        binaryRep.eventChunk.chunkSize = kwlInputStream_seekToEngineDataChunk(&is, KWL_EVENTS_CHUNK_ID);
        
        /*read the total number of event definitions*/
        binaryRep.eventChunk.numEventDefinitions = kwlInputStream_readIntBE(&is);
        if (binaryRep.eventChunk.numEventDefinitions <= 0)
        {
            errorLogCallback("Found %d event definitions. Expected at least 1.", binaryRep.eventChunk.numEventDefinitions);
            goto onDataError;
        }
        
        binaryRep.eventChunk.eventDefinitions = KWL_MALLOCANDZERO(binaryRep.eventChunk.numEventDefinitions * sizeof(kwlEventChunk),
                                                                  "bin ev defs");

        for (int i = 0; i < binaryRep.eventChunk.numEventDefinitions; i++)
        {
            kwlEventChunk* ei = &binaryRep.eventChunk.eventDefinitions[i];
            
            /*read the id of this event definition*/
            ei->id = kwlInputStream_readASCIIString(&is);

            ei->instanceCount = kwlInputStream_readIntBE(&is);
            KWL_ASSERT(ei->instanceCount >= -1);

            ei->gain = kwlInputStream_readFloatBE(&is);
            ei->pitch = kwlInputStream_readFloatBE(&is);
            ei->innerConeAngleDeg = kwlInputStream_readFloatBE(&is);
            ei->outerConeAngleDeg = kwlInputStream_readFloatBE(&is);
            ei->outerConeGain = kwlInputStream_readFloatBE(&is);

            /*read the index of the mix bus that this event belongs to*/
            ei->mixBusIndex = kwlInputStream_readIntBE(&is);
            KWL_ASSERT(ei->mixBusIndex >= 0 && ei->mixBusIndex < binaryRep.mixBusChunk.numMixBuses);
            ei->isPositional = kwlInputStream_readIntBE(&is);

            /*read the index of the sound referenced by this event (ignored for streaming events)*/
            ei->soundIndex = kwlInputStream_readIntBE(&is);
            
            if (ei->soundIndex < -1)
            {
                errorLogCallback("Invalid sound index %d in event definition %s.", ei->soundIndex, ei->id);
                goto onDataError;
            }
            
            /*read the event retrigger mode (ignored for streaming events)*/
            ei->retriggerMode = (kwlEventRetriggerMode)kwlInputStream_readIntBE(&is);

            /*read the index of the audio data referenced by this event (only used for streaming events)*/
            ei->waveBankIndex = kwlInputStream_readIntBE(&is);
            ei->audioDataIndex = kwlInputStream_readIntBE(&is);
            
            /*read loop flag (ignored for non-streaming events)*/
            ei->loopIfStreaming = kwlInputStream_readIntBE(&is);

            /*read referenced wave banks*/
            ei->numReferencedWaveBanks = kwlInputStream_readIntBE(&is);
            ei->waveBankIndices = (int*)KWL_MALLOC(ei->numReferencedWaveBanks * sizeof(int),
                                                        "bin evt wave bank refs");

            for (int j = 0; j < ei->numReferencedWaveBanks; j++)
            {
                ei->waveBankIndices[j] = kwlInputStream_readIntBE(&is);
                if (ei->waveBankIndices[j] < 0 || ei->waveBankIndices[j] >= binaryRep.waveBankChunk.numWaveBanks)
                {
                    errorLogCallback("Invalid wave bank index index %d in event definition %s. Wave bank count is %d",
                                     ei->waveBankIndices[j],
                                     ei->id,
                                     binaryRep.waveBankChunk.numWaveBanks);
                    goto onDataError;
                }
            }
        }
    }
    
    kwlInputStream_close(&is);
    kwlProjectDataBinary_dump(&binaryRep, errorLogCallback);
    return;
    
    onDataError:
    kwlInputStream_close(&is);
    kwlProjectDataBinary_free(&binaryRep);
    return;
}

void kwlProjectDataBinary_dump(kwlProjectDataBinary* bin, kwlLogCallback logCallback)
{
    logCallback("Mix bus chunk (ID %d, %d bytes, %d mix buses):\n",
                bin->mixBusChunk.chunkId,
                bin->mixBusChunk.chunkSize,
                bin->mixBusChunk.numMixBuses);
    for (int i = 0; i < bin->mixBusChunk.numMixBuses; i++)
    {
        kwlMixBusChunk* mbi = &bin->mixBusChunk.mixBuses[i];
        logCallback("    %s (%d sub buses)\n", mbi->id, mbi->numSubBuses);
        for (int j = 0; j < mbi->numSubBuses; j++)
        {
            logCallback("        %d\n", mbi->subBusIndices[j]);
        }
    }
    
    logCallback("\n");
    logCallback("Mix preset chunk (ID %d, %d bytes, %d mix presets):\n",
                bin->mixPresetChunk.chunkId,
                bin->mixPresetChunk.chunkSize,
                bin->mixPresetChunk.numMixPresets);
    for (int i = 0; i < bin->mixPresetChunk.numMixPresets; i++)
    {
        kwlMixPresetChunk* mpi = &bin->mixPresetChunk.mixPresets[i];
        logCallback("    %s\n", mpi->id);
    }
    
    logCallback("\n");
    logCallback("Wave bank chunk (ID %d, %d bytes, %d wave banks):\n",
                bin->waveBankChunk.chunkId,
                bin->waveBankChunk.chunkSize,
                bin->waveBankChunk.numWaveBanks);
    for (int i = 0; i < bin->waveBankChunk.numWaveBanks; i++)
    {
        
    }
    
    logCallback("\n");
    logCallback("Sound chunk (ID %d, %d bytes, %d sound definitions):\n",
                bin->soundChunk.chunkId,
                bin->soundChunk.chunkSize,
                bin->soundChunk.numSoundDefinitions);
    for (int i = 0; i < bin->soundChunk.numSoundDefinitions; i++)
    {
        
    }
    
    logCallback("\n");
    logCallback("Event chunk (ID %d, %d bytes, %d event definitions):\n",
                bin->eventChunk.chunkId,
                bin->eventChunk.chunkSize,
                bin->eventChunk.numEventDefinitions);
    for (int i = 0; i < bin->eventChunk.numEventDefinitions; i++)
    {
        
    }
}

void kwlProjectDataBinary_free(kwlProjectDataBinary* bin)
{
    //TODO
}
