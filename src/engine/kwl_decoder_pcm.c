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

#include "kwl_asm.h"
#include "kwl_audiofileutil.h"
#include "kwl_decoder_pcm.h"

kwlError kwlInitDecoderPCM(kwlDecoder* decoder)
{
    /*Allocate decoder data.*/
    kwlPCMDecoderData* data = 
        (kwlPCMDecoderData*)KWL_MALLOC(sizeof(kwlPCMDecoderData), "pcm decoder data");
    kwlMemset(data, 0, sizeof(kwlPCMDecoderData));

    /* Hook up data and callbacks to the decoder.*/
    decoder->codecData = data;
    decoder->decodeBuffer = kwlDecodeBufferPCM;
    decoder->deinit = kwlDeinitDecoderPCM;
    decoder->rewind = kwlRewindDecoderPCM;
    
    kwlError result = kwlLoadAIFFFromStream(&decoder->audioDataStream, 
                                            &data->pcmDataDescription,
                                            KWL_SKIP_AUDIO_DATA); /*1 for no audio data, just meta data*/
    
    if (result != KWL_NO_ERROR)
    {
        kwlInputStream_seek(&decoder->audioDataStream, 0, SEEK_SET); 
        result = kwlLoadAUFromStream(&decoder->audioDataStream, 
                                     &data->pcmDataDescription,
                                     KWL_SKIP_AUDIO_DATA); /*1 for no audio data, just meta data*/
    }
    
    if (result != KWL_NO_ERROR)
    {
        kwlInputStream_seek(&decoder->audioDataStream, 0, SEEK_SET); 
        result = kwlLoadWAVFromStream(&decoder->audioDataStream, 
                                      &data->pcmDataDescription,
                                      KWL_SKIP_AUDIO_DATA); /*1 for no audio data, just meta data*/
    }
    
    if (result == KWL_NO_ERROR)
    {
        /*seek to the first sample*/
        KWL_ASSERT(data->pcmDataDescription.fileOffset > 0);
        kwlInputStream_seek(&decoder->audioDataStream, 
                            data->pcmDataDescription.fileOffset, 
                            SEEK_SET);
    }
    
    if (result != KWL_NO_ERROR)
    {
        return result;
    }
    
    if (data->pcmDataDescription.encoding == KWL_ENCODING_UNSIGNED_8BIT_PCM ||
        data->pcmDataDescription.encoding == KWL_ENCODING_SIGNED_8BIT_PCM)
    {
        data->bytesPerSample = 1;
    }
    else if (data->pcmDataDescription.encoding == KWL_ENCODING_SIGNED_16BIT_PCM)
    {
        data->bytesPerSample = 2;
    }
    else if (data->pcmDataDescription.encoding == KWL_ENCODING_SIGNED_24BIT_PCM)
    {
        data->bytesPerSample = 3;
    }
    else if (data->pcmDataDescription.encoding == KWL_ENCODING_SIGNED_32BIT_PCM)
    {
        data->bytesPerSample = 4;
    }
    else
    {
        return KWL_UNSUPPORTED_ENCODING;
    }
    
    decoder->maxDecodedBufferSize = 4096 >> 1;
    decoder->numChannels = data->pcmDataDescription.numChannels;
    data->scratchBufferNumBytes = decoder->maxDecodedBufferSize;//TODO:make this work for all encodings!
    data->scratchBuffer = (char*)KWL_MALLOC(data->scratchBufferNumBytes, "pcm decoder scratch buffer");
    
    return KWL_NO_ERROR;
}

void kwlDeinitDecoderPCM(kwlDecoder* decoder)
{
    kwlPCMDecoderData* data = (kwlPCMDecoderData*)decoder->codecData;
    if (data->scratchBuffer != NULL)
    {
        KWL_FREE(data->scratchBuffer);
        data->scratchBuffer = NULL;
    }
    
    if (data != NULL)
    {
        KWL_FREE(data);
        data = NULL;
    }
}

int kwlDecodeBufferPCM(kwlDecoder* decoder)
{
    kwlPCMDecoderData* data = (kwlPCMDecoderData*)decoder->codecData;
    
    /*Read a new chunk of data into the scratch buffer*/
    int numBytesRead = kwlInputStream_read(&decoder->audioDataStream, 
                                           data->scratchBuffer, 
                                           data->scratchBufferNumBytes);
    
    /*Convert the chunk to signed 16 bit*/
    switch (data->pcmDataDescription.encoding) 
    {
        case KWL_ENCODING_UNSIGNED_8BIT_PCM:
        {
            KWL_ASSERT(2 * data->scratchBufferNumBytes == decoder->maxDecodedBufferSize);
            
            kwlUInt8ToInt16(data->scratchBuffer, 
                            numBytesRead, 
                            decoder->currentDecodedBuffer);
            break;
        }
        case KWL_ENCODING_SIGNED_8BIT_PCM:
        {
            KWL_ASSERT(2 * data->scratchBufferNumBytes == decoder->maxDecodedBufferSize);
            
            kwlInt8ToInt16(data->scratchBuffer, 
                           numBytesRead, 
                           decoder->currentDecodedBuffer);
            break;
        }
        case KWL_ENCODING_SIGNED_16BIT_PCM:
        {
            KWL_ASSERT(data->scratchBufferNumBytes == decoder->maxDecodedBufferSize);
            
            kwlInt16ToInt16(data->scratchBuffer, 
                            numBytesRead, 
                            decoder->currentDecodedBuffer, 
                            data->pcmDataDescription.isBigEndian);
            break;
        }
        case KWL_ENCODING_SIGNED_24BIT_PCM:
        {
            KWL_ASSERT(2 * data->scratchBufferNumBytes == 3 * decoder->maxDecodedBufferSize);
            
            kwlInt24ToInt16(data->scratchBuffer, 
                            numBytesRead, 
                            decoder->currentDecodedBuffer, 
                            data->pcmDataDescription.isBigEndian);
            break;
        }
        case KWL_ENCODING_SIGNED_32BIT_PCM:
        {
            KWL_ASSERT(data->scratchBufferNumBytes == 2 * decoder->maxDecodedBufferSize);
            
            kwlInt32ToInt16(data->scratchBuffer, 
                            numBytesRead, 
                            decoder->currentDecodedBuffer, 
                            data->pcmDataDescription.isBigEndian);
            break;
        }
        default:
        {
            KWL_ASSERT(0);
        }
    }
    
    decoder->currentDecodedBufferSizeInBytes = numBytesRead;
    
    /* Return 1 to signal that we reached the end of the audio data, zero otherwise.*/
    return numBytesRead < decoder->maxDecodedBufferSize ? 1 : 0;
}

int kwlRewindDecoderPCM(kwlDecoder* decoder)
{
    kwlPCMDecoderData* data = (kwlPCMDecoderData*)decoder->codecData;
    
    /*seek to the first sample*/
    KWL_ASSERT(data->pcmDataDescription.fileOffset > 0);
    kwlInputStream_seek(&decoder->audioDataStream, 
                        data->pcmDataDescription.fileOffset, 
                        SEEK_SET);
    return 1;
}
