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

#ifdef KWL_IPHONE

#include "kwl_decoder_iphone.h"
#include "kwl_inputstream.h"
#include "kwl_memory.h"

kwlError kwlInitDecoderIPhone(kwlDecoder* decoder)
{
    /* Allocate decoder data.*/
    kwlIPhoneDecoderData* data = (kwlIPhoneDecoderData*)KWL_MALLOC(sizeof(kwlIPhoneDecoderData), "iphone decoder data");
    kwlMemset(data, 0, sizeof(kwlIPhoneDecoderData));
    
    /* Hook up data and callbacks to the decoder.*/
    decoder->codecData = data;
    decoder->decodeBuffer = &kwlDecodeBufferIPhone;
    decoder->deinit = &kwlDeinitDecoderIPhone;
    decoder->rewind = &kwlRewindDecoderIPhone;
    
    /* 
     * Open the file for reading, providing data from the decoder input stream.
     */
    AudioFileID audioFileID = NULL;
    kwlMemset(&audioFileID, 0, sizeof(AudioFileID));
    OSStatus result = AudioFileOpenWithCallbacks(&decoder->audioDataStream,  
                                                 &audioFileReadCallback,
                                                 NULL, /*no inWriteFunc needed for reading.*/
                                                 &audioFileGetSizeCallback,
                                                 NULL, /*no inSetSizeFunc needed for reading.*/
                                                 0,
                                                 &audioFileID);
    if (result == 1954115647 || /*'typ?'*/
        result != noErr)
    {
        return KWL_UNKNOWN_FILE_FORMAT;
    }
    
    data->audioFileID = audioFileID;
    
    /*
     * Get the audio stream description for the audio data to decode.
     */
    AudioStreamBasicDescription sourceFormat;
    kwlMemset(&sourceFormat, 0, sizeof(AudioStreamBasicDescription));
    
    int asbdSize = sizeof(AudioStreamBasicDescription);
    result = AudioFileGetProperty(audioFileID, 
                                  kAudioFilePropertyDataFormat, 
                                  &asbdSize, 
                                  &sourceFormat);
    if (result != noErr)
    {
        return KWL_UNSUPPORTED_ENCODING;
    }
    
    /*
     * Create an audio stream description for the destination format
     * based on the format of the input.
     */
    AudioStreamBasicDescription destinationFormat;
    kwlMemset(&destinationFormat, 0, sizeof(AudioStreamBasicDescription));
    
    destinationFormat.mSampleRate = sourceFormat.mSampleRate;
	destinationFormat.mFormatID = 'lpcm'; /*Linear PCM*/
	destinationFormat.mFormatFlags =  0;/*kAudioFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;*/
	destinationFormat.mFramesPerPacket = 1; /*Always 1 for linear PCM.*/
	destinationFormat.mChannelsPerFrame = sourceFormat.mChannelsPerFrame;
    destinationFormat.mBytesPerFrame = 2 * destinationFormat.mChannelsPerFrame; /*2 for 2 bytes per 16 bit sample.*/
    destinationFormat.mBytesPerPacket = destinationFormat.mBytesPerFrame;
	destinationFormat.mBitsPerChannel = 16;
	destinationFormat.mReserved = 0;
    
    /*
     * Get an upper bound for the packet size.
     * kAudioFilePropertyMaximumPacketSize may give a tighter
     * bound but may also involve scanning the entire file.
     */
    int psubSize = sizeof(kAudioFilePropertyPacketSizeUpperBound);
    result = AudioFileGetProperty(audioFileID,
                                  kAudioFilePropertyPacketSizeUpperBound,
                                  &psubSize,
                                  &data->packetSizeUpperBound);
    KWL_ASSERT(result == 0 && "error getting ios decoder packet size upper bound");
    KWL_ASSERT(data->packetSizeUpperBound > 0);
    
    /*
     * Allocate a buffer to read packets of encoded audio data into.
     */
    data->packetOutBuffer = KWL_MALLOC(data->packetSizeUpperBound, "iphone decoder packet out buffer");
    
    /*
     * Create the audio converter
     */
    result = AudioConverterNew(&sourceFormat,
                               &destinationFormat,
                               &data->converter);
    if (result == 1718449215 || //'fmt?'
        result != noErr)
    {
        return KWL_UNSUPPORTED_ENCODING;
    }
    //CAShow(data->converter); /*Debug print the converter*/
    
    /* 
     * Get the actual input and output formats
     */
    int size = sizeof(sourceFormat);
    result = AudioConverterGetProperty(data->converter, 
                                       kAudioConverterCurrentInputStreamDescription, 
                                       &size, 
                                       &sourceFormat);
    KWL_ASSERT(result == 0 && "error getting ios converter input stream description");
    
    size = sizeof(destinationFormat);
    result = AudioConverterGetProperty(data->converter, 
                                       kAudioConverterCurrentOutputStreamDescription, 
                                       &size, 
                                       &destinationFormat);
    KWL_ASSERT(result == 0 && "error getting ios converter output stream description");
    
    /*
     * Check if the audio data contains any priming information and update the 
     * priming information of the converter if this is the case.
     */
    AudioFilePacketTableInfo srcPti;
    int ptiSize = sizeof(srcPti);
    OSStatus err = AudioFileGetProperty(audioFileID, 
                                        kAudioFilePropertyPacketTableInfo, 
                                        &ptiSize, 
                                        &srcPti); // try to get priming info from bitstream file
    if (err == noErr) 
    { 
        /*Priming info found.*/
        data->numValidFrames = srcPti.mNumberValidFrames;
        data->numTrailingFrames = srcPti.mRemainderFrames;
        AudioConverterPrimeInfo primeInfo;
        primeInfo.leadingFrames = srcPti.mPrimingFrames;
        primeInfo.trailingFrames = 0;
        
        err = AudioConverterSetProperty(data->converter, 
                                        kAudioConverterPrimeInfo, 
                                        sizeof(primeInfo), 
                                        &primeInfo);
        KWL_ASSERT(err == noErr && "error setting AudioConverter priming info");
    } 
    
    
    /*
     * Extract magic cookie data (if any) and pass to the converter
     * http://developer.apple.com/library/mac/#qa/qa1318/_index.html
     */
    UInt32 magicCookieSize = 0;
    err = AudioFileGetPropertyInfo(audioFileID, 
                                   kAudioFilePropertyMagicCookieData,
                                   &magicCookieSize,
                                   NULL); 

    if (err == noErr)
    {
        void *magicCookie = KWL_MALLOC(magicCookieSize, "iOS decoder magic cookie");
        if (magicCookie) 
        {
            err = AudioFileGetProperty(audioFileID, 
                                       kAudioFilePropertyMagicCookieData, 
                                       &magicCookieSize, 
                                       magicCookie); 
            
            if (err == noErr)
            {
                err = AudioConverterSetProperty(data->converter, 
                                                kAudioConverterDecompressionMagicCookie, 
                                                magicCookieSize, 
                                                magicCookie);
            }
            if (magicCookie)
            {
                KWL_FREE(magicCookie);
            }
        }
    }
    
    /*
     * Create an audio buffer list to store decoded PCM samples in
     */
    const int nFrames = 8300;//TODO: magic number
    kwlMemset(&data->bufferList, 0, sizeof(data->bufferList));
    AudioBuffer buffer = {0, 0, NULL};
    buffer.mNumberChannels = 2;
    buffer.mDataByteSize = nFrames * 2 * buffer.mNumberChannels;
    buffer.mData = KWL_MALLOC(buffer.mDataByteSize, "ios decoder buffer");
    data->bufferList.mNumberBuffers = 1;
    data->bufferList.mBuffers[0] = buffer;
    
    decoder->numChannels = destinationFormat.mChannelsPerFrame;
    decoder->maxDecodedBufferSize = 2 * nFrames * decoder->numChannels;
    decoder->currentDecodedBufferSizeInBytes = 0;
        
    return KWL_NO_ERROR;
}

