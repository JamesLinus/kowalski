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

#ifndef KWL_AUDIO_FILE_UTIL_H
#define KWL_AUDIO_FILE_UTIL_H

/*! \file */ 

#include "kwl_audiodata.h"
#include "kowalski.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
/** 
 * Available ways of loading the contents of an audio file into a /c kwlAudioData struct.
 */
typedef enum  {
    /**
     * Load the entire audio file (and not just the audio data) into the 
     * /c kwlAudioData struct and populate meta data fields if possible.
     */
    KWL_LOAD_ENTIRE_FILE = 0,
    /** Don't load audio data bytes into the /c kwlAudioData struct. */
    KWL_SKIP_AUDIO_DATA,
    /** Convert the audio file data to signed 16 bit PCM or fail.*/
    KWL_CONVERT_TO_INT16_OR_FAIL
} kwlAudioDataLoadingMode;
    
/** 
 * Loads PCM data from an AIFF file.
 * @param path The path of the file to load.
 * @param audioData A kwlAudioData struct to load the file into.
 */
kwlError kwlLoadAIFF(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);
    
/**
 * Loads AIFF PCM data from input stream and advances the input stream read position
 * accordingly.
 * @param stream The stream to load from.
 * @param audioData A kwlAudioData struct to load the file into. 
 * @param metadataOnly If non-zero, the \c audioData struct will only be populated with metadata
 * (and no audio data) and the \fileOffset field of the \c audioData struct will
 * be set to the byte index of the first sample.
 */
kwlError kwlLoadAIFFFromStream(kwlInputStream* stream, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);
    
/** 
 * Loads PCM data from an WAV file.
 * @param path The path of the file to load.
 * @param audioData A kwlAudioData struct to load the file into.
 */
kwlError kwlLoadWAV(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);
    
/**
 * Loads WAV PCM data from input stream and advances the input stream read position
 * accordingly.
 * @param stream The stream to load from.
 * @param audioData A kwlAudioData struct to load the file into. 
 * @param metadataOnly If non-zero, the \c audioData struct will only be populated with metadata
 * (and no audio data) and the \fileOffset field of the \c audioData struct will
 * be set to the byte index of the first sample.
 */
kwlError kwlLoadWAVFromStream(kwlInputStream* stream, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);
    
/** 
 * Loads PCM data from an AU file.
 * @param path The path of the file to load.
 * @param audioData A kwlAudioData struct to load the file into.
 */
kwlError kwlLoadAU(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);

/**
 * Loads WAV PCM data from input stream and advances the input stream read position
 * accordingly.
 * @param stream The stream to load from.
 * @param audioData A kwlAudioData struct to load the file into. 
 * @param metadataOnly If non-zero, the \c audioData struct will only be populated with metadata
 * (and no audio data) and the \fileOffset field of the \c audioData struct will
 * be set to the byte index of the first sample.
 */
kwlError kwlLoadAUFromStream(kwlInputStream* stream, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);
    
/** 
 * Loads vorbis data from an OGG file.
 * @param path The path of the file to load.
 * @param audioData A kwlAudioData struct to load the file into.
 */
kwlError kwlLoadOggVorbis(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);
    
/** 
 * Loads audio data from a given audio file.
 * @param path The path of the file to load.
 * @param audioData A kwlAudioData struct to load the file into.
 */
kwlError kwlLoadAudioFile(const char* path, kwlAudioData* audioData, kwlAudioDataLoadingMode mode);
    
short* kwlConvertBufferTo16BitSigned(char* inBuffer, 
                                     int inBufferSizeInBytes,
                                     int* outBufferSizeInBytes, 
                                     kwlAudioEncoding inBufferEncoding, 
                                     int isBigEndian);

kwlError kwlLoadIMAADPCMWAVMetadataFromStream(kwlInputStream* stream, 
                                              kwlAudioData* audioData, 
                                              int* firstDataBlockByte,
                                              int* dataBlockSize,
                                              int* nBlockAlign);
    
    
#ifdef __cplusplus
}
#endif /* __cplusplus */   
    
#endif /*KWL_AUDIO_FILE_UTIL_H*/
