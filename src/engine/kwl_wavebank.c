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

#include <string.h>

#include "kwl_audiodata.h"
#include "kwl_memory.h"
#include "kwl_assert.h"
#include "kwl_engine.h"
#include "kwl_wavebank.h"

kwlError kwlWaveBank_verifyWaveBankBinary(kwlEngine* engine, 
                                          const char* const waveBankPath,
                                          kwlWaveBank** waveBank)
{
    /*Open the file...*/
    kwlInputStream stream;
    kwlError result = kwlInputStream_initWithFile(&stream, waveBankPath);
    if (result != KWL_NO_ERROR)
    {
        kwlInputStream_close(&stream);
        return result;
    }
    /*... and check the wave bank file identifier.*/
    int i;
    for (i = 0; i < KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH; i++)
    {
        const char identifierChari = kwlInputStream_readChar(&stream);
        if (identifierChari != KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER[i])
        {
            /* Not the file identifier we expected. */
            kwlInputStream_close(&stream);
            return KWL_UNKNOWN_FILE_FORMAT;
        }
    }
    
    /*Read the ID from the wave bank binary file and find a matching wave bank struct.*/
    const char* waveBankToLoadId = kwlInputStream_readASCIIString(&stream);
    const int waveBankToLoadnumAudioDataEntries = kwlInputStream_readIntBE(&stream);
    const int numWaveBanks = engine->engineData.numWaveBanks;
    kwlWaveBank* matchingWaveBank = NULL;
    for (i = 0; i < numWaveBanks; i++)
    {
        if (strcmp(waveBankToLoadId, engine->engineData.waveBanks[i].id) == 0)
        {
            matchingWaveBank = &engine->engineData.waveBanks[i];
        }
    }
    
    KWL_FREE((void*)waveBankToLoadId);
    if (matchingWaveBank == NULL)
    {
        /*No matching bank was found. Close the file stream and return an error.*/
        kwlInputStream_close(&stream);
        return KWL_NO_MATCHING_WAVE_BANK;
    }
    else if (waveBankToLoadnumAudioDataEntries != matchingWaveBank->numAudioDataEntries)
    {
        /*A matching wave bank was found but the number of audio data entries
         does not match the binary wave bank data.*/
        kwlInputStream_close(&stream);
        return KWL_WAVE_BANK_ENTRY_MISMATCH;
    }
    else if (matchingWaveBank->isLoaded != 0)
    {
        /*The wave bank is already loaded, just set the handle and do nothing.*/
        *waveBank = matchingWaveBank;
        kwlInputStream_close(&stream);
        return KWL_NO_ERROR;
    }
    
    /*Store the path the wave bank was loaded from (used when streaming from disk).*/
    const int pathLen = strlen(waveBankPath);
    matchingWaveBank->waveBankFilePath = (char*)KWL_MALLOC((pathLen + 1) * sizeof(char), "wave bank path string");
    strcpy(matchingWaveBank->waveBankFilePath, waveBankPath);
    
    /*Make sure that the entries of the wave bank to load and the wave bank struct line up.*/
    for (i = 0; i < waveBankToLoadnumAudioDataEntries; i++)
    {
        const char* filePathi = kwlInputStream_readASCIIString(&stream);
        
        int matchingEntryIndex = -1;
        int j = 0;
        for (j = 0; j < waveBankToLoadnumAudioDataEntries; j++)
        {
            if (strcmp(matchingWaveBank->audioDataItems[j].filePath, filePathi) == 0)
            {
                matchingEntryIndex = j;
                break;
            }
        }
        
        KWL_FREE((void*)filePathi);
        
        if (matchingEntryIndex < 0)
        {
            /* This wave bank entry has no corresponding waveform slot. Abort loading.*/
            kwlInputStream_close(&stream);
            return KWL_WAVE_BANK_ENTRY_MISMATCH;
        }
        
        /*skip to the next wave data entry*/
        /*const int encoding = */kwlInputStream_readIntBE(&stream);
        /*const int streamFromDisk = */kwlInputStream_readIntBE(&stream);
        const int numChannels = kwlInputStream_readIntBE(&stream);
        KWL_ASSERT((numChannels == 0 || numChannels == 1 || numChannels == 2) && "invalid num channels");
        const int numBytes = kwlInputStream_readIntBE(&stream);
        KWL_ASSERT(numBytes > 0);
        kwlInputStream_skip(&stream, numBytes);
    }
    
    /* Reading went well. */
    kwlInputStream_close(&stream);
    *waveBank = matchingWaveBank;
    return KWL_NO_ERROR;
}

kwlError kwlWaveBank_loadAudioData(kwlWaveBank* waveBank, 
                                   const char* path, 
                                   int threaded,
                                   kwlWaveBankFinishedLoadingCallback callback)
{
    if (threaded == 0)
    {
        /*perform blocking loading*/
        kwlInputStream stream;
        kwlInputStream_initWithFile(&stream, path);
        return kwlWaveBank_loadAudioDataItems(waveBank, &stream);
    }
    else
    {
        /*do asynchronous loading*/
        kwlInputStream_initWithFile(&waveBank->loadingThread.inputStream, path);
        waveBank->loadingThread.waveBank = waveBank;
        waveBank->loadingThread.callback = callback;
        
        kwlThreadCreate(&waveBank->loadingThread.thread, 
                        kwlWaveBank_loadingThreadEntryPoint, 
                        waveBank);
        return KWL_NO_ERROR;
    }
}

