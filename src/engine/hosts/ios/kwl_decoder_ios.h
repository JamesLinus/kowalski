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

#ifndef KWL__DECODER_IPHONE_H
#define KWL__DECODER_IPHONE_H

/*! \file */ 

#ifdef KWL_IPHONE

#include "kwl_decoder.h"
#include <AudioToolbox/AudioToolbox.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** 
 * 
 */
typedef struct
{
    /** */
    AudioStreamPacketDescription outPacketDesc;
    /** */
    AudioFileID audioFileID;
    /** */
    AudioConverterRef converter;
    /** */
    AudioBufferList bufferList;
    /** */
    int currentPacketIndex;
    /** Non-zero if read successfully from priming data, zero otherwise. */
    int numValidFrames;
    /** Non-zero if read successfully from priming data, zero otherwise. Only used for sanity check.*/
    int numTrailingFrames;
    /** The number of decoded frames (gets reset when looping) */
    int numDecodedFrames;
    /** An upper bound on the size in bytes of packets of compressed audio data.*/
    int packetSizeUpperBound;
    /** */
    int hasMoreData;
    /** A buffer to read packets of compressed audio data into.*/
    void* packetOutBuffer;
} kwlIPhoneDecoderData;

/** */
kwlError kwlInitDecoderIPhone(kwlDecoder* decoder);

/** */
void kwlDeinitDecoderIPhone(kwlDecoder* decoder);

/** 
 * Callback for reading audio files from arbitrary sources. 
 * @param inClientData A pointer to the client data as set in the inClientData parameter 
 *        to AudioFileOpenWithCallbacks or AudioFileInitializeWithCallbacks.
 * @param inPosition An offset into the data from which to read.
 * @param requestCount The number of bytes to read.
 * @param buffer A pointer to the buffer in which to put the data read.
 * @param actualCount On output, the callback should set this parameter to a pointer 
 *        to the number of bytes successfully read.
 * @return A result code.
 */
OSStatus audioFileReadCallback(void *inClientData,
                               SInt64 inPosition,
                               UInt32 requestCount,
                               void *buffer,
                               UInt32 *actualCount);

/** Returns the size of an audio file.*/
SInt64 audioFileGetSizeCallback(void *inClientData);

/** 
 * Provides the converter with data. 
 * @param inAudioConverter The audio converter object that invoked this callback to obtain new data to convert.
 * @param ioNumberDataPackets On input, the minimum number of packets of input audio data the converter needs for 
 *        its current conversion cycle. On output, the number of packets of audio data provided for conversion, 
 *        or 0 if there is no more data to convert.
 * @param ioData On output, point the fields of the AudioBufferList structure, passed by this parameter, 
 *        to the audio data you are providing to be converted.
 * @param outDataPacketDescription If not NULL on input, the audio converter expects this callback to provide an array of 
 *        AudioStreamPacketDescription structures on output, one for each packet of audio data you
 *        are providing in the ioData parameter.
 * @param inUserData On input, the custom application data you provided to the AudioConverterFillComplexBuffer function.
 * @return A result code.
 */
OSStatus complexInputDataCallback(AudioConverterRef inAudioConverter,
                                  UInt32 *ioNumberDataPackets,
                                  AudioBufferList *ioData,
                                  AudioStreamPacketDescription **outDataPacketDescription,
                                  void *inUserData);

/**
 * Fills the temporary buffer of a given decoder with decoded samples.
 * @param decoder The decoder.
 * @return Non-zero if the end of the stream was reached, zero otherwise.
 */
int kwlDecodeBufferIPhone(kwlDecoder* decoder);
    
int kwlRewindDecoderIPhone(kwlDecoder* decoder);
    
#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /*KWL_IPHONE*/

#endif /*KWL__DECODER_IPHONE_H*/