void kwlDeinitDecoderIPhone(kwlDecoder* decoder)
{
    kwlIPhoneDecoderData* data = (kwlIPhoneDecoderData*)decoder->codecData;
    
    if (data->bufferList.mBuffers[0].mData != NULL)
    {
        KWL_FREE(data->bufferList.mBuffers[0].mData);
        data->bufferList.mBuffers[0].mData = NULL;
    }
    
    if (data->packetOutBuffer != NULL)
    {
        KWL_FREE(data->packetOutBuffer);
        data->packetOutBuffer = NULL;
    }
    
    KWL_FREE(data);
}

OSStatus audioFileReadCallback(void *inClientData,
                               SInt64 inPosition,
                               UInt32 requestCount,
                               void *buffer,
                               UInt32 *actualCount)
{
    kwlInputStream* inputStream = (kwlInputStream*)inClientData;
    
    kwlInputStream_seek(inputStream, inPosition, SEEK_SET);
    int bytesRead = kwlInputStream_read(inputStream, buffer, requestCount);
    *actualCount = bytesRead;
    
    return noErr;
}

SInt64 audioFileGetSizeCallback(void *inClientData)
{
    kwlInputStream* stream = (kwlInputStream*)inClientData;
    return stream->size; /*TODO: does not work for files. fix...*/
}