kwlError kwlWaveBank_loadAudioDataItems(kwlWaveBank* waveBank, kwlInputStream* stream)
{
    /*The input stream is assumed to be valid, so move the
      read position to the first audio data entry.*/
    kwlInputStream_reset(stream);
    kwlInputStream_skip(stream, KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH);
    const int strLen = kwlInputStream_readIntBE(stream);
    kwlInputStream_skip(stream, strLen); 
    /*int numEntries = */kwlInputStream_readIntBE(stream);
    
    const int waveBankToLoadnumAudioDataEntries = waveBank->numAudioDataEntries;
    
    for (int i = 0; i < waveBankToLoadnumAudioDataEntries; i++)
    {
        char* const waveEntryIdi = kwlInputStream_readASCIIString(stream);
        
        kwlAudioData* matchingAudioData = NULL;
        int j;
        for (int j = 0; j < waveBankToLoadnumAudioDataEntries; j++)
        {
            kwlAudioData* entryj = &waveBank->audioDataItems[j];
            if (strcmp(entryj->filePath, waveEntryIdi) == 0)
            {
                matchingAudioData = entryj;
                break;
            }
        }
        //printf("    loading %s\n", waveEntryIdi);
        KWL_FREE(waveEntryIdi);
        
        const kwlAudioEncoding encoding = (kwlAudioEncoding)kwlInputStream_readIntBE(stream);
        const int streamFromDisk = kwlInputStream_readIntBE(stream);
        const int numChannels = kwlInputStream_readIntBE(stream);
        const int numBytes = kwlInputStream_readIntBE(stream);
        const int numFrames = numBytes / 2 * numChannels;
        
        /* Check that the audio meta data makes sense */
        if (numBytes <= 0)
        {
            KWL_ASSERT(0 && "the number of audio data bytes must be positive");
            return KWL_CORRUPT_BINARY_DATA;
        }
        if (matchingAudioData == NULL)
        {
            KWL_ASSERT(0 && "no matching wave bank entry");
            return KWL_CORRUPT_BINARY_DATA;
        }
        if (numChannels != 0 && numChannels != 1 && numChannels != 2)
        {
            KWL_ASSERT(0 && "invalid number of channels");
            return KWL_CORRUPT_BINARY_DATA;
        }
        
        /*free any old data*/
        kwlAudioData_free(matchingAudioData);
        
        /*Store audio meta data.*/
        matchingAudioData->numFrames = numFrames;
        matchingAudioData->numChannels = numChannels;
        matchingAudioData->numBytes = numBytes;
        matchingAudioData->encoding = (kwlAudioEncoding)encoding;
        matchingAudioData->streamFromDisk = streamFromDisk;
        matchingAudioData->isLoaded = 1;
        matchingAudioData->bytes = NULL;
        
        if (streamFromDisk == 0)
        {
            /*This entry should not be streamed, so allocate audio data up front.*/
            matchingAudioData->bytes = KWL_MALLOC(numBytes, "kwlEngine_loadWaveBank");
            
            int bytesRead = kwlInputStream_read(stream, 
                                                (signed char*)matchingAudioData->bytes, 
                                                numBytes);
            if (bytesRead != numBytes)
            {
                KWL_ASSERT(0 && "error reading wave bank audio data bytes");
                return KWL_CORRUPT_BINARY_DATA;
            }
        }
        else
        {
            /*Store the offset into the wave bank binary files for streaming entries.*/
            matchingAudioData->fileOffset = kwlInputStream_tell(stream);
            kwlInputStream_skip(stream, numBytes);
        }
    }
    
    waveBank->isLoaded = 1;
    return KWL_NO_ERROR;
}

void* kwlWaveBank_loadingThreadEntryPoint(void* userData)
{
    printf("starting threaded wb load\n"); 
    kwlWaveBank* waveBank = (kwlWaveBank*)userData;
    kwlError result = kwlWaveBank_loadAudioDataItems(waveBank->loadingThread.waveBank, 
                                                     &waveBank->loadingThread.inputStream);
    
    //TODO: do something with the result.
    
    /*invoke the callback*/
    if (waveBank->loadingThread.callback != NULL)
    {
        //TODO: pass proper handle
        waveBank->loadingThread.callback(KWL_INVALID_HANDLE, waveBank->loadingThread.callbackUserData);
    }
    
    printf("threaded wb done.\n"); 
    return NULL;
}

void kwlWaveBank_unload(kwlWaveBank* waveBank)
{
    if (waveBank->isLoaded == 0)
    {
        return;
    }
    
    /* Free all allocated audio data in the wave bank*/
    const int numAudioDataEntriesInBank = waveBank->numAudioDataEntries;
    int i;
    for (i = 0; i < numAudioDataEntriesInBank; i++)
    {
        kwlAudioData* wavei = &waveBank->audioDataItems[i];
        kwlAudioData_free(wavei);
    }
    waveBank->isLoaded = 0;
    KWL_FREE(waveBank->waveBankFilePath);
}