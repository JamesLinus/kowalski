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
#ifndef KWL__AUDIO_DATA_H
#define KWL__AUDIO_DATA_H

/*! \file */

#include "kwl_inputstream.h"
#include "kwl_wavebank.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
    /**
     * An enumeration of audio encoding types.
     * NOTE: These values must match the values output by
     * the wave bank builder.
     */
    typedef enum kwlAudioEncoding
    {
        /** An unknown encoding.*/
        KWL_ENCODING_UNKNOWN = 4,
        /** Linear PCM. 16 bit, signed, interleaved.*/
        KWL_ENCODING_SIGNED_16BIT_PCM = 0,
        /** Ogg Vorbis. */
        KWL_ENCODING_VORBIS = 1,
        /** IMA ADPCM (aka DVI ADPCM or IMA4). */
        KWL_ENCODING_IMA_ADPCM = 2,
        /** Advanced audio coding, supported by the iPhone/iPad hardware decoder.*/
        KWL_ENCODING_AAC = 3,
        /** Linear PCM. 24 bit, signed, interleaved.*/
        KWL_ENCODING_SIGNED_24BIT_PCM = 5,
        /** Linear PCM. 32 bit, signed, interleaved.*/
        KWL_ENCODING_SIGNED_32BIT_PCM = 6,
        /** Linear PCM. 8 bit, signed, interleaved.*/
        KWL_ENCODING_SIGNED_8BIT_PCM = 7,
        /** Linear PCM. 8 bit, unsigned, interleaved.*/
        KWL_ENCODING_UNSIGNED_8BIT_PCM = 8,
        
    } kwlAudioEncoding;
    
    
    /**
     * A structure describing a piece of audio data. In the case
     * of PCM data, the data is just an array of signed, interleaved 16 bit samples.
     * In the case of non-PCM data, the data may represent an entire audio file
     * that a suitable decoder is responsible for parsing and decoding.
     */
    typedef struct kwlAudioData
    {
        /** The audio data encoding.*/
        kwlAudioEncoding encoding;
        /**
         * The file path relative to the Kowalski project data.
         * Note: this has nothing to do with the runtime path, but
         * is used to associate wave bank entries with audio data
         * entries in the engine and is also convenient for debugging.
         */
        const char* filePath;
        /**
         * The wave bank containing this piece of audio data.
         * NULL indicates that the data does not belong to a wavebank.
         */
        kwlWaveBank* waveBank;
        /** The number of audio frames in the data. Only used for PCM data. */
        int numFrames;
        /** The number of channels of the audio. Only used for PCM data. */
        int numChannels;
        /** The total number of bytes of loaded audio data. A value of 0 indicates that no data is loaded. */
        int numBytes;
        /** */
        void* bytes;
        int isEntireFile;
        /** Non-zero if the non-PCM data (if any) should be streamed from disk.*/
        int streamFromDisk;
        /** If this is streaming data, this indicates the byte offset into the stream of the first audio data byte.*/
        int fileOffset;
        /** Non-zero if the audio data is loaded, zero otherwise.*/
        int isLoaded;
        /** */
        int isBigEndian;
    } kwlAudioData;
    
    /** Releasesa any resources associated with a given audio data instance.*/
    void kwlAudioData_free(kwlAudioData* audioData);

    /**
     *
     */
    int kwlAudioData_isLinearPCM(kwlAudioData* audioData);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*KWL__AUDIO_DATA_H*/