OSStatus complexInputDataCallback(AudioConverterRef inAudioConverter,
                                  UInt32 *ioNumberDataPackets,
                                  AudioBufferList *ioData,
                                  AudioStreamPacketDescription **outDataPacketDescription,
                                  void *inUserData)
{
    KWL_ASSERT(inUserData != NULL && "ios decoder callback data is null");
    
    kwlDecoder* decoder = (kwlDecoder*)inUserData;
    kwlIPhoneDecoderData* decoderData = decoder->codecData; 
    
    /* request a single packet */
    *ioNumberDataPackets = 1;
    
    /*
     * Read the requested number of packets of compressed data into 
     * decoderData->packetOutBuffer.
     */
    int numBytesRead = -1;
    OSStatus result = AudioFileReadPackets(decoderData->audioFileID,
                                           0,
                                           &numBytesRead,
                                           &decoderData->outPacketDesc,
                                           decoderData->currentPacketIndex,
                                           ioNumberDataPackets,
                                           decoderData->packetOutBuffer);
    
    KWL_ASSERT(numBytesRead >= 0);
    KWL_ASSERT(numBytesRead <= decoderData->packetSizeUpperBound);
    KWL_ASSERT(result == noErr && "ios decoder AudioFileReadPackets failed");
    
    /* Pass on the read data */
    *outDataPacketDescription = &decoderData->outPacketDesc;
    ioData->mBuffers[0].mData = numBytesRead > 0 ? decoderData->packetOutBuffer : NULL;			
    ioData->mBuffers[0].mDataByteSize = numBytesRead;
    
    /* Advance packet counter.*/
    decoderData->currentPacketIndex += *ioNumberDataPackets;
    
    /* If no packets were read, signal that we're out of data. */
    decoderData->hasMoreData = *ioNumberDataPackets > 0 ? 1 : 0;
    
    return 0;
}

int kwlDecodeBufferIPhone(kwlDecoder* decoder)
{
    kwlIPhoneDecoderData* data = (kwlIPhoneDecoderData*)decoder->codecData;
    
    UInt32 numFramesRead = decoder->maxDecodedBufferSize / (2 * decoder->numChannels);
    
    OSStatus result = AudioConverterFillComplexBuffer(data->converter, 
                                                      &complexInputDataCallback, 
                                                      decoder, /*user data*/
                                                      &numFramesRead,/*gets set to the number 
                                                                           of read frames during the call*/
                                                      &data->bufferList, /*the buffer list to put
                                                                          decompressed output samples into*/
                                                      NULL);
        
    kwlMemcpy(decoder->currentDecodedBuffer, 
              data->bufferList.mBuffers[0].mData, 
              2 * numFramesRead * decoder->numChannels);
    
    data->numDecodedFrames += numFramesRead;
    
    int numTrailingFrames = 0;
    if (data->numValidFrames > 0)
    {
        /*
         * If we have valid priming info, and we're past the last actual audio frame, 
         * compute the number of frames to discard and signal that we're out of data.
         */
        if (data->numDecodedFrames > data->numValidFrames)
        {
            numTrailingFrames = data->numDecodedFrames - data->numValidFrames;
            data->hasMoreData = 0;
        }
    }

    int numSamples = (numFramesRead - numTrailingFrames) * decoder->numChannels;
    KWL_ASSERT(numSamples >= 0);
    
    decoder->currentDecodedBufferSizeInBytes = 2 * numSamples;
    
    return data->hasMoreData == 0 ? 1 : 0;
}

int kwlRewindDecoderIPhone(kwlDecoder* decoder)
{
    kwlIPhoneDecoderData* data = (kwlIPhoneDecoderData*)decoder->codecData;
    data->numDecodedFrames = 0;
    data->currentPacketIndex = 0;
    /*
     * From the docs:
     * Call this function after a discontinuity in the source
     * audio stream being provided to the converter.
     */
    OSStatus result = AudioConverterReset(data->converter);
    KWL_ASSERT(result == noErr);
    return 1;
}

#endif /*KWL_IPHONE*/
