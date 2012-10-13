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

#ifndef KWL__WAVE_BANK_H
#define KWL__WAVE_BANK_H

/*! \file */ 

#include "kowalski.h"
#include "kowalski.h"
#include "kwl_inputstream.h"
#include "kwl_messagequeue.h"
#include "kwl_synchronization.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

struct kwlEngine;

/** The number of bytes in the wave bank file identifier. */
#define KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH 9
    
/** 
 * The file identifier for wave bank binaries, ie the sequence of bytes
 * that all wave bank binary files start with.
 */
static const char KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER[KWL_WAVE_BANK_BINARY_FILE_IDENTIFIER_LENGTH] =
{
    0xAB, 'K', 'W', 'B', 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
};
    
/** 
 * A struct containing everything needed to load
 * a wave bank on a separate thread.
 */
typedef struct kwlWaveBankLoadingThread
{
    /** The thread to use for asyncronous loading. */
    kwlThread thread;
    /** The wave bank to load */
    struct kwlWaveBank* waveBank;
    /** */
    kwlMessageQueue* messageQueue;
    /** */
    kwlMutexLock* messageQueueLock;
    /** The input stream to load from.*/
    kwlInputStream inputStream;
} kwlWaveBankLoadingThread;
    
/** 
 * A named collection of pieces of audio data.
 */
typedef struct kwlWaveBank
{
    /** The ID of the wave bank. */
    const char* id;
    /** Non-zero if the wave bank is loaded, zero otherwise*/
    int isLoaded;
    /** The path to the wave bank file. Empty if the wave bank is not loaded.*/
    char *waveBankFilePath;
    /** An array of audio data entries for the wave bank. */
    struct kwlAudioData* audioDataItems;
    /** The number of audio data entries in the wave bank. */
    int numAudioDataEntries;
    /** Used for threaded loading (if requested). */
    kwlWaveBankLoadingThread loadingThread;
} kwlWaveBank;

/** 
 * Checks that a wave bank binary at a given path is valid with respect to 
 * currently loaded engine data.
 */    
kwlError kwlWaveBank_verifyWaveBankBinary(struct kwlEngine* engine, 
                                          const char* const waveBankPath,
                                          kwlWaveBank** waveBank);
    
/**
 * Loads all audio data items from a given input stream. The stream is assumed
 * to be a valid wave bank data stream.
 */
kwlError kwlWaveBank_loadAudioDataItems(kwlWaveBank* waveBank, kwlInputStream* inputStream);

/**
 * Load wave bank audio data from a file at a given path. If callback is not NULL, this method returns immediately and 
 * loading is performed in a separate thread and the callback gets invoked when loading finishes.
 * If callback is NULL, this function returns when all data has been loaded.
 */
kwlError kwlWaveBank_loadAudioData(kwlWaveBank* waveBank, 
                                   const char* path, 
                                   int threaded);
    
/** The entry point for the loading thread.*/
void* kwlWaveBank_loadingThreadEntryPoint(void* loadingThread);
    
/** */
void kwlWaveBank_unload(kwlWaveBank* waveBank);

#ifdef __cplusplus
}
#endif /* __cplusplus */    
    
#endif /*KWL__WAVE_BANK_H*/
